#include "filterviewwidget.h"
#include "sortfilterproxymodel.h"
#include "filterwidget.h"
#include "filtertreeview.h"
#include "SeriesDataRef.h"

#include <QVBoxLayout>
#include <QDockWidget>
#include <QStandardItemModel>
#include<QScrollBar>

FilterViewWidget::FilterViewWidget(QWidget *parent)
: QWidget(parent)
{
    proxyModel = new SortFilterProxyModel(this);

    filterWidget = new FilterWidget(this);
    connect(filterWidget, &FilterWidget::filterChanged, this, &FilterViewWidget::textFilterChanged);

    proxyView = new FilterTreeView(this);
    proxyView->setRootIsDecorated(false);
    //proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    //proxyView->setSortingEnabled(true);
    //proxyView->sortByColumn(1, Qt::AscendingOrder);
    proxyView->setUniformRowHeights(true);
    proxyView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    proxyView->setSelectionBehavior(QTreeView::SelectRows);
    proxyView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QVBoxLayout *filterLayout = new QVBoxLayout(this);
    filterLayout->addWidget(filterWidget);
    filterLayout->addWidget(proxyView);

    connect(proxyView, &FilterTreeView::doubleClicked, [=](const QModelIndex &index){
                        Q_EMIT treeViewClickedChange(index.row());
                    });

    QScrollBar *bar = proxyView->verticalScrollBar();
    connect(bar, &QAbstractSlider::sliderMoved, [=](int position){
                        Q_EMIT treeViewClickedChange(position);
                    });

    connect(proxyView, &FilterTreeView::treeViewSlect, this, &FilterViewWidget::treeViewSelectData);

    connect(proxyView, &FilterTreeView::entered, [=](const QModelIndex &index){
                        Q_EMIT treeViewClickedChange(index.row());
                    });

    m_model = new QStandardItemModel(this);
    proxyModel->setSourceModel(m_model);
}

void FilterViewWidget::treeViewSelectData(const QModelIndexList &moduleIndexList)
{
    //qDebug()<<"FilterViewWidget::" <<moduleIndexList.size()<<" "<<m_model->rowCount()<<" "<<m_model->columnCount();

    QList<QStringList> selectData;
    QStringList str;
    int columnCoumnt = m_model->columnCount();
    for(int i= 0; i<moduleIndexList.size(); i++)
    {
        QModelIndex index = moduleIndexList.at(i);
        QStandardItem *item = m_model->item(index.row(), index.column());
        if((i + 1) % columnCoumnt == 0)
        {
            selectData.append(str);
            str.clear();
        }
        else
        {
            str <<item->text();
        }
    }

    Q_EMIT treeViewSlectChange(selectData);
}

void FilterViewWidget::textFilterChanged()
{
    FilterWidget::PatternSyntax s = filterWidget->patternSyntax();
    QString pattern = filterWidget->text();
    switch (s) {
    case FilterWidget::Wildcard:
        pattern = QRegularExpression::wildcardToRegularExpression(pattern);
        break;
    case FilterWidget::FixedString:
        pattern = QRegularExpression::escape(pattern);
        break;
    default:
        break;
    }

    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    if (filterWidget->caseSensitivity() == Qt::CaseInsensitive)
        options |= QRegularExpression::CaseInsensitiveOption;
    QRegularExpression regularExpression(pattern, options);
    proxyModel->setFilterRegularExpression(regularExpression);
}

void FilterViewWidget::addData(const QSharedPointer<SeriesDataRef>& dataRef)
{
    setUpdatesEnabled(false);

    //######
//    //set data
//    for(auto &name : dataRef->headerList)
//    {
//        QList<QStandardItem*> items;
//        for (auto val : dataRef->valueList[name])
//        {
//            QStandardItem *item = new QStandardItem(QString::number(val));
//            items.append(item);
//        }
//        m_model->appendColumn(items);
//    }

//    //set header
//    for(int i = 0; i< dataRef->headerList.size(); i++)
//    {
//        std::string name = dataRef->headerList.at(i);
//        m_model->setHeaderData(i, Qt::Horizontal, QString::fromStdString(name));
//    }
    setUpdatesEnabled(true);
}
