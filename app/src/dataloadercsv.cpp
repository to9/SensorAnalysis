#include "dataloadercsv.h"
#include <QFile>
#include <QTextStream>
#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>

DataLoaderCSV::DataLoaderCSV()
{
}

DataLoaderCSV::~DataLoaderCSV()
{
}

uint32_t DataLoaderCSV::getFileTotalLine(const QString& fileName)
{
    uint32_t totalLines = 0;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    while (!in.atEnd())
    {
        in.readLine();
        totalLines++;
    }
    file.close();

    return totalLines;
}

bool DataLoaderCSV::loadDataFromFile(const QString& fileName, QSharedPointer<BlockData> blockDataPtr)
{
    if(blockDataPtr == nullptr)
        return false;

    uint32_t totalLines = getFileTotalLine(fileName);

    QProgressDialog progress_dialog;
    progress_dialog.setLabelText("Loading... please wait");
    progress_dialog.setWindowModality(Qt::ApplicationModal);
    progress_dialog.setRange(0, totalLines - 1);
    progress_dialog.setAutoClose(true);
    progress_dialog.setAutoReset(true);
    progress_dialog.show();

    bool interrupted = false;
    int linecount = 0;
    QStringList headerList;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().simplified();
        if(line.endsWith(','))
            line = line.first(line.size()-1);

        if(linecount == 0)
        {
            //read first line (header)
            QStringList strList = line.split(",");
            foreach (const QString &str, strList)
            {
                QString name = str.trimmed();
                if(headerList.contains(name))

                {
                    QString err_msg = QString("Duplicate Headers %1\n"
                                              "Aborting...")
                            .arg(str);
                    QMessageBox::warning(nullptr, "Error reading file", err_msg);
                    goto ERROR;
                }

                headerList.append(name);
            }
        }
        else
        {
            //read other line (data)
            QStringList strList = line.split(",");
            int column = strList.size();
            if(column != headerList.size())
            {
                QString err_msg = QString("The number of columns at %1 row \n"
                                          "is greater than header columns %2\n"
                                          "Aborting...")
                        .arg(linecount + 1)
                        .arg(column);
                QMessageBox::warning(nullptr, "Error reading file", err_msg);
                goto ERROR;
            }

            for (int i = 0; i < column; ++i)
            {
                bool ok;
                QString value = strList.at(i);
                double val = value.trimmed().toDouble(&ok);
                if(ok)
                {
                    blockDataPtr->addData(headerList[i], val);
                }
                else
                {
                    QString err_msg = QString("Row %1 of Column %2 in the document,\n"
                                              "Invalid characters %3\n"
                                              "Aborting...")
                            .arg(linecount + 1)
                            .arg(i + 1)
                            .arg(value);
                    QMessageBox::warning(nullptr, "Error reading file", err_msg);
                    goto ERROR;
                }
            }
        }

        //process
        if (linecount++ % 100 == 0)
        {
            progress_dialog.setValue(linecount+1);
            QApplication::processEvents();
            if (progress_dialog.wasCanceled())
            {
                interrupted = true;
                goto ERROR;
            }
        }
    }
    return true;

ERROR:
    if(interrupted)
        progress_dialog.cancel();

    blockDataPtr->clearData();
    return false;
}
