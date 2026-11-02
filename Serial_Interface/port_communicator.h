#ifndef PORT_COMMUNICATOR_H
#define PORT_COMMUNICATOR_H

#include <QObject>
#include "QSerialPort"
#include "Serial_Interface/SerialTypes.h"
class Port_Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Port_Communicator(QObject *parent = nullptr);
    ~Port_Communicator();

private:
    QSerialPort* m_port;
    qint32                  m_baudRate;
    QSerialPort::DataBits   m_dataBits;
    QSerialPort::StopBits     m_stopBits;
    QSerialPort::Parity     m_parity;
    bool applySettings();
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
signals:
    void dataRecived(const QByteArray& data);
    void portOpened(QString portName,int baudRate,QSerialPort::DataBits dataBits,
                    QSerialPort::StopBits stopBits,QSerialPort::Parity parity);
    void portClosed();
};

#endif // PORT_COMMUNICATOR_H
