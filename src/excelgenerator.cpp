#include "excelgenerator.h"
#include "pricetag.h"
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxcellrange.h>
#include <xlsxworksheet.h>
#include <QBrush>
#include <QDebug>


ExcelGenerator::ExcelGenerator (QObject *parent) : QObject (parent) {}

ExcelGenerator::~ExcelGenerator () {}


bool ExcelGenerator::generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    qDebug () << "Generating Excel document with" << priceTags.size () << "price tags";

    QXlsx::Document xlsx;

    int tagsPerRow = 5;
    int tagWidth   = 4;
    int tagHeight  = 12;

    // Стиль для заголовка "ИП Новиков А.В." (строка 1)
    QXlsx::Format headerFormat;
    headerFormat.setFontBold (false);
    headerFormat.setFontName ("Times New Roman");
    headerFormat.setFontSize (11);
    headerFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    headerFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    headerFormat.setBorderStyle (QXlsx::Format::BorderThin);
    headerFormat.setTopBorderStyle (QXlsx::Format::BorderMedium);
    headerFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    headerFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    headerFormat.setTextWrap (true);

    // Стиль для бренда (строка 2)
    QXlsx::Format brandFormat;
    brandFormat.setFontBold (true);
    brandFormat.setFontName ("Arial");
    brandFormat.setFontSize (12);
    brandFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    brandFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brandFormat.setBorderStyle (QXlsx::Format::BorderThin);
    brandFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    brandFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    brandFormat.setTextWrap (true);

    // Стиль для категории (строка 3)
    QXlsx::Format categoryFormat;
    categoryFormat.setFontName ("Times New Roman");
    categoryFormat.setFontSize (12);
    categoryFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    categoryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    categoryFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    categoryFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    categoryFormat.setTextWrap (true);

    // Стиль для текста страны бренда (строка 4)
    QXlsx::Format brendCountryFormat;
    brendCountryFormat.setFontSize (9);
    brendCountryFormat.setFontName ("Times New Roman");
    brendCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    brendCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brendCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    brendCountryFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    brendCountryFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    brendCountryFormat.setTextWrap (true);

    // Стиль для мелкого текста (строка 5)
    QXlsx::Format developCountryFormat;
    developCountryFormat.setFontSize (9);
    developCountryFormat.setFontName ("Times New Roman");
    developCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    developCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    developCountryFormat.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    developCountryFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    developCountryFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    developCountryFormat.setTextWrap (true);

    // Стиль для мелкого текста (строка 6) - заголовок материала
    QXlsx::Format materialHeaderFormat;
    materialHeaderFormat.setFontSize (8);
    materialHeaderFormat.setFontName ("Times New Roman");
    materialHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialHeaderFormat.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialHeaderFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    materialHeaderFormat.setRightBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialHeaderFormat.setTopBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialHeaderFormat.setBottomBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialHeaderFormat.setTextWrap (true);

    // Стиль для мелкого текста (строка 6) - значение материала
    QXlsx::Format materialValueFormat;
    materialValueFormat.setFontSize (10);
    materialValueFormat.setFontName ("Times New Roman");
    materialValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialValueFormat.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialValueFormat.setLeftBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialValueFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    materialValueFormat.setTopBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialValueFormat.setBottomBorderStyle (QXlsx::Format::BorderSlantDashDot);
    materialValueFormat.setTextWrap (true);

    // Стиль для мелкого текста (строка 7) - заголовок артикула
    QXlsx::Format articulHeaderFormat;
    articulHeaderFormat.setFontSize (8);
    articulHeaderFormat.setFontName ("Times New Roman");
    articulHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulHeaderFormat.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulHeaderFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    articulHeaderFormat.setRightBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulHeaderFormat.setTopBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulHeaderFormat.setBottomBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulHeaderFormat.setTextWrap (true);

    // Стиль для мелкого текста (строка 7) - значение артикула
    QXlsx::Format articulValueFormat;
    articulValueFormat.setFontSize (11);
    articulValueFormat.setFontName ("Times New Roman");
    articulValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulValueFormat.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulValueFormat.setLeftBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulValueFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    articulValueFormat.setTopBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulValueFormat.setBottomBorderStyle (QXlsx::Format::BorderSlantDashDot);
    articulValueFormat.setTextWrap (true);

    // Стиль для цены (строка 8, ячейка 1) - заголовок цены
    QXlsx::Format priceFormatCell1;
    priceFormatCell1.setFontBold (true);
    priceFormatCell1.setFontSize (10);
    priceFormatCell1.setFontName ("Times New Roman");
    priceFormatCell1.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormatCell1.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    priceFormatCell1.setBorderStyle (QXlsx::Format::BorderSlantDashDot);
    priceFormatCell1.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell1.setRightBorderStyle (QXlsx::Format::BorderSlantDashDot);
    priceFormatCell1.setTopBorderStyle (QXlsx::Format::BorderSlantDashDot);
    priceFormatCell1.setBottomBorderStyle (QXlsx::Format::BorderSlantDashDot);

    // Стиль для цены (строка 8, ячейка 2) - значение цены с жирной обводкой
    QXlsx::Format priceFormatCell2;
    priceFormatCell2.setFontBold (true);
    priceFormatCell2.setFontSize (12);
    priceFormatCell2.setFontName ("Times New Roman");
    priceFormatCell2.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormatCell2.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    priceFormatCell2.setBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setRightBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setTopBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setBottomBorderStyle (QXlsx::Format::BorderMedium);

    // Стиль для зачеркнутой цены (строка 8, ячейка 1)
    QXlsx::Format strikePriceFormat;
    strikePriceFormat.setFontBold (true);
    strikePriceFormat.setFontSize (12);
    strikePriceFormat.setFontName ("Times New Roman");
    strikePriceFormat.setFontStrikeOut (true);
    strikePriceFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    strikePriceFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    strikePriceFormat.setBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    strikePriceFormat.setRightBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);

    // Стиль для подписи (строка 9)
    QXlsx::Format signatureFormat;
    signatureFormat.setFontSize (8);
    signatureFormat.setFontName ("Times New Roman");
    signatureFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    signatureFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    signatureFormat.setBorderStyle (QXlsx::Format::BorderThin);
    signatureFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    signatureFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    signatureFormat.setTextWrap (true);

    // Стиль для поставщика (строка 10)
    QXlsx::Format supplierFormat;
    supplierFormat.setFontSize (7);
    supplierFormat.setFontName ("Times New Roman");
    supplierFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    supplierFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    supplierFormat.setBorderStyle (QXlsx::Format::BorderThin);
    supplierFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    supplierFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    supplierFormat.setTextWrap (true);

    // Стиль для адреса (строки 11-12)
    QXlsx::Format addressFormat;
    addressFormat.setFontSize (7);
    addressFormat.setFontName ("Times New Roman");
    addressFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    addressFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    addressFormat.setBorderStyle (QXlsx::Format::BorderThin);
    addressFormat.setLeftBorderStyle (QXlsx::Format::BorderMedium);
    addressFormat.setRightBorderStyle (QXlsx::Format::BorderMedium);
    addressFormat.setBottomBorderStyle (QXlsx::Format::BorderMedium);
    addressFormat.setTextWrap (true);

    for (int i = 1; i <= 1000; ++i)
        xlsx.setRowHeight (i, 15.0);

    int tagIndex = 0;

    for (const PriceTag &tag : priceTags)
    {
        for (int q = 0; q < tag.getQuantity (); ++q)
        {
            // Вычисляем позицию текущего ценника (начинаем с 2-й строки и 2-го столбца)
            int col = (tagIndex % tagsPerRow) * tagWidth + 2;
            int row = (tagIndex / tagsPerRow) * tagHeight + 2;

            qDebug () << "Creating price tag" << tagIndex << "at position (" << row << "," << col << ")";

            // Настраиваем высоту строк для этого ценника
            for (int r = row; r < row + 9; ++r) { xlsx.setRowHeight (r, 15.0); }
            xlsx.setRowHeight (row + 9, 13.5);  // 10-я строка
            xlsx.setRowHeight (row + 10, 9.75); // 11-я строка
            xlsx.setRowHeight (row + 11, 9.75); // 12-я строка

            // Настраиваем ширину колонок
            xlsx.setColumnWidth (col, 7.0);
            for (int c = col + 1; c < col + tagWidth; ++c)
                xlsx.setColumnWidth (c, 3.57);

            // Строка 1: "ИП Новиков А.В." (всегда одинаковый текст)
            xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagWidth - 1), headerFormat);
            xlsx.write (row, col, "ИП Новиков А.В.", headerFormat);

            // Строка 2: Фирма (бренд)
            xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagWidth - 1), brandFormat);
            xlsx.write (row + 1, col, tag.getBrand (), brandFormat);

            // Строка 3: Категория + пол (если категория <= 12 символов)
            xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagWidth - 1), categoryFormat);
            QString categoryText = tag.getCategory ();

            if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
                categoryText += " " + tag.getGender ();
            xlsx.write (row + 2, col, categoryText, categoryFormat);

            // Строка 4: Заголовок страны производителя
            xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagWidth - 1), brendCountryFormat);
            xlsx.write (row + 3, col, "Страна: " + tag.getBrandCountry (), brendCountryFormat);

            // Строка 5: Место
            xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagWidth - 1), developCountryFormat);
            xlsx.write (row + 4, col, "Место: " + tag.getManufacturingPlace (), developCountryFormat);

            // Строка 6: Материал - разделена на две ячейки
            // Первая ячейка: заголовок "Матер-л:"
            xlsx.write (row + 5, col, "Матер-л:", materialHeaderFormat);
            // Остальные три ячейки объединены для значения
            xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagWidth - 1), materialValueFormat);
            xlsx.write (row + 5, col + 1, tag.getMaterial (), materialValueFormat);

            // Строка 7: Артикул - разделена на две ячейки
            // Первая ячейка: пустая или заголовок
            xlsx.write (row + 6, col, "", articulHeaderFormat);
            // Остальные три ячейки объединены для значения артикула
            xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagWidth - 1), articulValueFormat);
            xlsx.write (row + 6, col + 1, tag.getArticle (), articulValueFormat);

            // Строка 8: Цена - разделена на две ячейки
            // Первая ячейка: заголовок цены
            if (tag.getPrice2 () > 0)
            {
                // Если есть Цена 2, то основная цена зачеркивается
                QString priceText = "Цена " + QString::number (tag.getPrice ()) + " =";
                xlsx.write (row + 7, col, priceText, strikePriceFormat);
                
                // Остальные три ячейки объединены для новой цены с жирной обводкой
                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagWidth - 1), priceFormatCell2);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", priceFormatCell2);
            }
            else
            {
                QString priceText = "Цена";
                xlsx.write (row + 7, col, priceText, priceFormatCell1);
                
                // Остальные три ячейки объединены для цены с жирной обводкой
                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagWidth - 1), priceFormatCell2);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", priceFormatCell2);
            }

            // Строка 9: Подпись
            xlsx.mergeCells (QXlsx::CellRange (row + 8, col, row + 8, col + tagWidth - 1), signatureFormat);
            xlsx.write (row + 8, col, "Подпись", signatureFormat);

            // Строка 10: Поставщик
            xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagWidth - 1), supplierFormat);
            xlsx.write (row + 9, col, "Поставщик: " + tag.getSupplier (), supplierFormat);

            // Строки 11-12: Адрес поставщика (разбивается на две строки)
            QString address          = tag.getAddress ();
            QStringList addressParts = address.split (", ");

            // Первая часть адреса
            xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagWidth - 1), addressFormat);
            if (addressParts.size () > 0)
                xlsx.write (row + 10, col, addressParts[0], addressFormat);

            // Вторая часть адреса
            xlsx.mergeCells (QXlsx::CellRange (row + 11, col, row + 11, col + tagWidth - 1), addressFormat);
            if (addressParts.size () > 1)
            {
                QString secondPart = addressParts.mid (1).join (", ");
                xlsx.write (row + 11, col, secondPart, addressFormat);
            }

            tagIndex++;
        }
    }

    qDebug () << "Saving Excel document to:" << outputPath;
    bool result = xlsx.saveAs (outputPath);
    qDebug () << "Save result:" << result;

    return result;
} 
