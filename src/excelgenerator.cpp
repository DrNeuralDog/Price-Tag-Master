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
    headerFormat.setFontBold (true);
    headerFormat.setFontSize (10);
    headerFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    headerFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    headerFormat.setBorderStyle (QXlsx::Format::BorderThin);
    headerFormat.setTextWrap (true);

    // Стиль для бренда (строка 2)
    QXlsx::Format brandFormat;
    brandFormat.setFontBold (true);
    brandFormat.setFontSize (12);
    brandFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    brandFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brandFormat.setBorderStyle (QXlsx::Format::BorderThin);
    brandFormat.setTextWrap (true);

    // Стиль для категории (строка 3)
    QXlsx::Format categoryFormat;
    categoryFormat.setFontSize (10);
    categoryFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    categoryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    categoryFormat.setTextWrap (true);

    // Стиль для мелкого текста (строки 4-7)
    QXlsx::Format smallFormat;
    smallFormat.setFontSize (9);
    smallFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    smallFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    smallFormat.setBorderStyle (QXlsx::Format::BorderThin);
    smallFormat.setTextWrap (true);

    // Стиль для цены (строка 8)
    QXlsx::Format priceFormat;
    priceFormat.setFontBold (true);
    priceFormat.setFontSize (14);
    priceFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    priceFormat.setBorderStyle (QXlsx::Format::BorderThin);

    // Стиль для зачеркнутой цены
    QXlsx::Format strikeFormat;
    strikeFormat.setFontBold (true);
    strikeFormat.setFontSize (14);
    strikeFormat.setFontStrikeOut (true);
    strikeFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    strikeFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    strikeFormat.setBorderStyle (QXlsx::Format::BorderThin);

    // Стиль для подписи (строка 9)
    QXlsx::Format signatureFormat;
    signatureFormat.setFontSize (9);
    signatureFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    signatureFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    signatureFormat.setBorderStyle (QXlsx::Format::BorderThin);

    // Стиль для поставщика (строка 10)
    QXlsx::Format supplierFormat;
    supplierFormat.setFontSize (9);
    supplierFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    supplierFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    supplierFormat.setBorderStyle (QXlsx::Format::BorderThin);
    supplierFormat.setTextWrap (true);

    // Стиль для адреса (строки 11-12)
    QXlsx::Format addressFormat;
    addressFormat.setFontSize (8);
    addressFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    addressFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    addressFormat.setBorderStyle (QXlsx::Format::BorderThin);
    addressFormat.setTextWrap (true);


    for (int i = 1; i <= 1000; ++i)
        xlsx.setRowHeight (i, 15.0);


    int tagIndex = 0;

    for (const PriceTag &tag : priceTags)
    {
        for (int q = 0; q < tag.getQuantity (); ++q)
        {
            // Вычисляем позицию текущего ценника
            int col = (tagIndex % tagsPerRow) * tagWidth + 1;
            int row = (tagIndex / tagsPerRow) * tagHeight + 1;

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


            // Строка 4: Страна
            xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagWidth - 1), smallFormat);
            xlsx.write (row + 3, col, "Страна: " + tag.getBrandCountry (), smallFormat);


            // Строка 5: Место
            xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagWidth - 1), smallFormat);
            xlsx.write (row + 4, col, "Место: " + tag.getManufacturingPlace (), smallFormat);


            // Строка 6: Матер-л
            xlsx.mergeCells (QXlsx::CellRange (row + 5, col, row + 5, col + tagWidth - 1), smallFormat);
            xlsx.write (row + 5, col, "Матер-л: " + tag.getMaterial (), smallFormat);


            // Строка 7: Артикул
            xlsx.mergeCells (QXlsx::CellRange (row + 6, col, row + 6, col + tagWidth - 1), smallFormat);
            xlsx.write (row + 6, col, tag.getArticle (), smallFormat);


            // Строка 8: Цена (с учетом Цена 2)
            xlsx.mergeCells (QXlsx::CellRange (row + 7, col, row + 7, col + tagWidth - 1), priceFormat);
            if (tag.getPrice2 () > 0)
            {
                // Если есть Цена 2, то основная цена зачеркивается
                QString priceText = "Цена " + QString::number (tag.getPrice ()) + " = " + QString::number (tag.getPrice2 ()) + " =";
                xlsx.write (row + 7, col, priceText, priceFormat);
                // TODO: Нужно добавить зачеркивание для первой цены
            }
            else
            {
                QString priceText = "Цена " + QString::number (tag.getPrice ()) + " =";
                xlsx.write (row + 7, col, priceText, priceFormat);
            }


            // Строка 9: Подпись (пустая)
            xlsx.mergeCells (QXlsx::CellRange (row + 8, col, row + 8, col + tagWidth - 1), signatureFormat);
            xlsx.write (row + 8, col, "Подпись", signatureFormat);


            // Строка 10: Поставщик
            xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagWidth - 1), supplierFormat);
            xlsx.write (row + 9, col, "Поставщик: " + tag.getSupplier (), supplierFormat);


            // Строки 11-12: Адрес поставщика (разбиваем на две строки)
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
