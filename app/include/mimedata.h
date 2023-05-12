#ifndef MIMEDATA_H
#define MIMEDATA_H

#include <QMimeData>
#include <QMap>
#include <QString>

class MimeData : public QMimeData
{
    Q_OBJECT
public:
    MimeData();
    virtual ~MimeData();

    void setSelect(const QMap<QString, QStringList> &data);
    QStringList getSelectType() const;
    void getSelect(const QString &type, QStringList &dataList) const;
    void getSelectList(QMap<QString, QStringList> &select) const;

private:
    QMap<QString, QStringList> m_treeSelect;
};

#endif // MIMEDATA_H
