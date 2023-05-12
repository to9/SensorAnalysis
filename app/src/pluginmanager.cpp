#include "pluginmanager.h"
#include <QPluginLoader>
#include <QDir>
#include <QDebug>

PluginManager* PluginManager::m_instance;
PluginManager::PluginManager(QObject *parent) : QObject(parent)
{

}

PluginManager::~PluginManager()
{
    unloadAllPlugins();
}


void getPlugisFile(const QString& dir_name, QStringList& name_list)
{
    QDir dir(dir_name);
    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);
    for(const QFileInfo &info : list)
    {
        if(info.isDir())
        {
            getPlugisFile(info.absoluteFilePath(), name_list);
        }
        else if(info.isFile() && (QString::compare("dll", info.suffix()) == 0 ||
                                  QString::compare("so", info.suffix()) == 0))
        {
            name_list.append(info.absoluteFilePath());
        }
    }
}

//加载所有插件
void PluginManager::loadAllPlugins()
{
    QDir dir(QDir::currentPath());
    dir.cd("debug");
    dir.cd("plugins");

    QStringList plugins;
    getPlugisFile(dir.path(), plugins);

    for(QString &file : plugins)
    {
        qDebug()<<file;
        loadPlugin(file);
    }
}

//加载其中某个插件
void PluginManager::loadPlugin(const QString &filepath)
{
    if(!QLibrary::isLibrary(filepath))
        return;

    //加载插件
    QPluginLoader *loader = new QPluginLoader(filepath);
    QString plugin_name;
    if(loader->load())
    {
        PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if(plugin)
        {
            plugin_name = plugin->get_name();
            m_loaders.insert(filepath, loader);
            m_names.insert(filepath,plugin_name);
            qDebug()<<"plugins name:"<<plugin->get_name()<<"plugins info:"<<plugin->show_text();

            connect(loader->instance(),SIGNAL(sendMsg2Manager(PluginMetaData)),this,SLOT(recMsgfromPlugin(PluginMetaData)));
        }
        else
        {
            delete loader;
            loader = nullptr;
        }
    }
    else
    {
        qDebug()<<"loadPlugin:"<<filepath<<loader->errorString();
    }
}

//卸载所有插件
void PluginManager::unloadAllPlugins()
{
    for(QString filepath : m_loaders.keys())
        unloadPlugin(filepath);
}

void PluginManager::unloadPlugin(const QString &filepath)
{
    QPluginLoader *loader = m_loaders.value(filepath);
    //卸载插件，并从内部数据结构中移除
    if(loader->unload())
    {
        m_loaders.remove(filepath);
        delete loader;
        loader = nullptr;
    }
}

//获取某个插件名称
QVariant PluginManager::getPluginName(QPluginLoader *loader)
{
    if(loader)
        return m_names.value(m_loaders.key(loader));
    else
        return "";
}

//根据名称获得插件
QPluginLoader *PluginManager::getPlugin(const QString &name)
{
    return m_loaders.value(m_names.key(name));
}

void PluginManager::recMsgfromPlugin(PluginMetaData metadata)
{
    auto loader = getPlugin(metadata.dest);//目标插件
    if(loader)
    {
        auto interface = qobject_cast<PluginInterface*>(loader->instance());;
        if(interface)
        {
            interface->recMsgfromManager(metadata);//转发给对应插件
        }
    }
}
