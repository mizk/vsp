#ifndef GLOBALDEF_H
#define GLOBALDEF_H
#include <QString>
#define MAX_NAME_LENGTH 5
enum AppKind{
    FFPROBE,
    FFMPEG,
    FFPLAY
};
enum SettingType{
    FileList
};

bool isWindows();
bool isMac();
bool isUnix();
bool isLinux();
bool hasApp(const AppKind appKind);
QString padding(qint64 num, const char padding, qsizetype limit);
QString getAppFilePath(const AppKind appKind);
void openWithSystemFileManager(const QString& file);
#endif // GLOBALDEF_H
