#include "processor.h"
#include "globaldef.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>



Processor::Processor(QObject *parent)
    : QObject{parent}
{

}

void Processor::join(const QStringList &files,int volume)
{
    if(bRunning){
        return;
    }
    iVolume=volume;
    if(iVolume<2){
        iVolume=2;
    }
    if(iVolume>100){
        iVolume=100;
    }
    fileQueue.clear();
    foreach(const QString& file,files){
        fileQueue.enqueue(file);
    }
}




void Processor::work( VTask* task)
{
    if(task==nullptr){
        return;
    }
    lastTask=task;
    if(getState(task->file())!=Processing){
        stateMap.insert(task->file(),Processing);
        emit notify(task->file());
    }
    process=new QProcess(this);
    connect(process,&QProcess::started,this,&Processor::workStart);
    connect(process,&QProcess::finished,this,&Processor::workQuit);
    connect(process,&QProcess::errorOccurred,this,&Processor::workError);
    QFileInfo info(task->file());
    QString root=info.dir().absolutePath();
    QString baseName=info.baseName();
    QString ext=info.suffix();
    if(!ext.contains(".")){
        ext.insert(0,QLatin1Char('.'));
    }
    //padding number 0
    QString index=padding(task->index(),'0',MAX_NAME_LENGTH);
    QString fileName=QString("%1-%2%3").arg(baseName,index,ext);
    QString fullName=QString("%1/%2").arg(root,fileName);
    process->setStandardErrorFile(QString("%1%2xcc-error.log").arg(root,QDir::separator()));
    process->setStandardOutputFile(QString("%1%2xcc.log").arg(root,QDir::separator()));
    QString app=getAppFilePath(AppKind::FFMPEG);
    QString position(QString::number(task->position()));
    QString length(QString::number(task->offset()));
    QString command=QString("\"%1\" -hide_banner -y -ss %2 -t %3 -i \"%4\" -c:v copy -c:a copy \"%5\"").arg(app,position,length,task->file(),fullName);
    process->start(command);
}

void Processor::workStart()
{
    notifyTaskProgress();
    if(lastTask!=nullptr){
        emit ongoing(lastTask->file(),lastTask->index(),lastTask->volume());
    }
}

void Processor::workQuit()
{
    completeTask(Success);
}

void Processor::workError()
{
    completeTask(Fail);
}


void Processor::completeTask(State state)
{
    disconnect(process,&QProcess::started,this,&Processor::workStart);
    disconnect(process,&QProcess::finished,this,&Processor::workQuit);
    disconnect(process,&QProcess::errorOccurred,this,&Processor::workError);
    process->deleteLater();
    if(lastTask->isLast()){
        stateMap.insert(lastTask->file(),state);
        emit notify(lastTask->file());
    }
    lastTask->deleteLater();
    lastTask=nullptr;
    emit nextTask();
}

void Processor::notifyTaskProgress()
{
    iNow++;
    int percent=0;
    float value=0.0;
    if(iMax>0){
        float s1=static_cast<float>(iNow*100);
        float sm=static_cast<float>(iMax);
        value=s1/sm;
        if (value>100.0){
            value=100.0;
        }
        if (value<0.0){
            value=0.0;
        }
    }
    percent=static_cast<int>(value);
    emit progress(percent);
}
void Processor::start(){
    iNow=0;
    iMax=workingQueue.size();
    connect(this,&Processor::nextTask,this,&Processor::convert);
    emit nextTask();
}

void Processor::bootstrap()
{
    if (bRunning){
        return;
    }
    bRunning=true;
    stateMap.clear();
    connect(this,&Processor::newTask,this,&Processor::createTask);
    emit newTask();
}

void Processor::createTask()
{
    if(!fileQueue.isEmpty()){
        const QString& file=fileQueue.dequeue();
        processing(file,iVolume);
        emit newTask();
    }else{
        disconnect(this,&Processor::newTask,this,&Processor::createTask);
        start();
    }
}

void Processor::processing(const QString &file, int volume)
{
    qint64 duration=loadDuration(file);
    if(duration<=0){
        return;
    }
    qint64 left=duration % volume;
    qint64 offset=(duration-left)/volume;
    for(int index=0;index<volume;index++){
        VTask* task=new VTask(this);
        task->setFile(file);
        task->setIndex(index+1);
        task->setVolume(volume);
        if(index==volume-1){
            task->setOffset(offset+left);
        }else{
            task->setOffset(offset);
        }
        task->setPosition(index*offset);
        workingQueue.enqueue(task);
    }
}

void Processor::convert()
{
    if(workingQueue.isEmpty()){
        bRunning=false;
        disconnect(this,&Processor::nextTask,this,&Processor::convert);
        emit quit();
    }else{
        VTask* task=workingQueue.dequeue();
        work(task);
    }
}

QString Processor::getStateString(const QString &file)
{
    State state=getState(file);
    switch(state){
    case Ready:
        return tr("Ready");
    case Processing:
        return tr("Processing");
    case Success:
        return tr("Done");
    case Fail:
        return tr("Fail");
    default:
        return tr("Ready");
    }
}


QString Processor::getStateIcon(const QString &file)
{
    State state=getState(file);
    switch(state){
    case Ready:
        return ":/icon/img/ready.png";
    case Processing:
        return ":/icon/img/processing.png";
    case Success:
        return ":/icon/img/success.png";
    case Fail:
        return ":/icon/img/fail.png";
    default:
        return ":/icon/img/ready.png";
    }
}

Qt::GlobalColor Processor::getStateColor(const QString &file)
{
    State state=getState(file);
    switch(state){
    case Ready:
        return Qt::cyan;
    case Processing:
        return Qt::blue;
    case Success:
        return Qt::green;
    case Fail:
        return Qt::red;
    default:
        return Qt::cyan;
    }
    return Qt::cyan;
}

Processor::State Processor::getState(const QString &file)
{
    if(stateMap.contains(file)){
        return stateMap[file];
    }
    return Ready;
}

bool Processor::isWorking() const
{
    return bRunning;
}

void Processor::terminate()
{
    if(process!=nullptr){
        QProcess::ProcessState state=process->state();
        if(state==QProcess::Running||state==QProcess::Starting){
            process->kill();
        }
        process->deleteLater();
        process=nullptr;
        workingQueue.clear();
        bRunning=false;
        stateMap.clear();
    }
    if(lastTask!=nullptr){
        lastTask->deleteLater();
        lastTask=nullptr;
    }
}

qint64 Processor::loadDuration(const QString &file)
{
    QString sdk=getAppFilePath(AppKind::FFPROBE);
    if(!QFile::exists(sdk)){
        return 0;
    }
    QProcess process;
    QString command=QString("\"%1\" -hide_banner -loglevel fatal -show_error -show_format -print_format json \"%2\"").arg(sdk,file);
    process.start(command);
    process.waitForFinished();
    return parseFormat(process.readAllStandardOutput());
}

qint64 Processor::parseFormat(const QByteArray& bytes)
{
    QJsonDocument doc=QJsonDocument::fromJson(bytes);
    if(!doc.isNull()){
        const QJsonObject object=doc.object();
        if(object.contains("format")&&object["format"].isObject()){
            const QJsonObject& formatObject=object["format"].toObject();
            if(formatObject.contains("duration")&&formatObject["duration"].isString()){
                bool ok=false;
                double duration=formatObject["duration"].toString().toDouble(&ok);
                if(ok){
                    return static_cast<qint64>(duration);
                }
            }
        }
    }
    return 0;
}
