#include "port_communicator.h"
#include "Serial_Interface/QDebugtools.h"
#include "QMessageBox"
#include "QFile"
static constexpr qint64 kChunkSize = 1024;
static constexpr qint64 kHighWater = 4096;
static constexpr int    kPumpInterval = 10;
static constexpr qint64 kReportStep = 8192;

Port_Communicator::Port_Communicator(QObject *parent)
    : QObject{parent},m_port(new QSerialPort(this))
{
    connect(m_port,&QSerialPort::readyRead,this,&Port_Communicator::onReadyRead);

    m_fileTimer = new QTimer(this);
    m_fileTimer->setInterval(kPumpInterval);
    connect(m_fileTimer,&QTimer::timeout,this,&Port_Communicator::pumpFileChunk);

    //connect(m_port,&QSerialPort::bytesWritten,this,&Port_Communicator::onBytesWritten);
}

Port_Communicator::~Port_Communicator()
{
    if(m_port->isOpen())
        m_port->close();
}

bool Port_Communicator::applySettings()
{
    if(!m_port->setBaudRate(m_baudRate)){
        //QDEBUG(QString("该端口不支持波特率 %1").arg);
        return false;
    }
    if(!m_port->setDataBits(m_dataBits)){
        QDEBUG(QString("设置数据位失败"))
        return false;
    }
    if(!m_port->setParity(m_parity)){
        QDEBUG(QString("设置校验位失败"));
        return false;
    }
    if(!m_port->setStopBits(m_stopBits)){
        QDEBUG(QString("设置停止位失败"));
        return false;
    }
    return true;
}

void Port_Communicator::finishFileSend(bool ok, const QString &message)
{
    if(m_fileTimer)     m_fileTimer->stop();
    if(m_file.isOpen()) m_file.close();
    m_pending.clear();
    m_abortRequested    = false;
    m_fileSending       = false;
    emit fileSendFinished(ok,message);
}

void Port_Communicator::openPort(const QString &portName)
{
    if(portName.isEmpty()){
        QDEBUG("未指定串口");
        return;
    }
    if(m_port->isOpen())
    {
        m_port->close();
    }
    m_port->setPortName(portName);

    if(!applySettings()){
        return;
    }
    if(!m_port->open(QIODevice::ReadWrite))
    {
        return;
    }
    m_port->clear();
    emit portOpened(m_port->portName(),m_port->baudRate(),m_port->dataBits(),m_port->stopBits(),m_port->parity());
}

void Port_Communicator::closePort()
{
    if(m_fileSending)
        finishFileSend(false,tr("端口关闭，发送终止"));

    if(!m_port->isOpen()){
        return;
    }
    m_port->close();
    emit portClosed();
}

void Port_Communicator::setBaudRate(qint32 baudRate)
{
    m_baudRate = baudRate;

    if(m_port->isOpen() && !m_port->setBaudRate(m_baudRate)){
        QDEBUG("设置波特率失败");
    }
}

void Port_Communicator::setDataBits(int dataBits)
{
    m_dataBits = static_cast<QSerialPort::DataBits>(dataBits);
    if(m_port->isOpen() && !m_port->setDataBits(m_dataBits)){
        QDEBUG("设置数据位失败");
    }
}

void Port_Communicator::setStopBits(int stopBits)
{
    m_stopBits = static_cast<QSerialPort::StopBits>(stopBits);
    if(m_port->isOpen() && !m_port->setStopBits(m_stopBits)){
        QDEBUG("设置停止位失败");
    }
}

void Port_Communicator::setParity(int parity)
{
    m_parity = static_cast<QSerialPort::Parity>(parity);
    if(m_port->isOpen() && !m_port->setParity(m_parity)){
        QDEBUG("设置校验位失败");
    }
}

void Port_Communicator::onThreadStarted()
{
    //TODO暂时好像没什么用
}

void Port_Communicator::onBytesWritten(QByteArray data)
{

}

void Port_Communicator::onReadyRead()
{

    emit dataRecived(m_port->readAll());
}

void Port_Communicator::sendData(QByteArray data)
{
    if(!m_port->isOpen())
        return;

    auto res = m_port->write(data);

    if(res < 0){
        return;
    }
}

void Port_Communicator::sendFile(const QString &filePath)
{
    if(!m_port->isOpen()){
        emit fileSendFinished(false,tr("串口未打开"));
        return;
    }
    if(m_fileSending){
        emit fileSendFinished(false,tr("已有串口在发送中"));
        return;
    }

    m_file.setFileName(filePath);
    if(!m_file.open(QIODevice::ReadOnly)){
        emit fileSendFinished(false,tr("无法打开文件:%1").arg(m_file.errorString()));
        return;
    }

    m_fileTotal = m_file.size();
    m_fileSent  = 0;
    m_lastreported  = 0;
    m_pending.clear();

    if(m_fileTotal <= 0){
        m_file.close();
        emit fileSendFinished(false,tr("文件为空"));
        return;
    }

    m_fileSending = true;
    emit fileSendProgress(0,m_fileTotal);
    m_fileTimer->start();
}

void Port_Communicator::cancelFileSend()
{
    if (!m_fileSending || m_abortRequested)
        return;

    m_abortRequested = true;
    m_abortClock.start();

    // 停止投喂
    if (m_file.isOpen())
        m_file.close();
    m_pending.clear();

}

void Port_Communicator::pauseFileSend(bool pause)
{
    if(!m_fileSending) return;
    if(pause) m_fileTimer->stop();
    else m_fileTimer->start();
}

void Port_Communicator::pumpFileChunk()
{
    if(!m_port->isOpen()){
        finishFileSend(false,tr("串口已关闭，发送终止"));
        return;
    }

    if (m_abortRequested) {
        if (m_port->bytesToWrite() == 0) {
            finishFileSend(false, tr("已取消"));
        } else if (m_abortClock.elapsed() > 5000) {
            finishFileSend(false, tr("取消后数据未能排空，设备可能无响应"));
        }
        return;
    }

    //背压，如果等待QSerialPort写入的数据高于高水位，这次就什么也不做直接return
    //如果下一次进入pumpFileChunk(),QSerialPort写入了足够多的数据导致Qt内部写缓冲中的数据小于高水位则接着往下
    if(m_port->bytesToWrite() > kHighWater)
        return;

    if(m_pending.isEmpty()){
        m_pending = m_file.read(kChunkSize);

        if(m_pending.isEmpty()){
            if(m_port->bytesToWrite() == 0){
                emit fileSendProgress(m_fileTotal,m_fileTotal);
                finishFileSend(true,tr("发送完成"));
            }
            return;
        }
    }

    const qint64 n = m_port->write(m_pending);
    if(n < 0){
        finishFileSend(false,tr("写入失败:%1").arg(m_port->errorString()));
        return;
    }
    m_pending.remove(0,int(n));
    m_fileSent +=n;

    if(m_fileSent - m_lastreported >= kReportStep){
        m_lastreported = m_fileSent;
        emit fileSendProgress(m_fileSent, m_fileTotal);
    }
}
