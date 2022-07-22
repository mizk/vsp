#include "vlist.h"
#include "globaldef.h"
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>

VList::VList(QObject *parent)
    : QObject{parent}
{

}

void VList::addAll(const QStringList &list)
{
    foreach(const QString& item,list){
        addOne(item);
    }
}

qsizetype VList::size() const
{
    return items.size();
}

void VList::empty()
{
    items.empty();
}

void VList::remove(const QString &item)
{
    qsizetype position=items.indexOf(item);
    if(position!=-1){
        items.removeAt(position);
    }
}

bool VList::isEmpty() const
{
    return items.isEmpty();
}

qsizetype VList::indexOf(const QString &item)
{
    return items.indexOf(item);
}

QStringList VList::data() const
{
    QStringList list;
    foreach(const QString& item,items){
        list.append(item);
    }
    return list;
}

void VList::load()
{
    QFile reader(QDir::homePath()+"/.xcc/list");
    if(reader.open(QIODevice::ReadOnly)){
        QJsonDocument doc=QJsonDocument::fromJson(reader.readAll());
        reader.close();
        if(doc.isArray()){
            const QJsonArray& array=doc.array();
            items.clear();
            foreach(const QJsonValue& value,array){
                if(value.isString()){
                    QString filename(value.toString());
                    if(QFile::exists(filename)){
                        items.append(filename);
                    }
                }
            }
        }

    }
}

void VList::save()
{
    QJsonArray array=QJsonArray::fromStringList(items);
    QJsonDocument doc;
    doc.setArray(array);
    if(!QDir::home().exists(".xcc")){
        QDir::home().mkdir(".xcc");
    }
    QFile file(QDir::homePath()+"/.xcc/list");
    if(file.open(QIODevice::WriteOnly)){
        file.write(doc.toJson(QJsonDocument::Compact));
        file.flush();
        file.close();
    }
}

void VList::addOne(const QString &item)
{
    if(!items.contains(item)){
        items.append(item);
    }
}
