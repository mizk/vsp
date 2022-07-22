#ifndef PROCESSORTHREAD_H
#define PROCESSORTHREAD_H

#include <QObject>
#include <QThread>
#include "processor.h"

class ProcessorThread : public QThread
{
    Q_OBJECT
public:
    explicit ProcessorThread(QObject *parent = nullptr,Processor* prpcessor=nullptr);
protected:
    virtual void run() override;
signals:

private:
    Processor* worker;

};

#endif // PROCESSORTHREAD_H
