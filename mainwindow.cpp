#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QSerialPortInfo"
#include "Serial_Interface/QDebugtools.h"
#include "Serial_Interface/serial_interface.h"
#include "QActionGroup"
#define START_STR "Start"
#define STOP_STR "Stop"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_serial = new Serial_Interface(this);
    setupUIMenu();

    connectUiEventReceivers();

    connectSerialPortsSignals();
    doFLushBtnClicked();

    emit loadSettingsRequest();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSerialPortsSignals()
{
    //刷新串口下拉框部分
    connect(ui->Btn_Flush,&QPushButton::clicked,this,&MainWindow::doFLushBtnClicked);
    connect(this,&MainWindow::flushPortListRequest,m_serial,&Serial_Interface::flushPortListRequest);
    connect(m_serial,&Serial_Interface::flushPortListRecived,this,&MainWindow::doPortListUpdate);
    /*-------------------------------------------------------------------------------------*/
    connect(this,&MainWindow::portOpenRequest,m_serial,&Serial_Interface::openPortRequest);
    connect(this,&MainWindow::portCloseRequest,m_serial,&Serial_Interface::closePortRequest);
    /*-------------------------------------------------------------------------------------*/
    connect(this,&MainWindow::sendDataRequest,m_serial,&Serial_Interface::sendDataRequest);
    connect(m_serial,&Serial_Interface::receivedData,this,&MainWindow::onRecivedData);

}

void MainWindow::connectUiEventReceivers()
{
    /*
    connect(ui->menuBaudRate,&QMenu::triggered,this,&MainWindow::onBaudRateActionTriggered);
    connect(ui->menuDataBits,&QMenu::triggered,this,&MainWindow::onDataBitsActionTriggered);
    connect(ui->menuStopBits,&QMenu::triggered,this,&MainWindow::onStopBitsActionTriggered);
    connect(ui->menuParity,&QMenu::triggered,this,&MainWindow::onParityActionTriggered);
    */
    /*-------------------------------------------------------------------------------------*/
    connect(m_serial,&Serial_Interface::portOpened,this,&MainWindow::onPortOpened);
    connect(m_serial,&Serial_Interface::portClosed,this,&MainWindow::onPortClosed);
    /*-------------------------------------------------------------------------------------*/
    connect(this,&MainWindow::loadSettingsRequest,m_serial,&Serial_Interface::onReadyLoadSettings);
    connect(m_serial,&Serial_Interface::initialSettingsLoaded,this,&MainWindow::applyInitiaSettings);
}

//用的QSerialPortInfo::standardBaudRates(),因此无法使用通用helper,可以重载或者写个BaudRate的List但是我懒了而且这玩意已经有了感觉没什么必要
QActionGroup* MainWindow::setupUIBaudRateMenu()
{
    QMenu *menu = ui->menuBaudRate;
    menu->clear();
    auto* group = new QActionGroup(this);
    group->setExclusive(true);

    for(qint32 rate : QSerialPortInfo::standardBaudRates()){
        QAction *act = menu->addAction(QString::number(rate));
        act->setCheckable(true);
        act->setData(rate);
        group->addAction(act);
    }
    return group;
}

//通用helper
QActionGroup* MainWindow::fillUIMenu(QMenu *menu,const QList<QPair<int, QString>> &items)
{
    menu->clear();
    auto* group = new QActionGroup(this);
    group->setExclusive(true);
    for(QPair<int,QString> item:items){
        QAction* act = menu->addAction(item.second);
        act->setData(item.first);
        act->setCheckable(true);
        group->addAction(act);
    }
    return group;
}


//调用以上俩函数构建四个串口数据的菜单，还多了个行末符的菜单
void MainWindow::setupUIMenu()
{
    groupBaudRate = setupUIBaudRateMenu();
    groupDataBits = fillUIMenu(ui->menuDataBits,dataBitsList);
    groupStopBits = fillUIMenu(ui->menuStopBits,stopBitsList);
    groupParity = fillUIMenu(ui->menuParity,parityList);
    lineEndingGroup = fillUIMenu(ui->menuLineEnding,lineEndingList);
    interLineDelayGroup = fillUIMenu(ui->menuInterLineDelay,interLineDelayList);

    connect(groupBaudRate,&QActionGroup::triggered,this,&MainWindow::onBaudRateActionTriggered);
    connect(groupDataBits,&QActionGroup::triggered,this,&MainWindow::onDataBitsActionTriggered);
    connect(groupStopBits,&QActionGroup::triggered,this,&MainWindow::onStopBitsActionTriggered);
    connect(groupParity,&QActionGroup::triggered,this,&MainWindow::onParityActionTriggered);

    checkedAndTriggerAction(groupBaudRate->actions().at(0));
    checkedAndTriggerAction(groupDataBits->actions().at(0));
    checkedAndTriggerAction(groupStopBits->actions().at(0));
    checkedAndTriggerAction(groupParity->actions().at(0));
    checkedAndTriggerAction(lineEndingGroup->actions().at(0));
    checkedAndTriggerAction(interLineDelayGroup->actions().at(0));
    // groupBaudRate->actions().at(0)->triggered(true);
    // groupDataBits->actions().at(0)->triggered(true);
    // groupStopBits->actions().at(0)->triggered(true);
    // groupParity->actions().at(0)->triggered(true);
    // lineEndingGroup->actions().at(0)->triggered(true);
    // interLineDelayGroup->actions().at(0)->triggered(true);


    // groupBaudRate->actions().at(0)->setChecked(true);
    // groupDataBits->actions().at(0)->setChecked(true);
    // groupStopBits->actions().at(0)->setChecked(true);
    // groupParity->actions().at(0)->setChecked(true);
    // lineEndingGroup->actions().at(0)->setChecked(true);
    // interLineDelayGroup->actions().at(0)->setChecked(true);

    // //应该可以整好点给这四条默认塞娜个部分里，但是感觉没有必要直接手动设置默认值
    // /*TODO
    // * 后续Settings读取配置直接覆盖这里的默认值，不过默认值依旧留着
    // */
    // m_serial->setBaudRate(QSerialPortInfo::standardBaudRates().at(0));
    // m_serial->setDataBits(dataBitsList.at(3).first);
    // groupDataBits->actions().at(3)->setChecked(true);
    // m_serial->setStopBits(stopBitsList.at(0).first);
    // m_serial->setParity(parityList.at(0).first);


}

/* 最开始的版本，上面的信号用的是QMenu链接下面这个槽函数，但是又在槽函数中强转了sender,
 * 我原本以为sender会是触发信号的QAction,问了AI才知道是QMenu,导致与下面这个槽函数差不多的四个串口槽函数在中途因为act的不存在而return
 * 解决方式就是现在的代码，不用QMenu而是用在创建菜单UI时为了设置单选创建的QActionGroup
 * 值得一提的是QActionGroup::triggered发送的是group内触发信号的QAction,因此改用QActionGroup，
 * 具体解决思路为修改创建菜单UI的函数返回值，void->QActionGroup*
 * 创建四个QActionGroup*接收返回值然后用这四个group链接各自的槽函数修复bug
void MainWindow::onBaudRateActionTriggered()
{
    qDebug() << "1";
    auto *act = qobject_cast<QAction *>(sender());

    if(!act)
        return;
    qDebug() << "1";
    const qint32 rate = act->data().toInt();
    m_serial->setBaudRate(rate);

}
*/
//四个数据的处理侧  //或许四个数据的处理侧可以改用函数指针以合并成一个通用的处理侧，不过这次还是算了
void MainWindow::onBaudRateActionTriggered(QAction* act)
{
    if(!act)
        return;
    const qint32 rate = act->data().toInt();
    m_serial->setBaudRate(rate);

}
void MainWindow::onDataBitsActionTriggered(QAction* act)
{
    if(!act)
        return;
    const int databit = act->data().toInt();
    m_serial->setDataBits(databit);
}
void MainWindow::onStopBitsActionTriggered(QAction* act)
{
    if(!act)
        return;

    const int stopBits = act->data().toInt();
    m_serial->setStopBits(stopBits);
}
void MainWindow::onParityActionTriggered(QAction* act)
{
    if(!act)
        return;
    const int parity = act->data().toInt();
    m_serial->setParity(parity);
}

//响应Btn_Flush的中转槽函数
void MainWindow::doFLushBtnClicked()
{
    //保存当前选中项文本并向下传递
    emit flushPortListRequest(ui->combo_SerialPort->currentText());
}

//响应串口列表刷新信号的槽函数
void MainWindow::doPortListUpdate(QString port,QStringList portList)
{   //刷新串口下拉框的最后部分
    ui->combo_SerialPort->clear();
    ui->combo_SerialPort->addItems(portList);
    int index = ui->combo_SerialPort->findText(port);

    if(index < 0){
        index = 0;
    }
    ui->combo_SerialPort->setCurrentIndex(index);
}


void MainWindow::on_Btn_SerialPort_clicked()
{
    if(ui->Btn_SerialPort->text() == START_STR){
        emit portOpenRequest(ui->combo_SerialPort->currentText());
    }else{
        emit portCloseRequest();
    }
}


void MainWindow::on_Btn_Send_clicked()
{
    //linkData();
    QString str = ui->edit_Send->toPlainText();
    if(str.isEmpty())
        return;
    QByteArray payload = str.toUtf8();
    payload += lineEndingBytes();

    //qDebug().noquote() << "TX" << payload.toHex(' ');

    emit sendDataRequest(payload);
    ui->edit_Send->clear();
}

void MainWindow::onRecivedData(const QByteArray & data)
{
    const QString text = m_recvdecoder(data);
    if(m_recvdecoder.hasError()){/*TODO*/};
    ui->edit_recivie->moveCursor(QTextCursor::End);
    ui->edit_recivie->insertPlainText(text);
}

void MainWindow::onPortOpened(QString portName,int baudRate,QSerialPort::DataBits dataBits,
                              QSerialPort::StopBits stopBits,QSerialPort::Parity parity)
{
    ui->menuBaudRate->setEnabled(false);
    ui->menuDataBits->setEnabled(false);
    ui->menuStopBits->setEnabled(false);
    ui->menuParity->setEnabled(false);
    ui->menuLineEnding->setEnabled(false);
    ui->menuInterLineDelay->setEnabled(false);
    ui->combo_SerialPort->setEnabled(false);
    ui->Btn_Send->setEnabled(true);
    ui->Btn_SendFile->setEnabled(true);

    ui->Btn_SerialPort->setText(STOP_STR);


    QString strPairty;
    switch (parity) {
    case QSerialPort::NoParity:
        strPairty = "N";
        break;
    case QSerialPort::EvenParity:
        strPairty = "E";
        break;
    case QSerialPort::OddParity:
        strPairty = "O";
        break;
    case QSerialPort::SpaceParity:
        strPairty = "S";
        break;
    case QSerialPort::MarkParity:
        strPairty = "M";
        break;
    }

    float stopBitsVal = static_cast<float>(stopBits);
    if(stopBitsVal == 3){
        stopBitsVal = 1.5;
    }


    QString params = QStringLiteral("%1 %2-%3-%4").arg(baudRate).arg(static_cast<int>(dataBits))
                         .arg(strPairty).arg(stopBitsVal);

    ui->label->setText(ui->label->text()+portName+" "+params);

     // ui->label->setText(ui->label->text()+portName+" "+QString::number(baudRate)+" "+
     //         QString::number(static_cast<int>(stopBits))+" "+QString::number(static_cast<int>(dataBits))+" "+
     //                     strPairty);
}

void MainWindow::onPortClosed()
{
    ui->menuBaudRate->setEnabled(true);
    ui->menuDataBits->setEnabled(true);
    ui->menuStopBits->setEnabled(true);
    ui->menuParity->setEnabled(true);
    ui->menuLineEnding->setEnabled(true);
    ui->menuInterLineDelay->setEnabled(true);
    ui->combo_SerialPort->setEnabled(true);
    ui->Btn_Send->setEnabled(false);
    ui->Btn_SendFile->setEnabled(false);
    ui->Btn_SerialPort->setText(START_STR);

    ui->label->setText("CurrentPort: ");
}

void MainWindow::applyInitiaSettings(qint32 baud, int dataBits, int parity, int stopBits, int lineEnding, int interLineDelay)
{
    QAction* actionBaud = findActionInMenu(ui->menuBaudRate,baud);
    checkedAndTriggerAction(actionBaud);

    QAction* actiondataBits = findActionInMenu(ui->menuDataBits,dataBits);
    checkedAndTriggerAction(actiondataBits);

    QAction* actionparity = findActionInMenu(ui->menuParity,parity);
    checkedAndTriggerAction(actionparity);

    QAction* actionstopBits = findActionInMenu(ui->menuStopBits,stopBits);
    checkedAndTriggerAction(actionstopBits);

    QAction* actionlineEnding = findActionInMenu(ui->menuLineEnding,lineEnding);
    checkedAndTriggerAction(actionlineEnding);

    QAction* actioninterLineDelay = findActionInMenu(ui->menuInterLineDelay,interLineDelay);
    checkedAndTriggerAction(actioninterLineDelay);
}

//原本为了链接行末符号写的，具体实现是直接在QTextEdit后加上符号，
//但是之后发现行末符经过QTextedit会导致符号消失，可能看着没啥问题但是字节上对不上
//于是在下面的  QByteArray MainWindow::lineEndingBytes()  中改成用QBYteArray链接不经过QTextEdit
// void MainWindow::linkData()
// {
//     QAction* act = lineEndingGroup->checkedAction();
//     switch (act->data().toInt()) {
//     case 0:
//         break;
//     case 1:
//         ui->edit_Send->setText(ui->edit_Send->toPlainText()+"\r");
//         break;
//     case 2:
//         ui->edit_Send->setText(ui->edit_Send->toPlainText()+"\n");
//         break;
//     case 3:
//         ui->edit_Send->setText(ui->edit_Send->toPlainText()+"\r\n");
//         break;
//     }
// }

QByteArray MainWindow::lineEndingBytes()
{
    QAction* act = lineEndingGroup ? lineEndingGroup->checkedAction() : nullptr;
    switch (act ? act->data().toInt() :0) {
    case 1: return QByteArray("\r");
    case 2: return QByteArray("\n");
    case 3: return QByteArray("\r\n");
    default:return QByteArray();
    }
}

QAction* MainWindow::findActionInMenu(QMenu *menu, int data)
{
    QList<QAction*> actList = menu->actions();
    foreach (QAction *act, actList) {
        if(act->data().toInt()==data){
            return act;
        }
    }
    return nullptr;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    m_serial->stopSerialPort(groupBaudRate->checkedAction()->data().toInt(),
                             groupDataBits->checkedAction()->data().toInt(),
                             groupStopBits->checkedAction()->data().toInt(),
                             groupParity->checkedAction()->data().toInt(),
                             lineEndingGroup->checkedAction()->data().toInt(),
                             interLineDelayGroup->checkedAction()->data().toInt());



}

void MainWindow::checkedAndTriggerAction(QAction *act)
{
    if(act == nullptr) //之前没加这个判断为空就返回导致如果没有配置文件或者配置文件为空则无法启动
        return;        //加上之后没有或者为空都可以正常启动
    act->setChecked(true);

    act->trigger();
}

