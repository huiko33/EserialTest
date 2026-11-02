#ifndef SERIALTYPES_H
#define SERIALTYPES_H
#include"QList"
#include"QSerialPort"
#include"QPair"

const QList<QPair<int,QString>> dataBitsList={
    {QSerialPort::Data5,"5"},{QSerialPort::Data6,"6"},
    {QSerialPort::Data7,"7"},{QSerialPort::Data8,"8"}};

const QList<QPair<int,QString>> parityList={
    {QSerialPort::NoParity,"NoParity"},
    {QSerialPort::EvenParity,"EvenParity"},
    {QSerialPort::OddParity,"OddParity"},
    {QSerialPort::SpaceParity,"SpaceParity"},
    {QSerialPort::MarkParity,"MarkParity"}
};

const QList<QPair<int,QString>> stopBitsList={
    {QSerialPort::OneStop,"1"},
    {QSerialPort::OneAndHalfStop,"1.5"},
    {QSerialPort::TwoStop,"2"}
};

const QList<QPair<int,QString>> lineEndingList={
    {0,"None"},
    {1,"CR\\r"},
    {2,"LF\\n"},
    {3,"CRLF\\r\\n"}
};

const QList<QPair<int,QString>> interLineDelayList={
    {1,"1ms"},
    {20,"20ms"},
    {40,"40ms"},
    {100,"100ms"},
    {200,"200ms"}
};


#endif // SERIALTYPES_H
