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


static double mmToExcelColumnWidth (double mm) { return mm / 2.4; }

// Excel row height is in points (1/72 inch). 1 mm = 2.83465 points
static double mmToRowHeightPt (double mm)
{
    return mm * 2.834645669; // points
}

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
    tagRows = 12;

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
    headerFormat.setBorderStyle (QXlsx::Format::BorderThin);
    headerFormat.setTextWrap (true);

    QXlsx::Format brandFormat;
    brandFormat.setFontBold (true);
    brandFormat.setFontName ("Arial");
    brandFormat.setFontSize (12);
    brandFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    brandFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brandFormat.setBorderStyle (QXlsx::Format::BorderThin);
    brandFormat.setTextWrap (true);

    QXlsx::Format categoryFormat;
    categoryFormat.setFontName ("Times New Roman");
    categoryFormat.setFontSize (12);
    categoryFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    categoryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    categoryFormat.setTextWrap (true);

    QXlsx::Format brendCountryFormat;
    brendCountryFormat.setFontSize (9);
    brendCountryFormat.setFontName ("Times New Roman");
    brendCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    brendCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    brendCountryFormat.setFontBold (true);
    brendCountryFormat.setFontItalic (true);
    brendCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    brendCountryFormat.setTextWrap (true);

    QXlsx::Format developCountryFormat;
    developCountryFormat.setFontSize (9);
    developCountryFormat.setFontName ("Times New Roman");
    developCountryFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    developCountryFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    developCountryFormat.setFontBold (true);
    developCountryFormat.setFontItalic (true);
    developCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);
    developCountryFormat.setTextWrap (true);

    QXlsx::Format materialHeaderFormat;
    materialHeaderFormat.setFontSize (8);
    materialHeaderFormat.setFontName ("Times New Roman");
    materialHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialHeaderFormat.setFontBold (true);
    materialHeaderFormat.setFontItalic (true);
    materialHeaderFormat.setTextWrap (true);

    QXlsx::Format materialValueFormat;
    materialValueFormat.setFontSize (10);
    materialValueFormat.setFontName ("Times New Roman");
    materialValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    materialValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    materialValueFormat.setTextWrap (true);

    QXlsx::Format articulHeaderFormat;
    articulHeaderFormat.setFontSize (8);
    articulHeaderFormat.setFontName ("Times New Roman");
    articulHeaderFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulHeaderFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulHeaderFormat.setFontBold (true);
    articulHeaderFormat.setFontItalic (true);
    articulHeaderFormat.setTextWrap (true);

    QXlsx::Format articulValueFormat;
    articulValueFormat.setFontSize (11);
    articulValueFormat.setFontName ("Times New Roman");
    articulValueFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    articulValueFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    articulValueFormat.setFontBold (true);
    articulValueFormat.setFontItalic (false);
    articulValueFormat.setTextWrap (true);

    QXlsx::Format priceFormatCell1;
    priceFormatCell1.setFontBold (true);
    priceFormatCell1.setFontSize (10);
    priceFormatCell1.setFontName ("Times New Roman");
    priceFormatCell1.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    priceFormatCell1.setVerticalAlignment (QXlsx::Format::AlignVCenter);


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

    QXlsx::Format strikePriceFormat;
    strikePriceFormat.setFontBold (true);
    strikePriceFormat.setFontSize (12);
    strikePriceFormat.setFontName ("Times New Roman");
    strikePriceFormat.setFontStrikeOut (true);
    strikePriceFormat.setHorizontalAlignment (QXlsx::Format::AlignHCenter);
    strikePriceFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    // Draw a diagonal slash across the cell to visually cross the old price when two prices are present
    strikePriceFormat.setDiagonalBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setDiagonalBorderType (QXlsx::Format::DiagonalBorderUp);

    QXlsx::Format signatureFormat;
    signatureFormat.setFontSize (8);
    signatureFormat.setFontName ("Times New Roman");
    signatureFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    signatureFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    signatureFormat.setTextWrap (true);

    QXlsx::Format supplierFormat;
    supplierFormat.setFontSize (7);
    supplierFormat.setFontName ("Times New Roman");
    supplierFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    supplierFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    supplierFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    supplierFormat.setTextWrap (true);

    QXlsx::Format addressFormat;
    addressFormat.setFontSize (7);
    addressFormat.setFontName ("Times New Roman");
    addressFormat.setHorizontalAlignment (QXlsx::Format::AlignLeft);
    addressFormat.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    addressFormat.setFontBold (true);
    addressFormat.setFontItalic (true);
    addressFormat.setTextWrap (true);


    for (int i = 1; i <= 2000; ++i)
        xlsx.setRowHeight (i, mmToRowHeightPt (4.0));

    // Helper to add thick outer borders for the tag rectangle edges only
    auto withOuterEdges = [] (const QXlsx::Format &base, bool left, bool right, bool top, bool bottom)
    {
        QXlsx::Format f = base;
        if (left)
            f.setLeftBorderStyle (QXlsx::Format::BorderMedium);
        if (right)
            f.setRightBorderStyle (QXlsx::Format::BorderMedium);
        if (top)
            f.setTopBorderStyle (QXlsx::Format::BorderMedium);
        if (bottom)
            f.setBottomBorderStyle (QXlsx::Format::BorderMedium);
        return f;
    };

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


            // Adjusted to yield visible widths in cm
            // Target: col1 ≈ 7.71 cm; col2 ≈ 3.57 cm; col3 ≈ 3.57 cm; col4 ≈ 2.71 cm
            xlsx.setColumnWidth (col + 0, 8.60);
            xlsx.setColumnWidth (col + 1, 4.46);
            xlsx.setColumnWidth (col + 2, 4.46);
            xlsx.setColumnWidth (col + 3, 3.43);


            const double rhPts[12] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 10.50, 13.50, 9.75, 9.75};
            for (int r = 0; r < tagRows; ++r)
                xlsx.setRowHeight (row + r, rhPts[r]);


            {
                const QXlsx::Format fmt = withOuterEdges (headerFormat, true, true, true, false);
                xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagCols - 1), fmt);
                xlsx.write (row, col, "ИП Новиков А.В.", fmt);
            }


            {
                const QXlsx::Format fmt = withOuterEdges (brandFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagCols - 1), fmt);
                xlsx.write (row + 1, col, tag.getBrand (), fmt);
            }


            {
                const QXlsx::Format fmt = withOuterEdges (categoryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagCols - 1), fmt);
                QString categoryText = tag.getCategory ();

                if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
                    categoryText += " " + tag.getGender ();
                xlsx.write (row + 2, col, categoryText, fmt);
            }


            {
                const QXlsx::Format fmt = withOuterEdges (brendCountryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagCols - 1), fmt);
                xlsx.write (row + 3, col, "Страна: " + tag.getBrandCountry (), fmt);
            }


            {
                const QXlsx::Format fmt = withOuterEdges (developCountryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagCols - 1), fmt);
                xlsx.write (row + 4, col, "Место: " + tag.getManufacturingPlace (), fmt);
            }


            {
                const QXlsx::Format fmtLeft	 = withOuterEdges (materialHeaderFormat, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (materialValueFormat, false, true, false, false);
                xlsx.write (row + 5, col, "Матер-л:", fmtLeft);
                xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagCols - 1), fmtRight);
                xlsx.write (row + 5, col + 1, tag.getMaterial (), fmtRight);
            }


            {
                const QXlsx::Format fmtLeft	 = withOuterEdges (articulHeaderFormat, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (articulValueFormat, false, true, false, false);
                xlsx.write (row + 6, col, "Артикул:", fmtLeft);
                xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagCols - 1), fmtRight);
                xlsx.write (row + 6, col + 1, tag.getArticle (), fmtRight);
            }


            if (tag.getPrice2 () > 0)
            {
                // For two prices: show only the first price number in the left cell (no label), with strikethrough and diagonal slash
                QString priceText			 = QString::number (tag.getPrice ());
                const QXlsx::Format fmtLeft	 = withOuterEdges (strikePriceFormat, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (priceFormatCell2, false, true, false, false);
                xlsx.write (row + 7, col, priceText, fmtLeft);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", fmtRight);
            }
            else
            {
                QString priceText			 = "Цена";
                const QXlsx::Format fmtLeft	 = withOuterEdges (priceFormatCell1, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (priceFormatCell2, false, true, false, false);
                xlsx.write (row + 7, col, priceText, fmtLeft);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", fmtRight);
            }


            {
                const QXlsx::Format fmt = withOuterEdges (signatureFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 8, col, row + 8, col + tagCols - 1), fmt);
                xlsx.write (row + 8, col, "Подпись", fmt);
            }


            {
                const QXlsx::Format fmtLeft	 = withOuterEdges (supplierFormat, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (supplierFormat, false, true, false, false);
                xlsx.write (row + 9, col, "Поставщик:", fmtLeft);
                xlsx.mergeCells (QXlsx::CellRange (row + 9, col + 1, row + 9, col + tagCols - 1), fmtRight);
                xlsx.write (row + 9, col + 1, tag.getSupplier (), fmtRight);
            }


            QString addressText = tag.getAddress ().simplified ();

			const int charBudgetPerLine = 40; // conservative budget per line for 7pt font across merged width
            QStringList words			= addressText.split (' ', Qt::SkipEmptyParts);
			QString line1, line2;
			int iWord = 0;
			while (iWord < words.size ())
			{
                const QString &w   = words[iWord];
				int prospectiveLen = (line1.isEmpty () ? 0 : line1.size () + 1) + w.size ();
				if (prospectiveLen <= charBudgetPerLine)
				{
					line1 = line1.isEmpty () ? w : line1 + " " + w;
					++iWord;
				}
                else
                    break;
			}
			while (iWord < words.size ())
			{
                const QString &w   = words[iWord];
				int prospectiveLen = (line2.isEmpty () ? 0 : line2.size () + 1) + w.size ();
				if (prospectiveLen <= charBudgetPerLine)
				{
					line2 = line2.isEmpty () ? w : line2 + " " + w;
					++iWord;
				}
                else
                    break;
			}


			{
				const QXlsx::Format fmt = withOuterEdges (addressFormat, true, true, false, false);

				xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagCols - 1), fmt);
				xlsx.write (row + 10, col, line1, fmt);
			}


			{
				const QXlsx::Format fmt = withOuterEdges (addressFormat, true, true, false, true);

				xlsx.mergeCells (QXlsx::CellRange (row + 11, col, row + 11, col + tagCols - 1), fmt);
				xlsx.write (row + 11, col, line2, fmt);
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
