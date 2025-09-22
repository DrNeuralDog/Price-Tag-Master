#pragma once

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QString>

#include "xlsxabstractsheet.h"
#include "xlsxcell.h"
#include "xlsxcellrange.h"
#include "xlsxdocument.h"

#include "pricetag.h"


class ExcelParser: public QObject
{
    Q_OBJECT


public:
    explicit ExcelParser (QObject *parent = nullptr);
    ~ExcelParser ();

    bool parseExcelFile (const QString &filePath, QList<PriceTag> &priceTags);


private:
    struct ColumnMapping
    {
        int nameColumn				 = -1;
        int priceColumn				 = -1;
        int quantityColumn			 = -1;
        int supplierColumn			 = -1;
        int supplierAddressColumn	 = -1;
        int categoryColumn			 = -1;
        int additionalDataColumn	 = -1;
        int genderColumn			 = -1;
        int brandCountryColumn		 = -1;
        int manufacturingPlaceColumn = -1;
        int materialColumn			 = -1;
        int sizeColumn				 = -1;
        int articleColumn			 = -1;
        int price2Column			 = -1;
    };


    bool findHeaders (QXlsx::Document *xlsx, const QXlsx::CellRange &range, ColumnMapping &mapping);
    int findMaxDataRow (QXlsx::Document *xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping);

    bool parseDataRow (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier, QString &currentAddress,
                       PriceTag &priceTag);

    bool validatePriceTag (const PriceTag &priceTag);

    QString normalizeText (const QString &text);
};
