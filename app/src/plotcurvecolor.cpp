#include "plotcurvecolor.h"
#include <QRandomGenerator>
#include <QDebug>

//https://www.w3.org/TR/SVG11/types.html#ColorKeywords
const QVector<QColor> PlotCurveColor::colorMap =
{
    {QColor("black")}, {QColor("red")}, {QColor("green")}, {QColor("blue")}, {QColor("cyan")},
    {QColor("magenta")}, {QColor("yellow")},{QColor("brown")},{QColor("orchid")}, {QColor("fuchsia")},
    {QColor("darkgreen")}, {QColor("darkblue")}, {QColor("orangered")}, {QColor("springgreen")}, {QColor("lawngreen")},
    {QColor("violet")},{QColor("indigo")},{QColor("olivedrab")},{QColor("mediumvioletred")},{QColor("blueviolet")}
};

PlotCurveColor::PlotCurveColor()
{
}

PlotCurveColor::~PlotCurveColor()
{
}

QVector<QColor> PlotCurveColor::getRandomColor(int count)
 {
    QVector<QColor> colorList;

    if(count < colorMap.size())
    {
        QVector<int> indexList;
        while(indexList.size() < count)
        {
            //QColor color = QColor::fromHsl(rand()%360, rand()%256, rand()%200);
            int index = QRandomGenerator::global()->bounded(colorMap.size()-1);
            if(indexList.indexOf(index) < 0)
            {
                indexList.push_back(index);
            }
        };

//        qDebug()<<"count: "<< count<<"list: "<<indexList;

        for(auto &index : indexList)
        {
            QColor color = colorMap.at(index);
            colorList.push_back(color);
        }
    }
//    qDebug()<<"colorList: "<< colorList;
    return std::move(colorList);
 }
