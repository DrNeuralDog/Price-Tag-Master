#include "excelgenerator.h"
#include <QBrush>
#include <QDebug>
#include <xlsxcellrange.h>
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxworksheet.h>
#include "pricetag.h"


ExcelGenerator::ExcelGenerator (QObject *parent) : QObject (parent) {}

ExcelGenerator::~ExcelGenerator () {}


static double mmToExcelColumnWidth (double mm)
{
    // Conservative mapping: 1 width unit ≈ 2.4 mm (approx for Calibri 11 at 96 DPI)
    return mm / 2.4;
}

// Excel row height is in points (1/72 inch). 1 mm = 2.83465 points
static double mmToRowHeightPt (double mm)
{
    return mm * 2.834645669; // points
}

// Compute grid fitting per PRD §6
static void computeGrid (const ExcelGenerator::ExcelLayoutConfig &cfg, int &nCols, int &nRows)
{
    const double pageW	= 210.0;
    const double pageH	= 297.0;
    const double availW = pageW - cfg.marginLeftMm - cfg.marginRightMm;
    const double availH = pageH - cfg.marginTopMm - cfg.marginBottomMm;
    nCols				= std::max (1, static_cast<int> (std::floor ((availW + cfg.spacingHMm) / (cfg.tagWidthMm + cfg.spacingHMm))));
    nRows				= std::max (1, static_cast<int> (std::floor ((availH + cfg.spacingVMm) / (cfg.tagHeightMm + cfg.spacingVMm))));
}

// Convert mm position to starting Excel row/col offsets given a base origin (row0/col0)
static void placeTagCellRange (const ExcelGenerator::ExcelLayoutConfig &cfg, int gridCol, int gridRow, int originCol, int originRow,
                               int &startCol, int &startRow, int &tagCols, int &tagRows)
{
    tagCols = 4;
    tagRows = 13;

    startCol = originCol + gridCol * tagCols;
    startRow = originRow + gridRow * tagRows;
}

bool ExcelGenerator::generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    qDebug () << "Generating Excel document with" << priceTags.size () << "price tags";

    QXlsx::Document xlsx;


    int nCols = 1, nRows = 1;
    computeGrid (layoutConfig, nCols, nRows);


    const int originCol = 2;
    const int originRow = 2;


    const double spacerWcol = mmToExcelColumnWidth (layoutConfig.spacingHMm);
    const double spacerHrow = mmToRowHeightPt (layoutConfig.spacingVMm);


    QXlsx::Format headerFormat;
    headerFormat.setFontBold (false);
    headerFormat.setFontName ("Times New Roman");
    headerFormat.setFontSize (11);
    headerFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    headerFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    headerFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    headerFormat.setTopBorderStyle (QXlsx::Format::BorderThick);
    headerFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    headerFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    headerFormat.setTextWrap (true);

    QXlsx::Format brandFormat;
    brandFormat.setFontBold (true);
    brandFormat.setFontName ("Arial");
    brandFormat.setFontSize (12);
    brandFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    brandFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brandFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    brandFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    brandFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    brandFormat.setTextWrap (true);

    QXlsx::Format categoryFormat;
    categoryFormat.setFontName ("Times New Roman");
    categoryFormat.setFontSize (12);
    categoryFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    categoryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    categoryFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    categoryFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    categoryFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    categoryFormat.setTextWrap (true);

    QXlsx::Format brendCountryFormat;
    brendCountryFormat.setFontSize (9);
    brendCountryFormat.setFontName ("Times New Roman");
    brendCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    brendCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brendCountryFormat.setFontBold (true);
    brendCountryFormat.setFontItalic (true);
    brendCountryFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    brendCountryFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    brendCountryFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    brendCountryFormat.setTextWrap (true);

    QXlsx::Format developCountryFormat;
    developCountryFormat.setFontSize (9);
    developCountryFormat.setFontName ("Times New Roman");
    developCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    developCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    developCountryFormat.setFontBold (true);
    developCountryFormat.setFontItalic (true);
    developCountryFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    developCountryFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    developCountryFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    developCountryFormat.setTextWrap (true);

    QXlsx::Format materialHeaderFormat;
    materialHeaderFormat.setFontSize (8);
    materialHeaderFormat.setFontName ("Times New Roman");
    materialHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialHeaderFormat.setFontBold (true);
    materialHeaderFormat.setFontItalic (true);
    materialHeaderFormat.setBorderStyle (QXlsx::Format::BorderThin);
    materialHeaderFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    materialHeaderFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    materialHeaderFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    materialHeaderFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);
    materialHeaderFormat.setTextWrap (true);

    QXlsx::Format materialValueFormat;
    materialValueFormat.setFontSize (10);
    materialValueFormat.setFontName ("Times New Roman");
    materialValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialValueFormat.setBorderStyle (QXlsx::Format::BorderThin);
    materialValueFormat.setLeftBorderStyle (QXlsx::Format::BorderThin);
    materialValueFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    materialValueFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    materialValueFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);
    materialValueFormat.setTextWrap (true);

    QXlsx::Format articulHeaderFormat;
    articulHeaderFormat.setFontSize (8);
    articulHeaderFormat.setFontName ("Times New Roman");
    articulHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulHeaderFormat.setFontBold (true);
    articulHeaderFormat.setFontItalic (true);
    articulHeaderFormat.setBorderStyle (QXlsx::Format::BorderThin);
    articulHeaderFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    articulHeaderFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    articulHeaderFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    articulHeaderFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);
    articulHeaderFormat.setTextWrap (true);

    QXlsx::Format articulValueFormat;
    articulValueFormat.setFontSize (11);
    articulValueFormat.setFontName ("Times New Roman");
    articulValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulValueFormat.setFontBold (true);
    articulValueFormat.setFontItalic (false);
    articulValueFormat.setBorderStyle (QXlsx::Format::BorderThin);
    articulValueFormat.setLeftBorderStyle (QXlsx::Format::BorderThin);
    articulValueFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    articulValueFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    articulValueFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);
    articulValueFormat.setTextWrap (true);

    QXlsx::Format priceFormatCell1;
    priceFormatCell1.setFontBold (true);
    priceFormatCell1.setFontSize (10);
    priceFormatCell1.setFontName ("Times New Roman");
    priceFormatCell1.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormatCell1.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    priceFormatCell1.setBorderStyle (QXlsx::Format::BorderThin);
    priceFormatCell1.setLeftBorderStyle (QXlsx::Format::BorderThick);
    priceFormatCell1.setRightBorderStyle (QXlsx::Format::BorderThin);
    priceFormatCell1.setTopBorderStyle (QXlsx::Format::BorderThin);
    priceFormatCell1.setBottomBorderStyle (QXlsx::Format::BorderThin);


    QXlsx::Format priceFormatCell2;
    priceFormatCell2.setFontBold (true);
    priceFormatCell2.setFontSize (12);
    priceFormatCell2.setFontName ("Times New Roman");
    priceFormatCell2.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormatCell2.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    priceFormatCell2.setBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setLeftBorderStyle (QXlsx::Format::BorderThick);
    priceFormatCell2.setRightBorderStyle (QXlsx::Format::BorderThick);
    priceFormatCell2.setTopBorderStyle (QXlsx::Format::BorderMedium);
    priceFormatCell2.setBottomBorderStyle (QXlsx::Format::BorderMedium);

    QXlsx::Format strikePriceFormat;
    strikePriceFormat.setFontBold (true);
    strikePriceFormat.setFontSize (12);
    strikePriceFormat.setFontName ("Times New Roman");
    strikePriceFormat.setFontStrikeOut (true);
    strikePriceFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    strikePriceFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    strikePriceFormat.setBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    strikePriceFormat.setRightBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setBottomBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format signatureFormat;
    signatureFormat.setFontSize (8);
    signatureFormat.setFontName ("Times New Roman");
    signatureFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    signatureFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    signatureFormat.setBorderStyle (QXlsx::Format::BorderThin);
    signatureFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    signatureFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    signatureFormat.setTextWrap (true);

    QXlsx::Format supplierFormat;
    supplierFormat.setFontSize (7);
    supplierFormat.setFontName ("Times New Roman");
    supplierFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    supplierFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    supplierFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    supplierFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    supplierFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    supplierFormat.setTextWrap (true);

    QXlsx::Format addressFormat;
    addressFormat.setFontSize (7);
    addressFormat.setFontName ("Times New Roman");
    addressFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    addressFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    addressFormat.setFontBold (true);
    addressFormat.setFontItalic (true);
    addressFormat.setBorderStyle (QXlsx::Format::BorderMedium);
    addressFormat.setLeftBorderStyle (QXlsx::Format::BorderThick);
    addressFormat.setRightBorderStyle (QXlsx::Format::BorderThick);
    addressFormat.setBottomBorderStyle (QXlsx::Format::BorderMedium);
    addressFormat.setTextWrap (true);

    for (int i = 1; i <= 2000; ++i)
        xlsx.setRowHeight (i, mmToRowHeightPt (4.0));

    int tagIndex = 0;

    for (const PriceTag &tag : priceTags)
    {
        for (int q = 0; q < tag.getQuantity (); ++q)
        {
            const int perPage	= nCols * nRows;
            const int pageIdx	= tagIndex / perPage;
            const int idxInPage = tagIndex % perPage;
            const int gridRow	= idxInPage / nCols;
            const int gridCol	= idxInPage % nCols;


            int col = 0, row = 0, tagCols = 0, tagRows = 0;
            placeTagCellRange (layoutConfig, gridCol + pageIdx * nCols, gridRow, originCol, originRow, col, row, tagCols, tagRows);

            qDebug () << "Creating price tag" << tagIndex << "at position (" << row << "," << col << ")";


            xlsx.setColumnWidth (col + 0, 7.00);
            xlsx.setColumnWidth (col + 1, 3.57);
            xlsx.setColumnWidth (col + 2, 3.57);
            xlsx.setColumnWidth (col + 3, 3.43);


            const double rhPts[13] = {15.00, 15.75, 15.75, 12.75, 12.75, 12.75, 12.75, 15.75, 10.50, 13.50, 13.50, 13.50, 13.50};
            for (int r = 0; r < tagRows; ++r)
                xlsx.setRowHeight (row + r, rhPts[r]);


            xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagCols - 1), headerFormat);
            xlsx.write (row, col, "ИП Новиков А.В.", headerFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagCols - 1), brandFormat);
            xlsx.write (row + 1, col, tag.getBrand (), brandFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagCols - 1), categoryFormat);
            QString categoryText = tag.getCategory ();

            if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
                categoryText += " " + tag.getGender ();
            xlsx.write (row + 2, col, categoryText, categoryFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagCols - 1), brendCountryFormat);
            xlsx.write (row + 3, col, "Страна: " + tag.getBrandCountry (), brendCountryFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagCols - 1), developCountryFormat);
            xlsx.write (row + 4, col, "Место: " + tag.getManufacturingPlace (), developCountryFormat);


            xlsx.write (row + 5, col, "Матер-л:", materialHeaderFormat);
            xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagCols - 1), materialValueFormat);
            xlsx.write (row + 5, col + 1, tag.getMaterial (), materialValueFormat);


            xlsx.write (row + 6, col, "Артикул:", articulHeaderFormat);

            xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagCols - 1), articulValueFormat);
            xlsx.write (row + 6, col + 1, tag.getArticle (), articulValueFormat);


            if (tag.getPrice2 () > 0)
            {
                QString priceText = "Цена " + QString::number (tag.getPrice ()) + " =";
                xlsx.write (row + 7, col, priceText, strikePriceFormat);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), priceFormatCell2);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", priceFormatCell2);
            }
            else
            {
                QString priceText = "Цена";
                xlsx.write (row + 7, col, priceText, priceFormatCell1);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), priceFormatCell2);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", priceFormatCell2);
            }


            xlsx.mergeCells (QXlsx::CellRange (row + 8, col, row + 8, col + tagCols - 1), signatureFormat);
            xlsx.write (row + 8, col, "Подпись", signatureFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagCols - 1), supplierFormat);
            xlsx.write (row + 9, col, "Поставщик: " + tag.getSupplier (), supplierFormat);


            QString address			 = tag.getAddress ();
            QStringList addressParts = address.split (", ");


            xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagCols - 1), addressFormat);
            if (addressParts.size () > 0)
                xlsx.write (row + 10, col, addressParts[0], addressFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 11, col, row + 11, col + tagCols - 1), addressFormat);
            if (addressParts.size () > 1)
                xlsx.write (row + 11, col, addressParts[1], addressFormat);


            xlsx.mergeCells (QXlsx::CellRange (row + 12, col, row + 12, col + tagCols - 1), addressFormat);
            if (addressParts.size () > 2)
            {
                QString rest = addressParts.mid (2).join (", ");
                xlsx.write (row + 12, col, rest, addressFormat);
            }

            tagIndex++;
        }
    }


    const QXlsx::CellRange used = xlsx.dimension ();
    if (used.isValid ())
    {
        const QString printArea = QString ("='%1'!%2").arg (xlsx.currentWorksheet ()->sheetName (), used.toString ());

        xlsx.defineName ("_xlnm.Print_Area", printArea);
    }

    qDebug () << "Saving Excel document to:" << outputPath;
    bool result = xlsx.saveAs (outputPath);
    qDebug () << "Save result:" << result;


    return result;
}
