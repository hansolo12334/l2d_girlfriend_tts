#ifndef CHARA_RESOURCE_LOADER
#define CHARA_RESOURCE_LOADER

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


class chara_resource_loader : public QObject
{
    Q_OBJECT

public:
    static chara_resource_loader &instance()
    {
        static chara_resource_loader ins;
        return ins;
    }


    void load_resource();

private:

};


#endif
