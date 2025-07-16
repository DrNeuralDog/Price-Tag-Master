#include "wordgenerator.h"
#include <QTextDocumentWriter>
#include <QTextTableFormat>
#include <QTextTableCell>
#include <QTextLength>
#include <QColor>

WordGenerator::WordGenerator (QObject *parent) : QObject (parent) {}

WordGenerator::~WordGenerator () {}

bool WordGenerator::generateWordDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    if (priceTags.isEmpty ())
    {
        qDebug () << "No price tags to generate";
        return false;
    }

    QTextDocument document;
    QTextCursor cursor (&document);


    int totalTags = 0;
    for (const PriceTag &tag : priceTags) { totalTags += tag.getQuantity (); }

    int rowsNeeded = (totalTags + tagsPerRow - 1) / tagsPerRow;


    QTextTableFormat tableFormat;
    tableFormat.setAlignment (Qt::AlignLeft);
    tableFormat.setCellPadding (5);
    tableFormat.setCellSpacing (0);
    tableFormat.setBorder (0);

    QTextTable *table = cursor.insertTable (rowsNeeded, tagsPerRow, tableFormat);


    QVector<QTextLength> columnWidths;
    for (int i = 0; i < tagsPerRow; ++i) { columnWidths.append (QTextLength (QTextLength::FixedLength, tagWidth)); }
    table->format ().setColumnWidthConstraints (columnWidths);


    int currentRow = 0;
    int currentCol = 0;

    for (const PriceTag &priceTag : priceTags)
    {
        for (int i = 0; i < priceTag.getQuantity (); ++i)
        {
            if (currentCol >= tagsPerRow)
            {
                currentRow++;
                currentCol = 0;
            }

            if (currentRow >= rowsNeeded) { break; }

            createPriceTagCell (table, currentRow, currentCol, priceTag);
            currentCol++;
        }
    }


    QTextDocumentWriter writer (outputPath);
    writer.setFormat ("odf"); // OpenDocument format

    return writer.write (&document);
}

void WordGenerator::createPriceTagCell (QTextTable *table, int row, int col, const PriceTag &priceTag)
{
    QTextTableCell cell    = table->cellAt (row, col);
    QTextCursor cellCursor = cell.firstCursorPosition ();


    QTextCharFormat cellFormat = cell.format ();
    cellFormat.setBackground (QBrush (Qt::white));
    cell.setFormat (cellFormat);


    addBrandInfo (cellCursor, priceTag);
    addCategoryInfo (cellCursor, priceTag);
    addPriceInfo (cellCursor, priceTag);
    addProductInfo (cellCursor, priceTag);
    addSupplierInfo (cellCursor, priceTag);
}

void WordGenerator::addBrandInfo (QTextCursor &cursor, const PriceTag &priceTag)
{
    if (! priceTag.getBrand ().isEmpty ())
    {
        QTextCharFormat brandFormat = createFormat (12, true);
        addTextWithFormat (cursor, priceTag.getBrand (), brandFormat);
    }
}

void WordGenerator::addCategoryInfo (QTextCursor &cursor, const PriceTag &priceTag)
{
    QString category = priceTag.getFormattedCategory ();
    if (! category.isEmpty ())
    {
        QTextCharFormat categoryFormat = createFormat (10, false);
        addTextWithFormat (cursor, category, categoryFormat);
    }
}

void WordGenerator::addPriceInfo (QTextCursor &cursor, const PriceTag &priceTag)
{
    if (priceTag.hasDiscount ())
    {
        // Показываем перечеркнутую оригинальную цену и цену со скидкой
        QTextCharFormat originalPriceFormat = createFormat (14, true, Qt::gray);
        originalPriceFormat.setFontStrikeOut (true);
        addTextWithFormat (cursor, formatPrice (priceTag.getOriginalPrice ()), originalPriceFormat, false);

        cursor.insertText (" ");

        QTextCharFormat discountPriceFormat = createFormat (16, true, Qt::red);
        addTextWithFormat (cursor, formatPrice (priceTag.getDiscountPrice ()), discountPriceFormat);
    }
    else
    {
        // Показываем обычную цену
        QTextCharFormat priceFormat = createFormat (16, true);
        addTextWithFormat (cursor, formatPrice (priceTag.getPrice ()), priceFormat);
    }
}

void WordGenerator::addProductInfo (QTextCursor &cursor, const PriceTag &priceTag)
{
    // Артикул
    if (! priceTag.getArticle ().isEmpty ())
    {
        QTextCharFormat articleFormat = createFormat (8, false, Qt::darkGray);
        addTextWithFormat (cursor, "Арт: " + priceTag.getArticle (), articleFormat);
    }

    // Размер
    if (! priceTag.getSize ().isEmpty ())
    {
        QTextCharFormat sizeFormat = createFormat (8, false);
        addTextWithFormat (cursor, "Размер: " + priceTag.getSize (), sizeFormat);
    }

    // Материал
    if (! priceTag.getMaterial ().isEmpty ())
    {
        QTextCharFormat materialFormat = createFormat (8, false);
        addTextWithFormat (cursor, priceTag.getMaterial (), materialFormat);
    }

    // Страна производства
    if (! priceTag.getManufacturingPlace ().isEmpty ())
    {
        QTextCharFormat countryFormat = createFormat (8, false, Qt::darkGray);
        addTextWithFormat (cursor, priceTag.getManufacturingPlace (), countryFormat);
    }
}

void WordGenerator::addSupplierInfo (QTextCursor &cursor, const PriceTag &priceTag)
{
    if (! priceTag.getSupplier ().isEmpty ())
    {
        QTextCharFormat supplierFormat = createFormat (7, false, Qt::darkBlue);
        addTextWithFormat (cursor, priceTag.getSupplier (), supplierFormat);
    }
}

void WordGenerator::addTextWithFormat (QTextCursor &cursor, const QString &text, const QTextCharFormat &format, bool newLine)
{
    cursor.insertText (text, format);
    if (newLine) { cursor.insertBlock (); }
}

QTextCharFormat WordGenerator::createFormat (int fontSize, bool bold, const QColor &color)
{
    QTextCharFormat format;
    format.setFontPointSize (fontSize);
    format.setFontWeight (bold ? QFont::Bold : QFont::Normal);
    format.setForeground (QBrush (color));
    return format;
}

QString WordGenerator::formatPrice (double price) { return QString::number (price, 'f', 0) + " ₽"; }
