#include "mimedata.h"


MimeData::MimeData()
    : QMimeData()
{
}

MimeData::~MimeData()
{
}

void MimeData::setSelect(const QMap<QString, QStringList> &data)
{
    m_treeSelect = data;
}

QStringList MimeData::getSelectType() const
{
    return m_treeSelect.keys();
}

void MimeData::getSelect(const QString &type, QStringList &dataList) const
{
    dataList = m_treeSelect[type];
}

void MimeData::getSelectList(QMap<QString, QStringList> &select) const
{
    select = m_treeSelect;
}
