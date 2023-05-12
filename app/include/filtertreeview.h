#ifndef FILTERTREEVIEW_H
#define FILTERTREEVIEW_H

#include <QTreeView>

class FilterTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit FilterTreeView(QWidget *parent = nullptr);
    ~FilterTreeView();

Q_SIGNALS:
    void treeViewSlect(const QModelIndexList &moduleIndexList);

public Q_SLOTS:
    void copy();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QMenu *m_menu;
};

#endif // FILTERTREEVIEW_H
