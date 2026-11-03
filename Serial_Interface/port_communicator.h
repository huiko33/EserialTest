#ifndef PORT_COMMUNICATOR_H
#define PORT_COMMUNICATOR_H
#include "QTimer"
#include <QObject>
#include "QSerialPort"
#include "Serial_Interface/SerialTypes.h"
#include "QFile"
#include "QElapsedTimer"
class Port_Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Port_Communicator(QObject *parent = nullptr);
    ~Port_Communicator();

private:
    QSerialPort*            m_port;
    qint32                  m_baudRate;
    QSerialPort::DataBits   m_dataBits;
    QSerialPort::StopBits   m_stopBits;
    QSerialPort::Parity     m_parity;
    bool applySettings();

    //文件发送相关
    QTimer*     m_fileTimer = nullptr;
    QByteArray  m_pending;           //已经读出，但还没被write()全部接受的数据
    QFile       m_file;
    qint64      m_fileTotal = 0;
    qint64      m_fileSent  = 0;
    qint64      m_lastreported = 0;
    bool        m_fileSending = false;

    void finishFileSend(bool ok,const QString &message);


    bool          m_abortRequested = false;   //取消标志
    QElapsedTimer m_abortClock;
public slots:
    void openPort(const QString& portName);
    void closePort();
    void setBaudRate(qint32 baudRate);
    void setDataBits(int dataBits);
    void setStopBits(int stopBits);
    void setParity(int parity);
    void onThreadStarted();
    void onBytesWritten(QByteArray data);
    void onReadyRead();
    void sendData(QByteArray data);

    //文件发送相关
    void sendFile(const QString &filePath);
    void cancelFileSend();
    void pauseFileSend(bool pause);
private slots:
    void pumpFileChunk();
signals:
    void dataRecived(const QByteArray& data);
    void portOpened(QString portName,int baudRate,QSerialPort::DataBits dataBits,
                    QSerialPort::StopBits stopBits,QSerialPort::Parity parity);
    void portClosed();

    //文件相关
    void fileSendFinished(bool ok , const QString &messgae);
    void fileSendProgress(qint64 sent,qint64 total);
};

#endif // PORT_COMMUNICATOR_H
