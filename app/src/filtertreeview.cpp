#include "filtertreeview.h"
#include <QMenu>
#include <QAction>
#include <QKeyEvent>

FilterTreeView::FilterTreeView(QWidget *parent)
    : QTreeView(parent)
{
    QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

    m_menu = new QMenu(this);
    m_menu->setStyleSheet(qss);
    QAction *copyAct = m_menu -> addAction(tr( "copy" ));
    connect(copyAct, &QAction::triggered, this, &FilterTreeView::copy);
}

FilterTreeView::~FilterTreeView()
{
}

void FilterTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu->exec(QCursor::pos());
}

void FilterTreeView::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy))
    {
        copy();
    }
    else if(event->matches(QKeySequence::SelectAll))
    {
        selectAll();
    }
}

void FilterTreeView::copy()
{
    QModelIndexList  moduleIndexList = selectedIndexes();
    Q_EMIT treeViewSlect(moduleIndexList);
}
