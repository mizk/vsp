#ifndef VLIST_H
#define VLIST_H

#include <QObject>
#include <QList>


class VList : public QObject
{
    Q_OBJECT
public:
    explicit VList(QObject *parent = nullptr);
    void addAll(const QStringList& list);
    qsizetype size() const;
    void empty();
    void remove(const QString& item);
    bool isEmpty() const;
    qsizetype indexOf(const QString& item);
    QStringList data() const;
    void load();
    void save();
private:
    void addOne(const QString& item);
private:
    QList<QString> items;
signals:

};

#endif // VLIST_H
