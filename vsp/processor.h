#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QProcess>
#include "vtask.h"


class Processor : public QObject
{
    Q_OBJECT
public:
    enum State{
        Ready,
        Processing,
        Success,
        Fail
    };
    Q_ENUM(State);
    explicit Processor(QObject *parent = nullptr);
    //join file list
    void join(const QStringList& files,int volume);

    //get duration
    qint64 loadDuration(const QString& file);
    //get state string
    QString getStateString(const QString& file);
    //get state icon
    QString getStateIcon(const QString& file);
    //get state icon
    Qt::GlobalColor getStateColor(const QString& file);
    //working now
    bool isWorking() const;
    void terminate();
public slots:
    //startup processing
    void bootstrap();

private slots:
    void work(VTask* task);
    void workStart();
    void workQuit();
    void workError();
    void createTask();
    void convert();

signals:
    //
    void ongoing(QString,qint64,qint64);
    //state change for a file
    void notify(QString);
    //all done
    void quit();
    //
    void progress(int);
    //create new task;
    void newTask();
    //next task
    void nextTask();
private:
    int iNow=0;
    int iMax=0;
    int iVolume=2;
    bool bRunning=false;
    QProcess* process=nullptr;
    VTask* lastTask=nullptr;
    QMap<QString,State> stateMap;
    QQueue<VTask*> workingQueue;
    QQueue<QString> fileQueue;
private:
    //get state
    State getState(const QString& file);
    void completeTask(State state);
    void notifyTaskProgress();
    void start();
    qint64 parseFormat(const QByteArray& bytes);
    void processing(const QString& file,int volume);


};

#endif // PROCESSOR_H
