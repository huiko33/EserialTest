#include "port_communicator.h"
#include "Serial_Interface/QDebugtools.h"
#include "QMessageBox"

Port_Communicator::Port_Communicator(QObject *parent)
    : QObject{parent},m_port(new QSerialPort(this))
{
    connect(m_port,&QSerialPort::readyRead,this,&Port_Communicator::onReadyRead);
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
