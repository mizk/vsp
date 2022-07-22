#include "globaldef.h"
#include <QCoreApplication>
#include <QSysInfo>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QProcessEnvironment>
bool isWindows(){
#ifdef Q_OS_WINDOWS
    return true;
#else
    return false;
#endif

}
bool isMac(){
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}
bool isUnix(){
#ifdef Q_OS_UNIX
    return true;
#else
    return false;
#endif
}
bool isLinux(){
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}
QString __guessAppFile__(const QString& app){
    QStringList locations;
    QString base0("%1/%2");
    QString base1("%1/ffmpeg/%2");
    QString base2("%1/.ffmpeg/%2");
    QString base3("%1/sdk/bin/%2");
    if(isWindows()){
        QString fw01=base1.arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),app);
        QString fw02=base1.arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation),app);
        //windir enviornment variable on windows points to X:\Windows,the first char is a disk symbol
        QString windir(QProcessEnvironment::systemEnvironment().value("windir"));
        QString diskSymbol=windir.isEmpty()?"C":windir.first(1);
        QString fw03=QString("%1:/Program Files/ffmpeg/%2").arg(diskSymbol,app);
        QString fw04=QString("%1:/Program Files (x86)/ffmpeg/%2").arg(diskSymbol,app);

        //app exe's folder
        QString executable=QCoreApplication::applicationDirPath();
        QString fw05=base0.arg(executable,app);
        QString fw06=base1.arg(executable,app);
        QString fw07=base3.arg(executable,app);
        locations<<fw01<<fw02<<fw03<<fw04<<fw05<<fw06<<fw07;
    }else{

        QString f1=QString("/bin/%1").arg(app);
        QString f2=QString("/sbin/%1").arg(app);
        QString f3=QString("/usr/bin/%1").arg(app);
        QString f4=QString("/usr/sbin/%1").arg(app);
        QString f5=QString("/usr/local/bin/%1").arg(app);
        QString f6=QString("/usr/libexec/ffmpeg/%1").arg(app);
        QString f7=QString("/usr/libexec/.ffmpeg/%1").arg(app);
        QString f8=QString("/usr/libexec/%1").arg(app);
        QString opt("/opt");
        QString f9=base1.arg(opt,app);
        QString f10=base2.arg(opt,app);

        QString home1=base0.arg(QDir::homePath(),app);
        QString home2=base1.arg( QDir::homePath(),app);
        QString home3=base2.arg( QDir::homePath(),app);

        locations<<f1<<f2<<f3<<f4<<f5<<f6<<f7<<f8<<f9<<f10<<home1<<home2<<home3;

        if(isMac()){
            // /Applications folder
            QString apps=QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
            // /Library folder
            QString systemLibrary("/Library");
            // ~/Library folder
            QString userLibrary=QString("%1/Library").arg(QDir::homePath());

            QString application1=base0.arg(apps,app);
            QString application2=base1.arg(apps,app);
            QString application3=base2.arg(apps,app);

            QString library1=base0.arg(systemLibrary,app);
            QString library2=base1.arg(systemLibrary,app);
            QString library3=base2.arg(systemLibrary,app);

            QString library4=base0.arg(userLibrary,app);
            QString library5=base1.arg(userLibrary,app);
            QString library6=base2.arg(userLibrary,app);

            locations<<application1<<application2<<application3<<library1<<library2<<library3<<library4<<library5<<library6;
        }
    }

    foreach(const QString& file,locations){
        if(!file.isEmpty()&&QFile::exists(file)){
            return file;
        }
    }

    return QString();
}
QString getAppFilePath(const AppKind appKind){
    QString appName="";
    QString extension=isWindows()?".exe":"";
    switch(appKind){
    case FFMPEG:
        appName=QString("ffmpeg%1").arg(extension);
        break;
    case FFPROBE:
        appName=QString("ffprobe%1").arg(extension);
        break;
    case FFPLAY:
        appName=QString("ffplay%1").arg(extension);
        break;
    default:
        break;
    }
    return __guessAppFile__(appName);
}

bool hasApp(const AppKind appKind){
    QString app=getAppFilePath(appKind);
    return QFile::exists(app);
}

QString padding(qint64 num, const char padding, qsizetype limit)
{
    return QString("%1").arg(num,limit,10,QLatin1Char(padding));
}


void openWithSystemFileManager(const QString& file)
{
    QFileInfo fileInfo(file);
    if(isWindows()){
        QString exec=QProcessEnvironment::systemEnvironment().value("windir").append(QDir::separator()).append("explorer.exe");
        QStringList param;
        param += QLatin1String("/select,");
        param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
        QProcess::startDetached(exec,param);
    }else if(isMac()){
        QProcess::startDetached("/usr/bin/open", {"-R", fileInfo.canonicalFilePath()});
    }
}
