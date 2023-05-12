#ifndef PLUGIN01_H
#define PLUGIN01_H

#include "Plugin01_global.h"
#include <QObject>
#include "PluginInterface.h"
#include <QDebug>

class PLUGIN01_EXPORT Plugin01 : public QObject,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "my_plugin01")

public:
    Plugin01();
    QString get_name() const
    {
        return "Plugin01";
    }
    QString show_text() const
    {
        return "this is Plugin01";
    }
    virtual void recMsgfromManager(PluginMetaData metaData) Q_DECL_OVERRIDE
    {
        qDebug()<<"Plugin01 receive message:"<< metaData.msg;
    }

signals:
    void sendMsg2Manager(PluginMetaData) Q_DECL_OVERRIDE;
};

#endif // PLUGIN01_H
