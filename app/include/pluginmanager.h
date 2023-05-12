#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QHash>
#include "pluginInterface.h"

class QPluginLoader;

class PluginManager : public QObject
{
    Q_OBJECT

public:
    static PluginManager *instance()
    {
        if(m_instance == nullptr)
            m_instance = new PluginManager();
        return m_instance;
    }

    void loadAllPlugins();
    void loadPlugin(const QString &filepath);
    void unloadPlugin(const QString &filepath);
    void unloadAllPlugins();
    QPluginLoader* getPlugin(const QString &name);
    QVariant getPluginName(QPluginLoader *loader);

public slots:
    void recMsgfromPlugin(PluginMetaData metadata);

private:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

    QHash<QString, QPluginLoader *> m_loaders; //插件路径--QPluginLoader实例
    QHash<QString, QString> m_names; //插件路径--插件名称

    static PluginManager *m_instance;
    class GarbageCollector
    {
        ~GarbageCollector()
        {
            if (PluginManager::instance())
            {
                delete PluginManager::instance();
                PluginManager::m_instance = nullptr;
            }
        }
    };
    static GarbageCollector gc;
};

#endif // PLUGINMANAGER_H
