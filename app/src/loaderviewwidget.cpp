#include "loaderviewwidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>
#include <QGuiApplication>
#include <QApplication>
#include <QMessageBox>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QFontDatabase>
#include "mimedata.h"
#include <QMenu>
#include <QAction>
#include <QTreeWidget>

LoaderViewWidget::LoaderViewWidget(QWidget *parent)
    : QTreeWidget(parent)
    , m_dragging(false)
    , m_currentSelectItem(nullptr)
{
    setColumnCount(1);
    setEditTriggers(NoEditTriggers);
    setDragEnabled(false);
    setDefaultDropAction(Qt::IgnoreAction);
    setDragDropOverwriteMode(false);
    setMouseTracking(true);
    setDragDropMode(NoDragDrop);
    viewport()->installEventFilter(this);
    setSelectionMode(ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setFocusPolicy(Qt::ClickFocus);

    header()->setVisible(false);
    header()->setStretchLastSection(true);
    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int column) {
        if (column == 0)
        {
            expandChildren(!item->isExpanded(), item);
        }
    });

    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]() {
        if (getSelectedNames().empty())
        {
            // this looks nicer
            clearFocus();
            setFocusPolicy(Qt::NoFocus);
        }
        else
        {
            // this focus policy is needed to allow CurveListPanel::keyPressEvent to be called
            setFocusPolicy(Qt::ClickFocus);
        }
    });

    connect(this, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        m_currentSelectItem = this->itemAt(pos);
        if(m_currentSelectItem == nullptr)
            return;

        if(m_currentSelectItem->type() == root)
        {
            QAction* removeAction = new QAction("remove");
            connect(removeAction, &QAction::triggered, this, [this](bool point) {
                QString file = m_currentSelectItem->text(0);
                takeTopLevelItem(indexOfTopLevelItem(m_currentSelectItem));
                emit remove(file);
            });
            QMenu *popMenu =new QMenu(this);
            popMenu->addAction(removeAction);
            popMenu->exec(QCursor::pos());
        }
    });
}

void LoaderViewWidget::setData(const QString &file, const QStringList &headerList)
{
    QTreeWidgetItem *topItem1 = new QTreeWidgetItem(this, QStringList("tree"), root);
    topItem1->setText(0, file);
    addTopLevelItem(topItem1);

    for (const QString &name : headerList)
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(QStringList("tree"), child);
        //            QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        //            font.setPointSize(9);
        //            QFont font("Times", 10, QFont::Bold);
        //            child_item->setFont(0, font);
        child_item->setText(0, name);
        child_item->setData(0, Qt::UserRole, name);
        child_item->setFlags(child_item->flags() | Qt::ItemIsSelectable);
        child_item->setTextAlignment(1, Qt::AlignRight);
        topItem1->addChild(child_item);
    }
}

QMap<QString, QStringList> LoaderViewWidget::getSelectedNames()
{
    QMap<QString, QStringList> non_hidden_list;
    for (const auto& item : selectedItems())
    {
        QTreeWidgetItem *parent = item->parent();
        if(parent != nullptr)
            non_hidden_list[parent->text(0)].append(item->data(0, Qt::UserRole).toString());
    }
    return non_hidden_list;
}

void LoaderViewWidget::expandChildren(bool expanded, QTreeWidgetItem* item)
{
    int childCount = item->childCount();
    for (int i = 0; i < childCount; i++)
    {
        const auto child = item->child(i);
        if( child->childCount() > 0 )
        {
            child->setExpanded(expanded);
            expandChildren(expanded, child);
        }
    }
}

void LoaderViewWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        m_dragPosition = event->pos();
    }

    QTreeWidget::mousePressEvent(event);
}

void LoaderViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*if (event->type() == QEvent::MouseMove)
    {
      auto mouse_event = static_cast<QMouseEvent*>(event);
      QTreeWidgetItem* item = itemAt(mouse_event->pos());
      if (item)
      {
        auto tooltip = item->data(0, Qt::UserRole);
        qDebug()<<"tip: "<<tooltip.toString();
        if (tooltip.isValid())
        {
          QToolTip::showText(mapToGlobal(mouse_event->pos()), tooltip.toString());
        }
        else
        {
          QToolTip::hideText();
        }
      }
    }*/

    if ((event->buttons() & Qt::LeftButton) &&
            ((event->pos() - m_dragPosition).manhattanLength() >= QApplication::startDragDistance()) &&
            !m_dragging &&
            !getSelectedNames().empty())
    {
        m_dragging = true;
        QDrag *drag = new QDrag(this);
        MimeData *mimeData = new MimeData;
        mimeData->setSelect(getSelectedNames());
        QByteArray mdata;
        mimeData->setData("curveslist/add_curve", mdata);

        qDebug()<<"LoaderViewWidget::mousePressEvent";
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction | Qt::MoveAction);
        clearSelection();
    }
}
