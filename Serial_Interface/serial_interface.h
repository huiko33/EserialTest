#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H
#include "QStringList"
#include <QObject>
#include"QSerialPort"

#define SETTINGS_FILENAME "Settings.ini"

class Port_Communicator;
class PortDiscover;
class QThread;
class Settings;
class Serial_Interface : public QObject
{
    Q_OBJECT
public:
    explicit Serial_Interface(QObject *parent = nullptr);
    ~Serial_Interface() override;
public:
    void setBaudRate(qint32);
    void setDataBits(int);
    void setStopBits(int);
    void setParity(int);
    void stopSerialPort(int baudRate,int dataBits,int stopBits,int parity,int lineEnding,int interLineDelay);
public slots:
    void onReadyLoadSettings();
private:
    PortDiscover* m_portdiscover;
    Port_Communicator* m_portCommunicator;
    QThread* workerThread;
    Settings* m_settings;
    void connectSerialPortsSignals();
signals:
    void flushPortListRequest(QString);
    void flushPortListRecived(QString,QStringList);
    void setBaudRateRequest(qint32);
    void setDataBitsRequest(int dataBits);
    void setStopBitsRequest(int stopBits);
    void setParityRequest(int parity);
    void openPortRequest(QString);
    void closePortRequest();
    void portOpened(QString portName,int baudRate,QSerialPort::DataBits dataBits,
                    QSerialPort::StopBits stopBits,QSerialPort::Parity parity);
    void portClosed();
    void sendDataRequest(QByteArray data);
    void receivedData(const QByteArray& data);
    void initialSettingsLoaded(qint32,int,int,int,int,int);

};

#endif // SERIAL_INTERFACE_H
