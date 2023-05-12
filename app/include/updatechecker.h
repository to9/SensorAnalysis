#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QLabel>

class UpdateChecker : public QDialog
{
    Q_OBJECT
public:
    UpdateChecker(QString name, QString version, QWidget *parent = Q_NULLPTR);
    bool checkUpdate();

Q_SIGNALS:
    void appUpdate(QString, QString, QString);    

public Q_SLOTS:
    void replyFinished(QNetworkReply *reply);

private:
    int compareVersion(const QString &version1, const QString &version2);
    void startUpdate(const QString &name, QStringList arglist);
    void startAppUpdate(QString version, QString note, QString url);

private:
    QNetworkAccessManager *m_networkManagerDownload;
    QString m_AppName;
    QString m_AppVersion;
    QString m_updateVersion;
    QString m_udpateName;
    QString m_updateNote;
    QString m_updateUrl;
    QLabel *m_updateNoteLabel;
};

#endif // UPDATECHECKER_H
