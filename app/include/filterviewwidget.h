#ifndef FILTERVIEWWIDGET_H
#define FILTERVIEWWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QDockWidget>
#include <QStandardItemModel>

class SeriesDataRef;
class FilterTreeView;
class FilterListModel;
class SortFilterProxyModel;
class FilterWidget;

class FilterViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterViewWidget(QWidget *parent = nullptr);

    QDockWidget *create();
    void addData(const QSharedPointer<SeriesDataRef> &dataRef);

Q_SIGNALS:
    void treeViewClickedChange(const uint64_t &postion);
    void treeViewSlectChange(const QList<QStringList> &selectDataList);

public Q_SLOTS:
    void treeViewSelectData(const QModelIndexList &moduleIndexList);
    void textFilterChanged();

private:
    FilterWidget *filterWidget;
    SortFilterProxyModel *proxyModel;
    FilterTreeView *proxyView;
    QStandardItemModel *m_model;
};

#endif // FILTERVIEWWIDGET_H
