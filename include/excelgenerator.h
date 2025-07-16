#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include "pricetag.h"


class ExcelGenerator: public QObject
{
    Q_OBJECT //


public:
    explicit ExcelGenerator (QObject *parent = nullptr);
    ~ExcelGenerator ();

    bool generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath);
}; 
