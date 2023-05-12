#ifndef DATALOADER_H
#define DATALOADER_H
#include <QString>
#include <vector>
#include <memory>
#include <QObject>

#include "SeriesDataRef.h"
class DataLoader: public QObject
{
public:
    DataLoader() = default;
    virtual ~DataLoader() = default;

    virtual bool loadDataFromFile(const QString& fileName, QSharedPointer<BlockData> blockDataPtr) = 0;
};

using DataLoaderPtr = std::shared_ptr<DataLoader>;

#endif // DATALOADER_H
