#include "processorthread.h"

ProcessorThread::ProcessorThread(QObject *parent,Processor* processor)
    : QThread{parent},worker{processor}
{

}

void ProcessorThread::run()
{
    worker->bootstrap();
}
