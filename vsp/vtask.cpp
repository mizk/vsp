#include "vtask.h"

VTask::VTask(QObject *parent)
    : QObject{parent}
{

}

qint64 VTask::position() const
{
    return fPosition;
}

qint64 VTask::offset() const
{
    return fOffset;
}

QString VTask::file() const
{
    return strFile;
}

qint64 VTask::index() const
{
    return iIndex;
}

qint64 VTask::volume() const
{
    return iVolume;
}

void VTask::setPosition( qint64 position)
{
    fPosition=position;
}

void VTask::setOffset( qint64 offset)
{
    fOffset=offset;
}

void VTask::setFile(const QString &file)
{
    strFile=file;
}

void VTask::setIndex(qint64 index)
{
    iIndex=index;
}

void VTask::setVolume(qint64 volume)
{
    iVolume=volume;
}

bool VTask::isLast() const
{
    return iIndex>=iVolume;
}
