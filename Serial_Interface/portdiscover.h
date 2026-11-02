#ifndef PORTDISCOVER_H
#define PORTDISCOVER_H

#include <QObject>
class PortDiscover : public QObject
{
    Q_OBJECT
public:
    explicit PortDiscover(QObject *parent = nullptr);
public slots:
    void doFlushPortList(QString currentPort);
signals:
    void UpdatePortList(QString,QStringList);
private:
};

#endif // PORTDISCOVER_H
