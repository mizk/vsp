#ifndef VTASK_H
#define VTASK_H

#include <QObject>
#include <QString>

class VTask : public QObject
{
    Q_OBJECT
public:
    explicit VTask(QObject *parent = nullptr);
    qint64 position() const;
    qint64 offset() const;
    QString file() const;
    qint64 index() const;
    qint64 volume() const;
    void setPosition( qint64 position);
    void setOffset( qint64 offset);
    void setFile( const QString& file);
    void setIndex(qint64 order);
    void setVolume(qint64 volume);
    bool isLast() const;
signals:
private:
    qint64 iVolume;
    qint64 iIndex;
    qint64 fOffset;
    qint64 fPosition;
    QString strFile;
};

#endif // VTASK_H
