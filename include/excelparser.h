#pragma once

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
    int findMaxDataRow (QXlsx::Document *xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping) const;

    // Helpers for findHeaders splitting
    void resetColumnMapping (ColumnMapping &mapping) const;
    QMap<QString, int *> buildHeaderColumnMap (ColumnMapping &mapping) const;
    void tryAssignHeaderMatch (const QString &normalizedCellText, int col, const QMap<QString, int *> &columnMap) const;

    bool parseDataRow (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier, QString &currentAddress,
                       PriceTag &priceTag) const;

    // Helpers for parseExcelFile splitting
    bool quickZipSignatureCheck (const QString &filePath) const;
    bool preScanZipForWorkbookAndSheets (const QString &filePath) const;
    bool readAndValidateDimension (QXlsx::Document &xlsx, QXlsx::CellRange &outRange) const;
    void parseAllRows (QXlsx::Document &xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping,
                       QList<PriceTag> &priceTags) const;

    // Helpers for parseDataRow splitting
    bool readTrimmedStringFromCell (QXlsx::Document *xlsx, int row, int col, QString &out) const;
    bool readPositiveDoubleFromCell (QXlsx::Document *xlsx, int row, int col, double &out) const;

    bool parseBrand (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const;
    bool parsePrice (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const;
    void parseQuantity (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const;
    void updateSupplierAndAddress (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier,
                                   QString &currentAddress, PriceTag &priceTag) const;
    void parseAttributes (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const;


    bool validatePriceTag (const PriceTag &priceTag) const;

    QString normalizeText (const QString &text);
};
