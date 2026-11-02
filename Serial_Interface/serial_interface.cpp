#include "serial_interface.h"
#include "Serial_Interface/portdiscover.h"
#include "Serial_Interface/port_communicator.h"
#include "QThread"
#include "QDebug"
#include "Serial_Interface/settings.h"

#define SETTING_KEY_BAUD_RATE           "BaudRate"
#define SETTING_KEY_DATA_BITS           "DataBits"
#define SETTING_KEY_PARITY              "Parity"
#define SETTING_KEY_STOP_BITS           "StopBits"
#define SETTING_KEY_LINE_ENDING         "LineEnding"
#define SETTING_KEY_INTERLINE_DELAY     "InterlineDelay"

Serial_Interface::Serial_Interface(QObject *parent)
    : QObject{parent}
{
    m_settings = new Settings("Settings.ini",this);
    m_portdiscover = new PortDiscover(this);
    workerThread = new QThread(this);
    m_portCommunicator = new Port_Communicator();
    m_portCommunicator->moveToThread(workerThread);

    connect(workerThread,&QThread::started,m_portCommunicator,&Port_Communicator::onThreadStarted);
    connect(workerThread,&QThread::finished,m_portCommunicator,&Port_Communicator::deleteLater);
    workerThread->start();
    connectSerialPortsSignals();

    // qint32 baudRate = m_settings->getUintValue(SETTING_KEY_BAUD_RATE);
    // int dataBits = m_settings->getIntValue(SETTING_KEY_DATA_BITS);
    // int parity = m_settings->getIntValue(SETTING_KEY_PARITY);
    // int stopBits = m_settings->getIntValue(SETTING_KEY_STOP_BITS);
    // setBaudRate(baudRate);
    // setDataBits(dataBits);
    // setParity(parity);
    // setStopBits(stopBits);
    // emit initialSettingsLoaded(baudRate,dataBits,parity,stopBits);
}

Serial_Interface::~Serial_Interface()
{
    workerThread->quit();
    workerThread->wait();
}

void Serial_Interface::connectSerialPortsSignals()
{   /*                                                                    */  //刷新串口列表部分
    connect(this,&flushPortListRequest,m_portdiscover,&PortDiscover::doFlushPortList);
    connect(m_portdiscover,&PortDiscover::UpdatePortList,this,&Serial_Interface::flushPortListRecived);
    /*                                                                    */ //设置串口数据
    connect(this,&Serial_Interface::setBaudRateRequest,m_portCommunicator,&Port_Communicator::setBaudRate);
    connect(this,&Serial_Interface::setDataBitsRequest,m_portCommunicator,&Port_Communicator::setDataBits);
    connect(this,&Serial_Interface::setStopBitsRequest,m_portCommunicator,&Port_Communicator::setStopBits);
    connect(this,&Serial_Interface::setParityRequest,m_portCommunicator,&Port_Communicator::setParity);
    /*                                                                    */ //开关串口
    connect(this,&Serial_Interface::openPortRequest,m_portCommunicator,&Port_Communicator::openPort);
    connect(this,&Serial_Interface::closePortRequest,m_portCommunicator,&Port_Communicator::closePort);
    /*                                                                    */ //是否开关的反馈
    connect(m_portCommunicator,&Port_Communicator::portOpened,this,&Serial_Interface::portOpened);
    connect(m_portCommunicator,&Port_Communicator::portClosed,this,&Serial_Interface::portClosed);
    /*                                                                    */ //发送和接收
    connect(this,&Serial_Interface::sendDataRequest,m_portCommunicator,&Port_Communicator::sendData);
    connect(m_portCommunicator,&Port_Communicator::dataRecived,this,&Serial_Interface::receivedData);

}

void Serial_Interface::setBaudRate(qint32 BaudRate)
{
    emit setBaudRateRequest(BaudRate);
}
void Serial_Interface::setDataBits(int dataBits)
{
    emit setDataBitsRequest(dataBits);
}
void Serial_Interface::setStopBits(int stopBits)
{
    emit setStopBitsRequest(stopBits);
}
void Serial_Interface::setParity(int parity)
{
    emit setParityRequest(parity);
}

void Serial_Interface::stopSerialPort(int baudRate, int dataBits, int stopBits, int parity, int lineEnding, int interLineDelay)
{
    m_settings->setIntValue(SETTING_KEY_BAUD_RATE,baudRate);
    m_settings->setIntValue(SETTING_KEY_DATA_BITS,dataBits);
    m_settings->setIntValue(SETTING_KEY_STOP_BITS,stopBits);
    m_settings->setIntValue(SETTING_KEY_PARITY,parity);
    m_settings->setIntValue(SETTING_KEY_LINE_ENDING,lineEnding);
    m_settings->setIntValue(SETTING_KEY_INTERLINE_DELAY,interLineDelay);

    m_settings->saveSettings();
}

void Serial_Interface::onReadyLoadSettings()
{
    qint32 baudRate = m_settings->getUintValue(SETTING_KEY_BAUD_RATE,115200);
    int dataBits = m_settings->getIntValue(SETTING_KEY_DATA_BITS,QSerialPort::Data8);
    int parity = m_settings->getIntValue(SETTING_KEY_PARITY,QSerialPort::NoParity);
    int stopBits = m_settings->getIntValue(SETTING_KEY_STOP_BITS,QSerialPort::OneStop);
    int lineEnding = m_settings->getIntValue(SETTING_KEY_LINE_ENDING,0);
    int interLineDelay = m_settings->getIntValue(SETTING_KEY_INTERLINE_DELAY,1);

    emit initialSettingsLoaded(baudRate,dataBits,parity,stopBits,lineEnding,interLineDelay);
}
