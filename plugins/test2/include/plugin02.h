#ifndef PLUGIN02_H
#define PLUGIN02_H

#include "Plugin02_global.h"
#include <QObject>
#include "PluginInterface.h"
#include <QDebug>

class PLUGIN02_EXPORT Plugin02 : public QObject,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "my_plugin02")

public:
    Plugin02();
    QString get_name() const
    {
        return "Plugin02";
    }
    QString show_text() const
    {
        return "this is Plugin02";
    }
    virtual void recMsgfromManager(PluginMetaData metaData) Q_DECL_OVERRIDE
    {
        qDebug()<<"Plugin02 receive message:"<< metaData.msg;
    }

signals:
    void sendMsg2Manager(PluginMetaData) Q_DECL_OVERRIDE;
};


#endif // PLUGIN02_H
