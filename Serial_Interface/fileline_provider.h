#ifndef FILELINE_PROVIDER_H
#define FILELINE_PROVIDER_H

#include <QThread>

class FileLine_Provider : public QThread
{
    Q_OBJECT
public:
    explicit FileLine_Provider(QObject *parent = nullptr);
};

#endif // FILELINE_PROVIDER_H
