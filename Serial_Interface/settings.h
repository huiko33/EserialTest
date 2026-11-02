#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include "QMap"
class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QString fileName,QObject *parent = nullptr);
    void loadSettings();
    void saveSettings();

    void setQStringValue(QString key,QString value);
    void setIntValue(QString key,int value);
    int getIntValue(const QString& key, int default_value = 0);
    uint getUintValue(const QString& key,uint default_value = 0);
private:
    QString m_fileName;
    QMap<QString,QString> m_settings;
signals:
};

#endif // SETTINGS_H
