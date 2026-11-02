#include "settings.h"
#include "QMap"
#include "QFile"
#include "QTextStream"
Settings::Settings(QString fileName,QObject *parent)
    : QObject{parent}
{
    m_fileName = fileName;
    m_settings.clear();

    loadSettings();
}

void Settings::loadSettings()
{
    QFile file = QFile(m_fileName);

    m_settings;
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly)){
            QTextStream stream=QTextStream(&file);
            while(!stream.atEnd()){
                QString Line =stream.readLine();
                if(Line.contains("=")){
                    QStringList strList = Line.split(" = ");
                    setQStringValue(strList.value(0),
                                    strList.value(1));
                }
            }
            file.close();
        }
    }
}

void Settings::setQStringValue(QString key, QString value)
{
    if(m_settings.contains(key)){
        m_settings.value(key).remove(key);
    }
    m_settings.insert(key,value);
}

void Settings::setIntValue(QString key, int value)
{
    if(m_settings.contains(key)){
        m_settings.value(key).remove(key);
    }
    m_settings.insert(key,QString::number(value));
}


void Settings::saveSettings()
{
    QFile file = QFile(m_fileName);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream = QTextStream(&file);

        QMapIterator<QString,QString> iterator = QMapIterator(m_settings);
        while(iterator.hasNext()){
            iterator.next();
            stream<<iterator.key() <<" = "<<iterator.value()<<Qt::endl;
        }
        stream.flush();
        file.close();
    }
}

int Settings::getIntValue(const QString& key, int default_value)
{
    bool ok;
    int return_value;

    if(m_settings.contains(key)){
        return_value = m_settings[key].toInt(&ok);
        if(ok) return return_value;
    }
    return default_value;
}

uint Settings::getUintValue(const QString &key, uint default_value)
{
    bool ok;
    uint return_value;
    if(m_settings.contains(key)){
        return_value = m_settings[key].toUInt(&ok);
        if(ok) return return_value;
    }
    return default_value;
}

