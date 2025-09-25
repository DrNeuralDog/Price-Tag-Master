#include "excelgenerator.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <cmath>
#include <xlsxcellrange.h>
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxworksheet.h>
#include <xlsxrichstring.h>
#include "pricetag.h"


ExcelGenerator::ExcelGenerator (QObject *parent) : QObject (parent) {}

ExcelGenerator::~ExcelGenerator () {}


const double points = 2.834645669; // Excel row height is in points (1/72 inch). 1 mm = 2.83465 points


static double mmToExcelColumnWidth (double mm) { return mm / 2.4; }

static double mmToRowHeightPt (double mm) { return mm * points; }
// Extract readable label from template text (e.g. "Страна: ..." -> "Страна:")
static QString extractLabelFromTemplate (const QString &tmpl, const QString &fallback)
{
    // Preserve leading spaces exactly; detect label up to ':'
    const QString original = tmpl;
    const QString trimmed  = original.trimmed ();
    if (trimmed.isEmpty ())
        return fallback;
    int colon = original.indexOf (':');
    if (colon >= 0)
        return original.left (colon + 1);
    for (const QChar &ch : trimmed)
    {
        if (ch.isLetter ())
            return original;
    }
    return fallback;
}

// Excel may visually collapse leading ASCII spaces. Replace leading spaces with NBSP to preserve them.
static QString preserveLeadingSpacesExcel (const QString &s)
{
    int n = 0;
    while (n < s.size () && s.at (n) == QChar (' '))
        ++n;
    if (n == 0)
        return s;
    return QString (n, QChar (0x00A0)) + s.mid (n);
}

static int countLeadingSpacesGeneric (const QString &s)
{
    int n = 0;
    while (n < s.size ())
    {
        const QChar ch = s.at (n);
        if (ch == QChar (' ') || ch == QChar (0x00A0))
            ++n;
        else
            break;
    }
    return n;
}

static void writeWithInvisiblePad (QXlsx::Document &xlsx, int row, int col, const QXlsx::Format &cellFmt, const QString &text)
{
    const int lead = countLeadingSpacesGeneric (text);
    if (lead <= 0)
    {
        xlsx.write (row, col, text, cellFmt);
        return;
    }

    QXlsx::Format padFmt = cellFmt;
    padFmt.setFontColor (QColor (255, 255, 255)); // make leading '*' invisible on white background

    QXlsx::RichString rich;
    rich.addFragment (QString (lead, QChar ('*')), padFmt);
    rich.addFragment (text.mid (lead), cellFmt);

    xlsx.write (row, col, rich, cellFmt);
}

static QString replaceLeadingSpacesWithThin (const QString &s)
{
    int n = 0;
    while (n < s.size () && s.at (n) == QChar (' '))
        ++n;
    if (n == 0)
        return s;
    const QChar hair (0x200A); // hair space
    return QString (n, hair) + s.mid (n);
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
    tagRows = 11; // signature row removed

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


    const TagTextStyle stCompany  = tagTemplate.styleOrDefault (TagField::CompanyHeader);
    const TagTextStyle stBrand	  = tagTemplate.styleOrDefault (TagField::Brand);
    const TagTextStyle stCategory = tagTemplate.styleOrDefault (TagField::CategoryGender);
    const TagTextStyle stBrandC	  = tagTemplate.styleOrDefault (TagField::BrandCountry);
    const TagTextStyle stManuf	  = tagTemplate.styleOrDefault (TagField::ManufacturingPlace);
    const TagTextStyle stMatLab	  = tagTemplate.styleOrDefault (TagField::MaterialLabel);
    const TagTextStyle stMatVal	  = tagTemplate.styleOrDefault (TagField::MaterialValue);
    const TagTextStyle stArtLab	  = tagTemplate.styleOrDefault (TagField::ArticleLabel);
    const TagTextStyle stArtVal	  = tagTemplate.styleOrDefault (TagField::ArticleValue);
    const TagTextStyle stPriceL	  = tagTemplate.styleOrDefault (TagField::PriceLeft);
    const TagTextStyle stPriceR	  = tagTemplate.styleOrDefault (TagField::PriceRight);

    // Signature row removed per latest requirements
    const TagTextStyle stSupplierL = tagTemplate.styleOrDefault (TagField::SupplierLabel);
    const TagTextStyle stSupplierV = tagTemplate.styleOrDefault (TagField::SupplierValue);
    const TagTextStyle stAddress   = tagTemplate.styleOrDefault (TagField::Address);

    auto applyStyle = [] (QXlsx::Format &fmt, const TagTextStyle &st, QXlsx::Format::HorizontalAlignment halign)
    {
        fmt.setFontName (st.fontFamily);
        fmt.setFontSize (st.fontSizePt);
        fmt.setFontBold (st.bold);
        fmt.setFontItalic (st.italic);
        if (st.strike)
            fmt.setFontStrikeOut (true);
        fmt.setHorizontalAlignment (halign);
        fmt.setVerticalAlignment (QXlsx::Format::AlignVCenter);
        fmt.setTextWrap (true);
        if (st.align == TagTextAlign::Left)
            fmt.setIndent (0); // минимальный визуальный отступ (~0–0.5 мм)
    };

    auto halignFrom = [] (TagTextAlign a)
    {
        switch (a)
        {
            case TagTextAlign::Left:
                return QXlsx::Format::AlignLeft;
            case TagTextAlign::Center:
                return QXlsx::Format::AlignHCenter;
            case TagTextAlign::Right:
                return QXlsx::Format::AlignRight;
        }
        return QXlsx::Format::AlignLeft;
    };

    QXlsx::Format headerFormat;
    headerFormat.setFontBold (false);
    applyStyle (headerFormat, stCompany, halignFrom (stCompany.align));
    headerFormat.setBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format brandFormat;
    applyStyle (brandFormat, stBrand, halignFrom (stBrand.align));
    brandFormat.setBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format categoryFormat;
    applyStyle (categoryFormat, stCategory, halignFrom (stCategory.align));
    categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format brendCountryFormat;
    applyStyle (brendCountryFormat, stBrandC, halignFrom (stBrandC.align));
    brendCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format developCountryFormat;
    applyStyle (developCountryFormat, stManuf, halignFrom (stManuf.align));
    developCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    QXlsx::Format materialHeaderFormat;
    applyStyle (materialHeaderFormat, stMatLab, halignFrom (stMatLab.align));

    QXlsx::Format materialValueFormat;
    applyStyle (materialValueFormat, stMatVal, halignFrom (stMatVal.align));

    QXlsx::Format articulHeaderFormat;
    applyStyle (articulHeaderFormat, stArtLab, halignFrom (stArtLab.align));

    QXlsx::Format articulValueFormat;
    applyStyle (articulValueFormat, stArtVal, halignFrom (stArtVal.align));

    QXlsx::Format priceFormatCell1;
    applyStyle (priceFormatCell1, stPriceL, halignFrom (stPriceL.align));


    QXlsx::Format priceFormatCell2;
    applyStyle (priceFormatCell2, stPriceR, halignFrom (stPriceR.align));
    priceFormatCell2.setBorderStyle (QXlsx::Format::BorderThin); // внутренние границы тонкие; внешние усилим отдельно

    QXlsx::Format strikePriceFormat;
    applyStyle (strikePriceFormat, stPriceL, halignFrom (stPriceL.align));
    strikePriceFormat.setFontStrikeOut (true);

    // Draw a diagonal slash across the cell to visually cross the old price when two prices are present
    strikePriceFormat.setDiagonalBorderStyle (QXlsx::Format::BorderThin);
    strikePriceFormat.setDiagonalBorderType (QXlsx::Format::DiagonalBorderUp);

    // (no signatureFormat)

    QXlsx::Format supplierFormat;
    applyStyle (supplierFormat, stSupplierL, halignFrom (stSupplierL.align));
    supplierFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    supplierFormat.setTextWrap (true);

    QXlsx::Format addressFormat;
    applyStyle (addressFormat, stAddress, halignFrom (stAddress.align));
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
            // Place within page grid (no horizontal page offset). We stack pages vertically.
            placeTagCellRange (layoutConfig, gridCol, gridRow, originCol, originRow, col, row, tagCols, tagRows);

            // Add separation and vertical paging: one blank Excel row between pages
            // Compute vertical offset in raw Excel rows: full page height in rows + 1 spacer row
            const int pageGapRows = 1; // one blank row between pages
            row += pageIdx * (nRows * tagRows + pageGapRows);

            qDebug () << "Creating price tag" << tagIndex << "at position (" << row << "," << col << ")";


            // Scale inner columns to match current tag width from template
            {
                const double colMm[4] = {77.1, 35.7, 35.7, 27.1};
                const double sumMm = colMm[0] + colMm[1] + colMm[2] + colMm[3];
                const double targetMm = layoutConfig.tagWidthMm;
                const double k = (sumMm > 0.0) ? (targetMm / sumMm) : 1.0;
                const double w0 = mmToExcelColumnWidth (colMm[0] * k);
                const double w1 = mmToExcelColumnWidth (colMm[1] * k);
                const double w2 = mmToExcelColumnWidth (colMm[2] * k);
                const double w3 = mmToExcelColumnWidth (colMm[3] * k);
                xlsx.setColumnWidth (col + 0, w0);
                xlsx.setColumnWidth (col + 1, w1);
                xlsx.setColumnWidth (col + 2, w2);
                xlsx.setColumnWidth (col + 3, w3);
            }


            // 11 rows now (removed empty signature row between price and supplier)
            const double rhPts[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};
            for (int r = 0; r < tagRows; ++r)
                xlsx.setRowHeight (row + r, rhPts[r]);


            {
                QXlsx::Format fmt = withOuterEdges (headerFormat, true, true, true, false);
                const QString txt = tagTemplate.textOrDefault (TagField::CompanyHeader);
                xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagCols - 1), fmt);
                writeWithInvisiblePad (xlsx, row, col, fmt, txt);
            }


            {
                QXlsx::Format fmt = withOuterEdges (brandFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagCols - 1), fmt);
                const QString txt = tag.getBrand ();
                const int lead	  = countLeadingSpacesGeneric (txt);
                if (lead > 0) fmt.setIndent (qMin (15, lead));
                writeWithInvisiblePad (xlsx, row + 1, col, fmt, txt.mid (lead));
            }


            {
                QXlsx::Format fmt = withOuterEdges (categoryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagCols - 1), fmt);
                QString categoryText = tag.getCategory ();

                bool appendedGender = false;
                if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
                {
                    categoryText += " " + tag.getGender ();
                    appendedGender = true;
                }
                if (appendedGender && ! tag.getSize ().isEmpty ())
                    categoryText += " " + tag.getSize ();
                const int lead = countLeadingSpacesGeneric (categoryText);
                if (lead > 0) fmt.setIndent (qMin (15, lead));
                writeWithInvisiblePad (xlsx, row + 2, col, fmt, categoryText.mid (lead));
            }


            {
                QXlsx::Format fmt = withOuterEdges (brendCountryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagCols - 1), fmt);
                const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::BrandCountry), QString::fromUtf8 ("Страна:"));
                writeWithInvisiblePad (xlsx, row + 3, col, fmt, labelRaw + " " + tag.getBrandCountry ());
            }


            {
                QXlsx::Format fmt = withOuterEdges (developCountryFormat, true, true, false, false);
                xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagCols - 1), fmt);
                const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ManufacturingPlace), QString::fromUtf8 ("Место:"));
                writeWithInvisiblePad (xlsx, row + 4, col, fmt, labelRaw + " " + tag.getManufacturingPlace ());
            }


            {
                QXlsx::Format fmtLeft	 = withOuterEdges (materialHeaderFormat, true, false, false, false);
                QXlsx::Format fmtRight = withOuterEdges (materialValueFormat, false, true, false, false);
                const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::MaterialLabel), QString::fromUtf8 ("Матер-л:"));
                writeWithInvisiblePad (xlsx, row + 5, col, fmtLeft, labelRaw);
                xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagCols - 1), fmtRight);
                const QString val = tag.getMaterial ();
                const int valLeadMat = countLeadingSpacesGeneric (val);
                if (valLeadMat > 0) fmtRight.setIndent (qMin (15, valLeadMat));
                writeWithInvisiblePad (xlsx, row + 5, col + 1, fmtRight, val.mid (valLeadMat));
            }


            {
                QXlsx::Format fmtLeft	 = withOuterEdges (articulHeaderFormat, true, false, false, false);
                QXlsx::Format fmtRight = withOuterEdges (articulValueFormat, false, true, false, false);
                const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ArticleLabel), QString::fromUtf8 ("Артикул:"));
                writeWithInvisiblePad (xlsx, row + 6, col, fmtLeft, labelRaw);
                xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagCols - 1), fmtRight);
                const QString val = tag.getArticle ();
                const int valLeadArt = countLeadingSpacesGeneric (val);
                if (valLeadArt > 0) fmtRight.setIndent (qMin (15, valLeadArt));
                writeWithInvisiblePad (xlsx, row + 6, col + 1, fmtRight, val.mid (valLeadArt));
            }


            if (tag.getPrice2 () > 0)
            {
                // For two prices: show only the first price number in the left cell (no label), with strikethrough and diagonal slash
                QString priceText			 = QString::number (tag.getPrice ());
                QXlsx::Format fmtLeft	 = withOuterEdges (strikePriceFormat, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (priceFormatCell2, false, true, false, false);
                const int lead = countLeadingSpacesGeneric (priceText);
                if (lead > 0) fmtLeft.setIndent (qMin (15, lead));
                xlsx.write (row + 7, col, priceText.mid (lead), fmtLeft);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", fmtRight);
            }
            else
            {
                const QString priceTextRaw = tagTemplate.textOrDefault (TagField::PriceLeft);
                QXlsx::Format fmtLeft = withOuterEdges (priceFormatCell1, true, false, false, false);
                const QXlsx::Format fmtRight = withOuterEdges (priceFormatCell2, false, true, false, false);
                writeWithInvisiblePad (xlsx, row + 7, col, fmtLeft, priceTextRaw);

                xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
                xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", fmtRight);
            }

            // Supplier row moves up by one (was row+9)
            {
                QXlsx::Format fmtLeft	 = withOuterEdges (supplierFormat, true, false, false, false);
                QXlsx::Format fmtRight = withOuterEdges (supplierFormat, false, true, false, false);
                const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::SupplierLabel), QString::fromUtf8 ("Поставщик:"));
                writeWithInvisiblePad (xlsx, row + 8, col, fmtLeft, labelRaw);
                xlsx.mergeCells (QXlsx::CellRange (row + 8, col + 1, row + 8, col + tagCols - 1), fmtRight);
                const QString val = tag.getSupplier ();
                const int valLeadSup = countLeadingSpacesGeneric (val);
                if (valLeadSup > 0) fmtRight.setIndent (qMin (15, valLeadSup));
                writeWithInvisiblePad (xlsx, row + 8, col + 1, fmtRight, val.mid (valLeadSup));
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
                {
                    if (line1.isEmpty ())
                    {
                        // If a single word exceeds the budget, place it on the line anyway
                        line1 = w;
                        ++iWord;
                    }
                    break;
                }
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
                {
                    if (line2.isEmpty ())
                    {
                        // If a single word exceeds the budget, place it on the second line anyway
                        line2 = w;
                        ++iWord;
                    }
                    break;
                }
			}


			{
				QXlsx::Format fmt = withOuterEdges (addressFormat, true, true, false, false);

                xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagCols - 1), fmt);
                const int lead = countLeadingSpacesGeneric (line1);
                if (lead > 0) fmt.setIndent (qMin (15, lead));
                writeWithInvisiblePad (xlsx, row + 9, col, fmt, line1.mid (lead));
			}


			{
				QXlsx::Format fmt = withOuterEdges (addressFormat, true, true, false, true);

                xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagCols - 1), fmt);
                const int lead = countLeadingSpacesGeneric (line2);
                if (lead > 0) fmt.setIndent (qMin (15, lead));
                writeWithInvisiblePad (xlsx, row + 10, col, fmt, line2.mid (lead));
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

    bool result = xlsx.saveAs (outputPath);

    qDebug () << "Saving Excel document to:" << outputPath;
    qDebug () << "Save result:" << result;


    return result;
}
