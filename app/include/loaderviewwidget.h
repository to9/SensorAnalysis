#ifndef LOADER_VIEW_WIDGET_H
#define LOADER_VIEW_WIDGET_H

#include <QTreeWidget>

#include "dataloader.h"

class ListView;
class LoaderViewWidget: public QTreeWidget
{
    Q_OBJECT
public:
    LoaderViewWidget(QWidget *parent = nullptr);
    virtual ~LoaderViewWidget() = default;

    void setData(const QString &file, const QStringList &headerList);

signals:
    void remove(const QString &file);

private:
    QMap<QString, QStringList> getSelectedNames();
    void expandChildren(bool expanded, QTreeWidgetItem* item);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    enum itemType{root = QTreeWidgetItem::UserType, child};

    bool m_dragging;
    QPoint m_dragPosition;
    QTreeWidgetItem *m_currentSelectItem;
};

#endif // LOADER_VIEW_WIDGET_H
