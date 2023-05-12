#include "updatechecker.h"
#include <QNetworkRequest>
#include <QXmlStreamReader>
#include <QStringView>
#include <QFile>
#include <QCoreApplication>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QDialogButtonBox>

UpdateChecker::UpdateChecker(QString name, QString version, QWidget *parent)
: QDialog(parent)
, m_networkManagerDownload(NULL)
, m_AppName(name)
, m_AppVersion(version)
, m_updateVersion()
, m_udpateName()
, m_updateNote()
, m_updateUrl()
{
    this->setWindowFlags(Qt::Dialog);
    this->setFixedSize(250, 150);
    this->setWindowTitle("Version");
    m_updateNoteLabel = new QLabel(tr(""));
    m_updateNoteLabel->setWordWrap(true);
    QPushButton *updateButton = new QPushButton(tr("&Update"));
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(updateButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    connect(updateButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_updateNoteLabel);
    mainLayout->addItem(verticalSpacer);
    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);

    m_networkManagerDownload = new QNetworkAccessManager(this);
    connect(m_networkManagerDownload, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QNetworkRequest quest;
    quest.setUrl(QUrl("http://www.lnsign.com/images/upload/tools/SensorAnalysis/updatelist.xml"));
    quest.setHeader(QNetworkRequest::UserAgentHeader, "RT-Thread ART");
    m_networkManagerDownload->get(quest);
}

void UpdateChecker::replyFinished(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode != 200)
    {
        return;
    }

    QXmlStreamReader reader(reply->readAll());
    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name().toString().trimmed() == "update")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if (attributes.hasAttribute("version"))
                {
                    QString version = attributes.value("version").toString().trimmed();
                    if(version == "1.0")
                    {
                        while(reader.readNextStartElement())
                        {
                            if(reader.name().toString() == "bin")
                            {
                                m_udpateName = reader.readElementText().trimmed();
                                //qDebug()<<"m_udpateName: "<<m_udpateName;
                            }
                            else if(reader.name().toString() == "url")
                            {
                                m_updateUrl = reader.readElementText().trimmed();
                                //qDebug()<<"m_updateUrl: "<<m_updateUrl;
                            }
#ifdef Q_OS_LINUX
                            else if(reader.name().toString() == "linux")
                            {
                                while(reader.readNextStartElement())
                                {
                                    if(reader.name().toString() == "version")
                                    {
                                        m_updateVersion = reader.readElementText().trimmed();
                                        //qDebug()<<"linux"<<"m_updateVersion:"<<m_updateVersion;
                                    }
                                    else if(reader.name().toString() == "note")
                                    {
                                        m_updateNote = reader.readElementText().trimmed();
                                        //qDebug()<<"linux"<<"m_updateNote:"<<m_updateNote;
                                    }
                                };
                            }
#endif
#ifdef Q_OS_WIN32
                            else if(reader.name().toString() == "win")
                            {
                                while(reader.readNextStartElement())
                                {
                                    if(reader.name().toString() == "version")
                                    {
                                        m_updateVersion = reader.readElementText().trimmed();
                                        //qDebug()<<"win"<<"m_updateVersion:"<<m_updateVersion;
                                    }
                                    else if(reader.name().toString() == "note")
                                    {
                                        m_updateNote = reader.readElementText().trimmed();
                                        //qDebug()<<"win"<<"m_updateNote:"<<m_updateNote;
                                    }
                                };
                            }
#endif
                            else
                                reader.skipCurrentElement();
                        };
                    }
                    else
                    {
                        reader.raiseError("The file is version error.");
                    }
                }
            }
        }
        reader.readNext();
    };

    if (reader.hasError())
    {
        qDebug()<<"xml error: "<<reader.errorString();
    }

    reply->deleteLater();

    checkUpdate();
}

bool UpdateChecker::checkUpdate()
{
    if(m_udpateName.isEmpty() || m_updateVersion.isEmpty() || m_updateUrl.isEmpty())
        return false;

    if((compareVersion(m_updateVersion, m_AppVersion) > 0) && QString::compare(m_udpateName, m_AppName) == 0)
    {
        startAppUpdate(m_updateVersion, m_updateNote, m_updateUrl);
        return true;
    }

    qDebug()<<"UpdateChecker: "<<m_udpateName<<" "<<m_updateVersion<<", "<<m_AppName<<" "<<m_AppVersion;
    return false;
}

//-2: 版本号格式错误, "-1": ver1 < ver2, "0": ver1 == ver2, "1": ver1 > ver2
int UpdateChecker::compareVersion(const QString &version1, const QString &version2)
{
    if(!version1.compare(version2))
        return 0;

    QStringList list1 = version1.split('.');
    QStringList list2 = version2.split('.');

    int count1 = list1.count();
    int count2 = list2.count();
    int nCount = count1 > count2 ? count2 : count1;

    for(int i = 0; i < nCount; ++i)
    {
        bool ok1 = false, ok2 = false;
        int num1 = list1[i].toInt(&ok1);
        int num2 = list2[i].toInt(&ok2);

        if(!ok1 || !ok2)
            return -2;

        if(num1 == num2)
            continue;

        if(num1 > num2)
            return 1;
        else
            return -1;
    }

    return 0;
}

void UpdateChecker::startAppUpdate(QString version, QString note, QString url)
{
    QString tip = QString("New Version: %1\n %2").arg(version, note);
    m_updateNoteLabel->setText(tip);

    int n = this->exec();
    if(n == QDialog::Accepted)
    {
        QString updateAppName;
#ifdef Q_OS_LINUX
        updateAppName.append("update");
#endif
#ifdef Q_OS_WIN32
        updateAppName.append("update.exe");
#endif
        QString updateAppPath = QString("%1/%2").arg(QCoreApplication::applicationDirPath(), updateAppName);
        QStringList argList;
        argList <<url << QCoreApplication::applicationName();

        //qDebug()<<"updateAppPath"<<updateAppPath<<"argList:"<<argList;
        startUpdate(updateAppPath, argList);
    }
    else if(n == QDialog::Rejected)
    {
        //qDebug()<<"Rejected";
    }
}

void UpdateChecker::startUpdate(const QString &name, QStringList arglist)
{
    if (!name.isEmpty())
    {
        if (!QProcess::startDetached(name, arglist))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("warrning"), QString::fromLocal8Bit("Starting program error!\nprogram may not exist or be damaged!\nsolution: reinstall the program!"));
        }
    }
    this->close();
}
