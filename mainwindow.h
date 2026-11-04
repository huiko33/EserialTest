#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QStringList"
#include "Serial_Interface/SerialTypes.h"
#include "QStringDecoder"
#include "QElapsedTimer"
#include "QProgressBar"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class Serial_Interface;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    Serial_Interface* m_serial;

    QActionGroup* interLineDelayGroup;
    QActionGroup* lineEndingGroup;
    QActionGroup* groupBaudRate;
    QActionGroup* groupDataBits;
    QActionGroup* groupStopBits;
    QActionGroup* groupParity;

    QStringDecoder m_recvdecoder{QStringDecoder::Utf8};//


    void connectSerialPortsSignals();
    void connectUiEventReceivers();

    QActionGroup* setupUIBaudRateMenu();

    void setupUIMenu();
    QActionGroup* fillUIMenu(QMenu* menu,const QList<QPair<int,QString>>&items);
    //void linkData();
    QByteArray lineEndingBytes();
    QAction* findActionInMenu(QMenu* menu,int data);
    void closeEvent(QCloseEvent *event);

    void checkedAndTriggerAction(QAction* act);

    //依旧
    QElapsedTimer *m_fileClock = nullptr;
    QProgressBar  *m_fileBar   = nullptr;
    bool           m_fileSending = false;
private slots:
    void doFLushBtnClicked();
    void doPortListUpdate(QString port,QStringList portList);

    void on_Btn_SerialPort_clicked();

    void onBaudRateActionTriggered(QAction*);
    void onDataBitsActionTriggered(QAction*);
    void onStopBitsActionTriggered(QAction*);
    void onParityActionTriggered(QAction*);

    void on_Btn_Send_clicked();
    void onRecivedData(const QByteArray&);

    void onPortOpened(QString portName,int baudRate,QSerialPort::DataBits dataBits,
                      QSerialPort::StopBits stopBits,QSerialPort::Parity parity);
    void onPortClosed();
    void applyInitiaSettings(qint32 baud,int dataBits,int parity,int stopBits,int lineEnding,int interLineDelay);

    //依旧文件相关
    void on_Btn_SendFile_clicked();
    void onFileSendProgress(qint64 sent,qint64 total);
    void onFileSendFinished(bool ok, const QString &message);
signals:
    void flushPortListRequest(QString);
    void portOpenRequest(QString);
    void portCloseRequest();
    void sendDataRequest(const QByteArray&);
    void loadSettingsRequest();
};
#endif // MAINWINDOW_H
