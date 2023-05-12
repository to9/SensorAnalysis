#ifndef DATALOADERCSV_H
#define DATALOADERCSV_H

#include <QObject>
#include "dataloader.h"

class DataLoaderCSV : public DataLoader
{
    Q_OBJECT
public:
    DataLoaderCSV();
  virtual ~DataLoaderCSV();

  virtual bool loadDataFromFile(const QString& fileName, QSharedPointer<BlockData> blockDataPtr);

private:
    uint32_t getFileTotalLine(const QString& fileName);

};

#endif // DATALOADERCSV_H
