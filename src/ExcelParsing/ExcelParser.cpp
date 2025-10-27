#include "ExcelParser.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QRegularExpression>
#include <QString>

#include <xlsxzipreader_p.h>
#include "xlsxabstractsheet.h"
#include "xlsxcell.h"

#include "pricetag.h"


ExcelParser::ExcelParser (QObject *parent) : QObject (parent) {}

ExcelParser::~ExcelParser () {}


bool ExcelParser::parseExcelFile (const QString &filePath, QList<PriceTag> &priceTags)
{
    qDebug () << "Starting to parse Excel file:" << filePath;

    QFileInfo fileInfo (filePath);
    if (! fileInfo.exists ())
    {
        qDebug () << "File does not exist:" << filePath;

        return false;
    }

    if (! quickZipSignatureCheck (filePath))
        return false;

    if (! preScanZipForWorkbookAndSheets (filePath))
        return false;

    QXlsx::Document xlsx (filePath);

    QXlsx::CellRange range;
    if (! readAndValidateDimension (xlsx, range))
        return false;

    ColumnMapping columnMapping;
    if (! findHeaders (&xlsx, range, columnMapping))
    {
        qDebug () << "Failed to find required headers";

        return false;
    }

    parseAllRows (xlsx, range, columnMapping, priceTags);

    qDebug () << "Parsed" << priceTags.size () << "price tags";


    return ! priceTags.isEmpty ();
}

bool ExcelParser::quickZipSignatureCheck (const QString &filePath) const
{
    QFile f (filePath);
    if (! f.open (QIODevice::ReadOnly))
    {
        qDebug () << "Cannot open file for read:" << filePath;

        return false;
    }

    QByteArray sig = f.read (4);
    f.close ();
    if (sig.size () != 4 || ! (sig[0] == 'P' && sig[1] == 'K'))
    {
        qDebug () << "Not a valid ZIP/OOXML (PK) signature";

        return false;
    }


    return true;
}

bool ExcelParser::preScanZipForWorkbookAndSheets (const QString &filePath) const
{
    QXlsx::ZipReader zr (filePath);
    const auto files = zr.filePaths ();

    bool hasWorkbook	 = false;
    bool hasAnyWorksheet = false;


    for (const QString &p : files)
    {
        if (p == QLatin1String ("xl/workbook.xml"))
            hasWorkbook = true;

        if (p.startsWith (QLatin1String ("xl/worksheets/")))
            hasAnyWorksheet = true;

        if (p.startsWith (QLatin1String ("xl/drawings/")))
        {
            qDebug () << "Workbook contains drawings; skipping to avoid QXlsx debug asserts on malformed anchors";

            return false;
        }
    }

    if (! hasWorkbook || ! hasAnyWorksheet)
    {
        qDebug () << "Missing core workbook parts";

        return false;
    }


    return true;
}

bool ExcelParser::readAndValidateDimension (QXlsx::Document &xlsx, QXlsx::CellRange &outRange) const
{
    outRange = xlsx.dimension ();
    if (! outRange.isValid ())
    {
        qDebug () << "Invalid document range";

        return false;
    }

    qDebug () << "Document range:" << outRange.toString ();


    return true;
}

void ExcelParser::parseAllRows (QXlsx::Document &xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping,
                                QList<PriceTag> &priceTags) const
{
    QString currentSupplier;
    QString currentAddress;

    int maxDataRow = findMaxDataRow (&xlsx, range, mapping);

    for (int row = 1; row <= maxDataRow; ++row)
    {
        PriceTag priceTag;
        if (parseDataRow (&xlsx, row, mapping, currentSupplier, currentAddress, priceTag))
        {
            if (validatePriceTag (priceTag))
            {
                priceTags.append (priceTag);

                qDebug () << "Added price tag:" << priceTag.getBrand () << priceTag.getCategory () << "Price:" << priceTag.getPrice ();
            }
        }
    }
}


bool ExcelParser::findHeaders (QXlsx::Document *xlsx, const QXlsx::CellRange &range, ColumnMapping &mapping)
{
    qDebug () << "Searching for headers in entire document...";

    QMap<QString, int *> columnMap = buildHeaderColumnMap (mapping);

    resetColumnMapping (mapping);


    for (int row = range.firstRow (); row <= range.lastRow (); ++row)
    {
        for (int col = range.firstColumn (); col <= range.lastColumn (); ++col)
        {
            QVariant cellValue = xlsx->read (row, col);
            if (cellValue.isValid ())
            {
                QString cellText		   = cellValue.toString ().trimmed ();
                QString normalizedCellText = normalizeText (cellText);

                if (! cellText.isEmpty ())
                {
                    qDebug () << "Cell at row" << row << "col" << col << ":" << cellText;
                    qDebug () << "Normalized:" << normalizedCellText;

                    if (normalizedCellText.contains (QRegularExpression ("[\\x80-\\xFF]")))
                    {
                        qDebug () << "Contains encoding issues, trying to fix...";

                        QByteArray bytes = cellText.toLocal8Bit ();
                        QString decoded	 = QString::fromLocal8Bit (bytes);

                        qDebug () << "Decoded:" << decoded;
                    }
                }

                tryAssignHeaderMatch (normalizedCellText, col, columnMap);
            }
        }
    }

    if (mapping.nameColumn == -1 || mapping.priceColumn == -1)
    {
        qDebug () << "Missing required columns: Фирма or Цена";
        qDebug () << "Фирма column:" << mapping.nameColumn << "Цена column:" << mapping.priceColumn;

        return false;
    }

    qDebug () << "Header search completed successfully";


    return true;
}

int ExcelParser::findMaxDataRow (QXlsx::Document *xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping) const
{
    int maxRow = range.firstRow ();


    for (int row = range.firstRow (); row <= range.lastRow (); ++row)
    {
        bool hasData = false;

        if (mapping.nameColumn > 0)
        {
            QVariant cellValue = xlsx->read (row, mapping.nameColumn);

            if (cellValue.isValid () && ! cellValue.toString ().trimmed ().isEmpty ())
                hasData = true;
        }

        if (mapping.priceColumn > 0)
        {
            QVariant cellValue = xlsx->read (row, mapping.priceColumn);

            if (cellValue.isValid () && ! cellValue.toString ().trimmed ().isEmpty ())
                hasData = true;
        }

        if (hasData)
            maxRow = row;
    }

    qDebug () << "Max data row found:" << maxRow;


    return maxRow;
}


void ExcelParser::resetColumnMapping (ColumnMapping &mapping) const
{
    mapping.nameColumn				 = -1;
    mapping.priceColumn				 = -1;
    mapping.quantityColumn			 = -1;
    mapping.supplierColumn			 = -1;
    mapping.supplierAddressColumn	 = -1;
    mapping.categoryColumn			 = -1;
    mapping.additionalDataColumn	 = -1;
    mapping.genderColumn			 = -1;
    mapping.brandCountryColumn		 = -1;
    mapping.manufacturingPlaceColumn = -1;
    mapping.materialColumn			 = -1;
    mapping.sizeColumn				 = -1;
    mapping.articleColumn			 = -1;
    mapping.price2Column			 = -1;
}

QMap<QString, int *> ExcelParser::buildHeaderColumnMap (ColumnMapping &mapping) const
{
    return {{"Поставщик", &mapping.supplierColumn},
            {"Адрес", &mapping.supplierAddressColumn},
            {"Фирма", &mapping.nameColumn},
            {"Категория", &mapping.categoryColumn},
            {"Прочие данные", &mapping.additionalDataColumn},
            {"Пол", &mapping.genderColumn},
            {"Страна бренда", &mapping.brandCountryColumn},
            {"Место производства", &mapping.manufacturingPlaceColumn},
            {"Материал", &mapping.materialColumn},
            {"Размер", &mapping.sizeColumn},
            {"Артикул", &mapping.articleColumn},
            {"Цена", &mapping.priceColumn},
            {"Цена 2", &mapping.price2Column},
            {"Количество", &mapping.quantityColumn}};
}

void ExcelParser::tryAssignHeaderMatch (const QString &normalizedCellText, int col, const QMap<QString, int *> &columnMap) const
{
    for (auto it = columnMap.begin (); it != columnMap.end (); ++it)
    {
        QString expectedHeader = it.key ();

        if (normalizedCellText.compare (expectedHeader, Qt::CaseInsensitive) == 0)
        {
            *(it.value ()) = col;

            qDebug () << "Found header '" << expectedHeader << "' at column" << col;

            break;
        }
    }
}

bool ExcelParser::parseDataRow (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier,
                                QString &currentAddress, PriceTag &priceTag) const
{
    bool hasData = false;

    if (parseBrand (xlsx, row, mapping, priceTag))
        hasData = true;

    if (parsePrice (xlsx, row, mapping, priceTag))
        hasData = true;

    if (! hasData)
        return false;


    parseQuantity (xlsx, row, mapping, priceTag);
    updateSupplierAndAddress (xlsx, row, mapping, currentSupplier, currentAddress, priceTag);
    parseAttributes (xlsx, row, mapping, priceTag);


    return true;
}


bool ExcelParser::readTrimmedStringFromCell (QXlsx::Document *xlsx, int row, int col, QString &out) const
{
    if (col <= 0)
        return false;

    QVariant cellValue = xlsx->read (row, col);
    if (! cellValue.isValid ())
        return false;

    QString text = cellValue.toString ().trimmed ();
    if (text.isEmpty ())
        return false;

    out = text;


    return true;
}

bool ExcelParser::readPositiveDoubleFromCell (QXlsx::Document *xlsx, int row, int col, double &out) const
{
    if (col <= 0)
        return false;

    QVariant cellValue = xlsx->read (row, col);
    if (! cellValue.isValid ())
        return false;

    QString priceStr = cellValue.toString ().trimmed ();
    if (priceStr.isEmpty ())
        return false;

    priceStr.remove (QRegularExpression ("[^0-9.,]"));
    priceStr.replace (",", ".");

    double value = priceStr.toDouble ();
    if (value > 0)
    {
        out = value;

        return true;
    }


    return false;
}

bool ExcelParser::parseBrand (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const
{
    QString brand;

    if (readTrimmedStringFromCell (xlsx, row, mapping.nameColumn, brand))
    {
        priceTag.setBrand (brand);

        return true;
    }


    return false;
}

bool ExcelParser::parsePrice (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const
{
    double priceValue = 0.0;

    if (readPositiveDoubleFromCell (xlsx, row, mapping.priceColumn, priceValue))
    {
        priceTag.setPrice (priceValue);

        return true;
    }


    return false;
}

void ExcelParser::parseQuantity (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const
{
    priceTag.setQuantity (1);

    if (mapping.quantityColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.quantityColumn);
        if (cellValue.isValid ())
        {
            bool ok;
            int quantity = cellValue.toInt (&ok);

            if (ok && quantity > 0)
                priceTag.setQuantity (quantity);
            else
                priceTag.setQuantity (1);
        }
        else
            priceTag.setQuantity (1);
    }
}


void ExcelParser::updateSupplierAndAddress (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier,
                                            QString &currentAddress, PriceTag &priceTag) const
{
    QString supplier;

    if (readTrimmedStringFromCell (xlsx, row, mapping.supplierColumn, supplier))
        currentSupplier = supplier;

    priceTag.setSupplier (currentSupplier);


    QString address;

    if (readTrimmedStringFromCell (xlsx, row, mapping.supplierAddressColumn, address))
        currentAddress = address;

    priceTag.setAddress (currentAddress);
}

void ExcelParser::parseAttributes (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, PriceTag &priceTag) const
{
    QString textValue;

    if (readTrimmedStringFromCell (xlsx, row, mapping.categoryColumn, textValue))
        priceTag.setCategory (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.additionalDataColumn, textValue))
        priceTag.setAdditionalData (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.genderColumn, textValue))
        priceTag.setGender (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.brandCountryColumn, textValue))
        priceTag.setBrandCountry (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.manufacturingPlaceColumn, textValue))
        priceTag.setManufacturingPlace (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.materialColumn, textValue))
        priceTag.setMaterial (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.sizeColumn, textValue))
        priceTag.setSize (textValue);

    if (readTrimmedStringFromCell (xlsx, row, mapping.articleColumn, textValue))
        priceTag.setArticle (textValue);


    double price2Value = 0.0;
    if (readPositiveDoubleFromCell (xlsx, row, mapping.price2Column, price2Value))
        priceTag.setPrice2 (price2Value);
}


bool ExcelParser::validatePriceTag (const PriceTag &priceTag) const
{
    if (priceTag.getBrand ().isEmpty ())
    {
        qDebug () << "Price tag validation failed: empty brand";

        return false;
    }

    if (priceTag.getPrice () <= 0)
    {
        qDebug () << "Price tag validation failed: invalid price" << priceTag.getPrice ();

        return false;
    }

    if (priceTag.getQuantity () <= 0)
    {
        qDebug () << "Price tag validation failed: invalid quantity" << priceTag.getQuantity ();

        return false;
    }


    return true;
}

QString ExcelParser::normalizeText (const QString &text)
{
    QString normalized = text;

    normalized = normalized.simplified ();
    normalized = normalized.remove (QRegularExpression ("^[\"']+|[\"']+$"));


    if (normalized.contains (QRegularExpression ("[\\x80-\\xFF]")))
    {
        QByteArray bytes = normalized.toLocal8Bit ();
        QString decoded	 = QString::fromLocal8Bit (bytes);

        if (! decoded.contains (QRegularExpression ("[\\x80-\\xFF]")))
            normalized = decoded;
    }


    return normalized;
}
