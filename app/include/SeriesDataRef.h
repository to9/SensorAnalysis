#ifndef SERIESDATAREF_H
#define SERIESDATAREF_H
#include <vector>
#include <unordered_map>
#include <memory>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>

//struct ColumnDataRef
//{
//    std::string header;
//    std::vector<double> value;
//};

//struct SeriesDataRef
//{
//    std::vector<std::string> headerList;
//    std::unordered_map<std::string, std::vector<double>> valueList;
//};

struct ColumnData
{
    QString text;
    QList <double> value;
};

struct BlockData
{
    void addData(const QString& header, double value)
    {
        for(auto &columnPtr : m_columnList)
        {
            if(columnPtr->text == header)
            {
                columnPtr->value.append(value);
                return;
            }
         }

        QSharedPointer<ColumnData> columnDataPtr = QSharedPointer<ColumnData>(new ColumnData);
        columnDataPtr->text = header;
        columnDataPtr->value.append(value);
        m_columnList.append(columnDataPtr);
    }

    bool getColumnData(const QString& text, QSharedPointer<ColumnData> &columnDataPtr)
    {
        for(auto &columnPtr : m_columnList)
        {
            if(columnPtr->text == text)
            {
                columnDataPtr = columnPtr;
                return true;
            }
        }
        return false;
    }

    QStringList firstRowData()
    {
        QStringList dataList;
        for(auto &columnPtr : m_columnList)
        {
            dataList.append(columnPtr->text);
        }
        return dataList;
    }

    void clearData()
    {
        m_columnList.clear();
    }

private:
    QList<QSharedPointer<ColumnData>> m_columnList;
};

struct SeriesDataRef
{
    void removeData(const QString& file)
    {
        blockList.remove(file);
    }

    void addData(const QString& file, QSharedPointer<BlockData> blockDataRef)
    {
        if(blockDataRef != nullptr)
            blockList[file] = blockDataRef;
    }

    bool getBlockData(const QString& file, QSharedPointer<BlockData> &blockDataPtr)
    {
        if(!blockList.contains(file))
            return false;

        blockDataPtr = blockList[file];
        return true;
    }

    bool contains(const QString& file)
    {
        return blockList.contains(file);
    }

private:
    QMap<QString, QSharedPointer<BlockData>> blockList;
};


#endif // SERIESDATAREF_H
