#include "portdiscover.h"
#include "QSerialPort"
#include "QSerialPortInfo"
PortDiscover::PortDiscover(QObject *parent)
    : QObject{parent}
{
}

void PortDiscover::doFlushPortList(QString currentPort)
{
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    QStringList portList;
    foreach (QSerialPortInfo info, infoList) {
        portList<<info.portName();
    };
    emit UpdatePortList(currentPort,portList);
}
