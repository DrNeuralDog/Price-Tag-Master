#include "excelgenerator.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <cmath>
#include <xlsxcellrange.h>
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxrichstring.h>
#include <xlsxworksheet.h>
#include "pricetag.h"


ExcelGenerator::ExcelGenerator (QObject *parent) : QObject (parent) {}

ExcelGenerator::~ExcelGenerator () {}


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
        if (ch.isLetter ())
            return original;


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


// A small hack for correct formatting in Excel – indents often work incorrectly there,
// and this hack with inserting invisible * characters
// allows you to guarantee the expected behavior in different versions
static void writeWithInvisiblePad (QXlsx::Document &xlsx, int row, int col, const QXlsx::Format &cellFmt, const QString &text)
{
    const int lead = countLeadingSpacesGeneric (text);

    if (lead <= 0)
    {
        xlsx.write (row, col, text, cellFmt);

        return;
    }

    QXlsx::Format padFmt = cellFmt;

    // make leading '*' invisible on white background
    padFmt.setFontColor (QColor (255, 255, 255));

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

    // hair space
    const QChar hair (0x200A);


    return QString (n, hair) + s.mid (n);
}


// Compute how many tags fit per page taking into account the ACTUAL Excel row heights used
static void computeGrid (const ExcelGenerator::ExcelLayoutConfig &cfg, int &nCols, int &nRows, double &pageUsedMm)
{
    // Excel default print margins (approx):
    // Top/Bottom header/footer: 0.3 in = 7.62 mm; Left/Right non-printable ~1% of width
    const double excelLeftMm  = pageA4WidthMm * .01;
    const double excelRightMm = pageA4WidthMm * .01;

    // Use the larger of our logical side margins and Excel defaults to avoid optimistic overfill
    const double effLeftMm	= std::max (cfg.marginLeftMm, excelLeftMm);
    const double effRightMm = std::max (cfg.marginRightMm, excelRightMm);

    // Top/bottom: user margins PLUS Excel header/footer zones (constant 7.62 mm each)
    const double headerMm	 = 7.62;
    const double footerMm	 = 7.62;
    const double effTopMm	 = cfg.marginTopMm + headerMm;
    const double effBottomMm = cfg.marginBottomMm + footerMm;

    // Available drawing width; height also subtracts a small origin blank row (~4 mm)
    const double availW			  = pageA4WidthMm - effLeftMm - effRightMm;
    const double originTopBlankMm = 4.0;
    const double pageH			  = pageA4HeightMm - effTopMm - effBottomMm - originTopBlankMm;

    const double effectiveHorizSpacingMm = 0.0;

    nCols = std::max (1, static_cast<int> (std::floor ((availW + effectiveHorizSpacingMm) / (cfg.tagWidthMm + effectiveHorizSpacingMm))));

    // Vertical sizing: prefer template-driven tag height (cfg.tagHeightMm); fallback to base sum of row heights
    const double rhPts[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};
    double baseTagPt	   = 0.0;

    for (double v : rhPts)
        baseTagPt += v;

    const double baseTagMm		   = baseTagPt / points;
    const double tagHeightMmActual = (cfg.tagHeightMm > 0.0) ? cfg.tagHeightMm : baseTagMm;

    const double effectiveVertSpacingMm = 0.0;
    const double minGapMm				= 1.0; // keep minimal 1mm gap per page

    const double denom	   = tagHeightMmActual + effectiveVertSpacingMm;
    const double numerator = std::max (0.0, pageH - minGapMm) + effectiveVertSpacingMm;


    nRows = (denom > 0.0) ? static_cast<int> (std::floor (numerator / denom)) : 1;
    nRows = std::max (1, nRows);

    pageUsedMm = nRows * tagHeightMmActual;


    qDebug () << "T tagHeightMmActual: " << tagHeightMmActual;
    qDebug () << "T pageH: " << pageH;
    qDebug () << "T denom: " << denom;
    qDebug () << "T numerator: " << numerator;
    qDebug () << "T nRows: " << nRows;
}

// Compute printable height per page in millimeters (align with computeGrid logic)
static double printableHeightMm (const ExcelGenerator::ExcelLayoutConfig &cfg)
{
    // Header/Footer constants (7.62 mm each) added to user top/bottom margins
    const double headerMm	 = 7.62;
    const double footerMm	 = 7.62;
    const double effTopMm	 = cfg.marginTopMm + headerMm;
    const double effBottomMm = cfg.marginBottomMm + footerMm;

    const double originTopBlankMm = 4.0;


    return std::max (0.0, pageA4HeightMm - effTopMm - effBottomMm - originTopBlankMm);
}

// Draw a dashed rectangle around a logical page area to visualize page boundaries
// (Removed dashed page outline drawing to revert behavior)

// Convert mm position to starting Excel row/col offsets given a base origin (row0/col0)
static void placeTagCellRange (const ExcelGenerator::ExcelLayoutConfig &cfg, int gridCol, int gridRow, int originCol, int originRow,
                               int &startCol, int &startRow, int &tagCols, int &tagRows)
{
    tagCols = 4;
    tagRows = 11; // signature row removed

    startCol = originCol + gridCol * tagCols;
    startRow = originRow + gridRow * tagRows;
}

// Helper: add thick outer borders for the tag rectangle edges only
static QXlsx::Format withOuterEdges (const QXlsx::Format &base, bool left, bool right, bool top, bool bottom)
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
}

// Convert TagTextAlign to QXlsx alignment
static QXlsx::Format::HorizontalAlignment toQXlsxHAlign (TagTextAlign a)
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
}

// Apply TagTextStyle into format
static void applyTextStyle (QXlsx::Format &fmt, const TagTextStyle &st)
{
    fmt.setFontName (st.fontFamily);
    fmt.setFontSize (st.fontSizePt);
    fmt.setFontBold (st.bold);
    fmt.setFontItalic (st.italic);

    if (st.strike)
        fmt.setFontStrikeOut (true);

    fmt.setHorizontalAlignment (toQXlsxHAlign (st.align));
    fmt.setVerticalAlignment (QXlsx::Format::AlignVCenter);
    fmt.setTextWrap (true);

    if (st.align == TagTextAlign::Left)
        fmt.setIndent (0);
}

// Formats bundle
struct TagFormats
{
    QXlsx::Format headerFormat;
    QXlsx::Format brandFormat;
    QXlsx::Format categoryFormat;
    QXlsx::Format brendCountryFormat;
    QXlsx::Format developCountryFormat;
    QXlsx::Format materialHeaderFormat;
    QXlsx::Format materialValueFormat;
    QXlsx::Format articulHeaderFormat;
    QXlsx::Format articulValueFormat;
    QXlsx::Format priceFormatCell1;
    QXlsx::Format priceFormatCell2;
    QXlsx::Format strikePriceFormat;
    QXlsx::Format supplierFormat;
    QXlsx::Format addressFormat;
};

static TagFormats createTagFormats (const TagTemplate &tagTemplate)
{
    TagFormats tf;

    const TagTextStyle stCompany   = tagTemplate.styleOrDefault (TagField::CompanyHeader);
    const TagTextStyle stBrand	   = tagTemplate.styleOrDefault (TagField::Brand);
    const TagTextStyle stCategory  = tagTemplate.styleOrDefault (TagField::CategoryGender);
    const TagTextStyle stBrandC	   = tagTemplate.styleOrDefault (TagField::BrandCountry);
    const TagTextStyle stManuf	   = tagTemplate.styleOrDefault (TagField::ManufacturingPlace);
    const TagTextStyle stMatLab	   = tagTemplate.styleOrDefault (TagField::MaterialLabel);
    const TagTextStyle stMatVal	   = tagTemplate.styleOrDefault (TagField::MaterialValue);
    const TagTextStyle stArtLab	   = tagTemplate.styleOrDefault (TagField::ArticleLabel);
    const TagTextStyle stArtVal	   = tagTemplate.styleOrDefault (TagField::ArticleValue);
    const TagTextStyle stPriceL	   = tagTemplate.styleOrDefault (TagField::PriceLeft);
    const TagTextStyle stPriceR	   = tagTemplate.styleOrDefault (TagField::PriceRight);
    const TagTextStyle stSupplierL = tagTemplate.styleOrDefault (TagField::SupplierLabel);
    const TagTextStyle stAddress   = tagTemplate.styleOrDefault (TagField::Address);


    tf.headerFormat.setFontBold (false);
    applyTextStyle (tf.headerFormat, stCompany);
    tf.headerFormat.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.brandFormat, stBrand);
    tf.brandFormat.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.categoryFormat, stCategory);
    tf.categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.brendCountryFormat, stBrandC);
    tf.brendCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.developCountryFormat, stManuf);
    tf.developCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.materialHeaderFormat, stMatLab);
    applyTextStyle (tf.materialValueFormat, stMatVal);

    applyTextStyle (tf.articulHeaderFormat, stArtLab);
    applyTextStyle (tf.articulValueFormat, stArtVal);

    applyTextStyle (tf.priceFormatCell1, stPriceL);
    applyTextStyle (tf.priceFormatCell2, stPriceR);
    tf.priceFormatCell2.setBorderStyle (QXlsx::Format::BorderThin);

    applyTextStyle (tf.strikePriceFormat, stPriceL);
    tf.strikePriceFormat.setFontStrikeOut (true);
    tf.strikePriceFormat.setDiagonalBorderStyle (QXlsx::Format::BorderThin);
    tf.strikePriceFormat.setDiagonalBorderType (QXlsx::Format::DiagonalBorderUp);

    applyTextStyle (tf.supplierFormat, stSupplierL);
    tf.supplierFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
    tf.supplierFormat.setTextWrap (true);

    applyTextStyle (tf.addressFormat, stAddress);
    tf.addressFormat.setTextWrap (true);


    return tf;
}

static void setupDefaultRowHeights (QXlsx::Document &xlsx)
{
    for (int i = 1; i <= 2000; ++i)
        xlsx.setRowHeight (i, mmToRowHeightPt (4.0));
}

static void configureColumnsForTagWidth (QXlsx::Document &xlsx, int col, const ExcelGenerator::ExcelLayoutConfig &layoutConfig)
{
    const double colMm[4] = {77.1, 35.7, 35.7, 27.1};
    const double sumMm	  = colMm[0] + colMm[1] + colMm[2] + colMm[3];
    const double targetMm = layoutConfig.tagWidthMm;
    const double k		  = (sumMm > 0.0) ? (targetMm / sumMm) : 1.0;

    const double w0 = mmToExcelColumnWidth (colMm[0] * k);
    const double w1 = mmToExcelColumnWidth (colMm[1] * k);
    const double w2 = mmToExcelColumnWidth (colMm[2] * k);
    const double w3 = mmToExcelColumnWidth (colMm[3] * k);

    xlsx.setColumnWidth (col + 0, w0);
    xlsx.setColumnWidth (col + 1, w1);
    xlsx.setColumnWidth (col + 2, w2);
    xlsx.setColumnWidth (col + 3, w3);
}

static void setTagRowHeights (QXlsx::Document &xlsx, int row, int tagRows, const ExcelGenerator::ExcelLayoutConfig &layoutConfig)
{
    const double rhPts[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};
    // Scale row heights to match desired tag height from template editor
    double basePt = 0.0;

    for (int i = 0; i < tagRows; ++i)
        basePt += rhPts[i];

    const double baseMm	   = basePt / points; // points -> mm
    const double desiredMm = layoutConfig.tagHeightMm > 0.0 ? layoutConfig.tagHeightMm : baseMm;
    const double k		   = (baseMm > 0.0) ? (desiredMm / baseMm) : 1.0;

    for (int r = 0; r < tagRows; ++r)
        xlsx.setRowHeight (row + r, rhPts[r] * k);
}

static void writeCompanyHeaderRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const TagTemplate &tagTemplate,
                                   const TagFormats &tf)
{
    QXlsx::Format fmt = withOuterEdges (tf.headerFormat, true, true, true, false);
    const QString txt = tagTemplate.textOrDefault (TagField::CompanyHeader);

    xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagCols - 1), fmt);

    writeWithInvisiblePad (xlsx, row, col, fmt, txt);
}

static void writeBrandRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf)
{
    QXlsx::Format fmt = withOuterEdges (tf.brandFormat, true, true, false, false);
    xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagCols - 1), fmt);

    const QString txt = tag.getBrand ();
    const int lead	  = countLeadingSpacesGeneric (txt);

    if (lead > 0)
        fmt.setIndent (qMin (15, lead));

    writeWithInvisiblePad (xlsx, row + 1, col, fmt, txt.mid (lead));
}

static void writeCategoryRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf)
{
    QXlsx::Format fmt = withOuterEdges (tf.categoryFormat, true, true, false, false);
    xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagCols - 1), fmt);
    QString categoryText = tag.getCategory ();
    bool appendedGender	 = false;

    if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
    {
        categoryText += " " + tag.getGender ();

        appendedGender = true;
    }

    if (appendedGender && ! tag.getSize ().isEmpty ())
        categoryText += " " + tag.getSize ();

    const int lead = countLeadingSpacesGeneric (categoryText);

    if (lead > 0)
        fmt.setIndent (qMin (15, lead));

    writeWithInvisiblePad (xlsx, row + 2, col, fmt, categoryText.mid (lead));
}

static void writeBrandCountryRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                                  const TagFormats &tf)
{
    QXlsx::Format fmt = withOuterEdges (tf.brendCountryFormat, true, true, false, false);
    xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagCols - 1), fmt);

    const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::BrandCountry), QString::fromUtf8 ("Страна:"));

    writeWithInvisiblePad (xlsx, row + 3, col, fmt, labelRaw + " " + tag.getBrandCountry ());
}

static void writeManufacturingPlaceRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag,
                                        const TagTemplate &tagTemplate, const TagFormats &tf)
{
    QXlsx::Format fmt = withOuterEdges (tf.developCountryFormat, true, true, false, false);
    xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagCols - 1), fmt);

    const QString labelRaw =
            extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ManufacturingPlace), QString::fromUtf8 ("Место:"));

    writeWithInvisiblePad (xlsx, row + 4, col, fmt, labelRaw + " " + tag.getManufacturingPlace ());
}

static void writeMaterialRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                              const TagFormats &tf)
{
    QXlsx::Format fmtLeft  = withOuterEdges (tf.materialHeaderFormat, true, false, false, false);
    QXlsx::Format fmtRight = withOuterEdges (tf.materialValueFormat, false, true, false, false);
    const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::MaterialLabel), QString::fromUtf8 ("Матер-л:"));

    writeWithInvisiblePad (xlsx, row + 5, col, fmtLeft, labelRaw);
    xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagCols - 1), fmtRight);

    const QString val	 = tag.getMaterial ();
    const int valLeadMat = countLeadingSpacesGeneric (val);

    if (valLeadMat > 0)
        fmtRight.setIndent (qMin (15, valLeadMat));

    writeWithInvisiblePad (xlsx, row + 5, col + 1, fmtRight, val.mid (valLeadMat));
}

static void writeArticleRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                             const TagFormats &tf)
{
    QXlsx::Format fmtLeft  = withOuterEdges (tf.articulHeaderFormat, true, false, false, false);
    QXlsx::Format fmtRight = withOuterEdges (tf.articulValueFormat, false, true, false, false);
    const QString labelRaw = extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ArticleLabel), QString::fromUtf8 ("Артикул:"));

    writeWithInvisiblePad (xlsx, row + 6, col, fmtLeft, labelRaw);
    xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagCols - 1), fmtRight);

    const QString val	 = tag.getArticle ();
    const int valLeadArt = countLeadingSpacesGeneric (val);

    if (valLeadArt > 0)
        fmtRight.setIndent (qMin (15, valLeadArt));

    writeWithInvisiblePad (xlsx, row + 6, col + 1, fmtRight, val.mid (valLeadArt));
}

static void writePriceRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                           const TagFormats &tf)
{
    if (tag.getPrice2 () > 0)
    {
        QString priceText			 = QString::number (tag.getPrice ());
        QXlsx::Format fmtLeft		 = withOuterEdges (tf.strikePriceFormat, true, false, false, false);
        const QXlsx::Format fmtRight = withOuterEdges (tf.priceFormatCell2, false, true, false, false);
        const int lead				 = countLeadingSpacesGeneric (priceText);

        if (lead > 0)
            fmtLeft.setIndent (qMin (15, lead));

        xlsx.write (row + 7, col, priceText.mid (lead), fmtLeft);
        xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
        xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", fmtRight);
    }
    else
    {
        QXlsx::Format fmtLeft		 = withOuterEdges (tf.priceFormatCell1, true, false, false, false);
        const QXlsx::Format fmtRight = withOuterEdges (tf.priceFormatCell2, false, true, false, false);

        // Force left alignment for default label
        fmtLeft.setHorizontalAlignment (QXlsx::Format::AlignLeft);
        fmtLeft.setIndent (0);

        writeWithInvisiblePad (xlsx, row + 7, col, fmtLeft, QString::fromUtf8 ("Цена: "));

        xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
        xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", fmtRight);
    }
}

static void writeSupplierRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                              const TagFormats &tf)
{
    QXlsx::Format fmtLeft  = withOuterEdges (tf.supplierFormat, true, false, false, false);
    QXlsx::Format fmtRight = withOuterEdges (tf.supplierFormat, false, true, false, false);
    const QString labelRaw =
            extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::SupplierLabel), QString::fromUtf8 ("Поставщик:"));

    writeWithInvisiblePad (xlsx, row + 8, col, fmtLeft, labelRaw);
    xlsx.mergeCells (QXlsx::CellRange (row + 8, col + 1, row + 8, col + tagCols - 1), fmtRight);

    const QString val	 = tag.getSupplier ();
    const int valLeadSup = countLeadingSpacesGeneric (val);

    if (valLeadSup > 0)
        fmtRight.setIndent (qMin (15, valLeadSup));

    writeWithInvisiblePad (xlsx, row + 8, col + 1, fmtRight, val.mid (valLeadSup));
}

static std::pair<QString, QString> splitAddressTwoLines (const QString &address)
{
    const int charBudgetPerLine = 40;
    QStringList words			= address.simplified ().split (' ', Qt::SkipEmptyParts);
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
                line2 = w;

                ++iWord;
            }


            break;
        }
    }


    return {line1, line2};
}

static void writeAddressRows (QXlsx::Document &xlsx, int row, int col, int tagCols, const QString &line1, const QString &line2,
                              const TagFormats &tf)
{
    {
        QXlsx::Format fmt = withOuterEdges (tf.addressFormat, true, true, false, false);
        xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagCols - 1), fmt);

        const int lead = countLeadingSpacesGeneric (line1);

        if (lead > 0)
            fmt.setIndent (qMin (15, lead));

        writeWithInvisiblePad (xlsx, row + 9, col, fmt, line1.mid (lead));
    }


    {
        QXlsx::Format fmt = withOuterEdges (tf.addressFormat, true, true, false, true);
        xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagCols - 1), fmt);
        const int lead = countLeadingSpacesGeneric (line2);

        if (lead > 0)
            fmt.setIndent (qMin (15, lead));

        writeWithInvisiblePad (xlsx, row + 10, col, fmt, line2.mid (lead));
    }
}

static void renderTag (QXlsx::Document &xlsx, int row, int col, int tagCols, int tagRows, const PriceTag &tag,
                       const TagTemplate &tagTemplate, const ExcelGenerator::ExcelLayoutConfig &layoutConfig, const TagFormats &tf)
{
    Q_UNUSED (layoutConfig);


    configureColumnsForTagWidth (xlsx, col, layoutConfig);

    setTagRowHeights (xlsx, row, tagRows, layoutConfig);

    writeCompanyHeaderRow (xlsx, row, col, tagCols, tagTemplate, tf);
    writeBrandRow (xlsx, row, col, tagCols, tag, tf);
    writeCategoryRow (xlsx, row, col, tagCols, tag, tf);
    writeBrandCountryRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
    writeManufacturingPlaceRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
    writeMaterialRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
    writeArticleRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
    writePriceRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
    writeSupplierRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);

    const auto pair = splitAddressTwoLines (tag.getAddress ());
    writeAddressRows (xlsx, row, col, tagCols, pair.first, pair.second, tf);
}

bool ExcelGenerator::generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    qDebug () << "Generating Excel document with" << priceTags.size () << "price tags";

    QXlsx::Document xlsx;

    // Refactored orchestration (no logic change)
    int nCols = 1, nRows = 1;
    double pageUsedMm = 0.0;
    computeGrid (layoutConfig, nCols, nRows, pageUsedMm);

    const int originCol = 2;
    const int originRow = 2;

    const double spacerWcol = mmToExcelColumnWidth (layoutConfig.spacingHMm);
    const double spacerHrow = mmToRowHeightPt (layoutConfig.spacingVMm);

    Q_UNUSED (spacerWcol);
    Q_UNUSED (spacerHrow);

    const TagFormats tf = createTagFormats (tagTemplate);
    setupDefaultRowHeights (xlsx);


    int tagIndex = 0;

    const int rowsPerPage = std::max (1, nRows);
    for (const PriceTag &tag : priceTags)
    {
        const int perPage = nCols * rowsPerPage;
        qDebug () << "perPage: " << perPage;

        for (int q = 0; q < tag.getQuantity (); ++q)
        {
            const int pageIdx	= tagIndex / perPage;
            const int idxInPage = tagIndex % perPage;
            const int gridRow	= idxInPage / nCols; // 0..rowsPerPage-1
            const int gridCol	= idxInPage % nCols;

            int col = 0, row = 0, tagCols = 0, tagRows = 0;
            placeTagCellRange (layoutConfig, gridCol, gridRow, originCol, originRow, col, row, tagCols, tagRows);

            const int pageGapRows = 1; // base one-row gap between pages
            row += pageIdx * (rowsPerPage * tagRows + pageGapRows);


            qDebug () << "idxInPage: " << idxInPage;

            // After the last tag on a page, add one visible blank row (default ~4mm)
            // so there is a clear one-row gap before the next page block
            if (idxInPage == perPage - 1)
            {
                // Dynamic gap height: fill remaining printable area with a small safety pad
                const double pageH		 = printableHeightMm (layoutConfig);
                const double safetyPadMm = 6.5; // tiny pad to counter print rounding
                double gapMm			 = pageH - pageUsedMm + safetyPadMm;

                if (gapMm < 2.)
                    gapMm = 6.5; // ensure at least a thin visible gap

                xlsx.setRowHeight (row + tagRows, mmToRowHeightPt (gapMm));
            }

            qDebug () << "Creating price tag" << tagIndex << "at position (" << row << "," << col << ")";

            renderTag (xlsx, row, col, tagCols, tagRows, tag, tagTemplate, layoutConfig, tf);

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
