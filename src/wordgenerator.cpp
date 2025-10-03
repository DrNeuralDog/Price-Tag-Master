#include "wordgenerator.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <algorithm>
#include <cmath>


using namespace QXlsx;


WordGenerator::WordGenerator (QObject *parent) : QObject (parent) {}

WordGenerator::~WordGenerator () {}


static QList<PriceTag> expandByQuantity (const QList<PriceTag> &src)
{
    QList<PriceTag> out;

    for (const PriceTag &t : src)
    {
        const int q = std::max (1, t.getQuantity ());

        for (int i = 0; i < q; ++i)
            out.append (t);
    }


    return out;
}

void WordGenerator::computeGrid (const DocxLayoutConfig &cfg, int &nCols, int &nRows)
{
    const double pageW	= 210.0;
    const double pageH	= 297.0;
    const double availW = pageW - cfg.marginLeftMm - cfg.marginRightMm;
    const double availH = pageH - cfg.marginTopMm - cfg.marginBottomMm;

    nCols = std::max (1, static_cast<int> (std::floor ((availW + cfg.spacingHMm) / (cfg.tagWidthMm + cfg.spacingHMm))));
    nRows = std::max (1, static_cast<int> (std::floor ((availH + cfg.spacingVMm) / (cfg.tagHeightMm + cfg.spacingVMm))));
}

bool WordGenerator::generateWordDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    if (priceTags.isEmpty ())
    {
        qDebug () << "No price tags to generate";

        return false;
    }

    const QList<PriceTag> expanded = expandByQuantity (priceTags);


    ZipWriter zip (outputPath);
    if (zip.error ())
    {
        qDebug () << "Failed to open DOCX for writing:" << outputPath;

        return false;
    }

    writeContentTypes (zip);
    writeRelsRoot (zip);
    writeDocProps (zip);
    writeStyles (zip);
    writeSettings (zip);
    writeDocumentXml (zip, expanded);

    zip.close ();


    return true;
}

void WordGenerator::writeContentTypes (ZipWriter &zip)
{
    const char *xml =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
            "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
            "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
            "<Override PartName=\"/word/document.xml\" "
            "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
            "<Override PartName=\"/word/styles.xml\" "
            "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml\"/>"
            "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
            "<Override PartName=\"/docProps/app.xml\" "
            "ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"
            "<Override PartName=\"/word/settings.xml\" "
            "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml\"/>"
            "</Types>";

    zip.addFile ("[Content_Types].xml", QByteArray (xml));
}

void WordGenerator::writeRelsRoot (ZipWriter &zip)
{
    const char *rels =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" "
            "Target=\"word/document.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" "
            "Target=\"docProps/core.xml\"/>"
            "<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" "
            "Target=\"docProps/app.xml\"/>"
            "</Relationships>";

    zip.addFile ("_rels/.rels", QByteArray (rels));


    const char *docRels = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                          "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
                          "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" "
                          "Target=\"styles.xml\"/>"
                          "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings\" "
                          "Target=\"settings.xml\"/>"
                          "</Relationships>";

    zip.addFile ("word/_rels/document.xml.rels", QByteArray (docRels));
}

void WordGenerator::writeDocProps (ZipWriter &zip)
{
    const QString core = QString ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                                  "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
                                  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" "
                                  "xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
                                  "<dc:title>Price Tags</dc:title>"
                                  "<dc:creator>PriceTagMaster</dc:creator>"
                                  "<cp:lastModifiedBy>PriceTagMaster</cp:lastModifiedBy>"
                                  "<dcterms:created xsi:type=\"dcterms:W3CDTF\">%1</dcterms:created>"
                                  "<dcterms:modified xsi:type=\"dcterms:W3CDTF\">%1</dcterms:modified>"
                                  "</cp:coreProperties>")
                                 .arg (QDateTime::currentDateTimeUtc ().toString (Qt::ISODate));

    zip.addFile ("docProps/core.xml", core.toUtf8 ());


    const char *app = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                      "<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" "
                      "xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">"
                      "<Application>PriceTagMaster</Application>"
                      "</Properties>";

    zip.addFile ("docProps/app.xml", QByteArray (app));
}

void WordGenerator::writeStyles (ZipWriter &zip)
{
    const char *styles = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                         "<w:styles xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                         "<w:style w:type=\"paragraph\" w:default=\"1\" w:styleId=\"Normal\">"
                         "  <w:name w:val=\"Normal\"/>"
                         "  <w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"22\"/></w:rPr>"
                         "</w:style>"
                         "</w:styles>";

    zip.addFile ("word/styles.xml", QByteArray (styles));
}

void WordGenerator::writeSettings (ZipWriter &zip)
{
    const char *settings = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                           "<w:settings xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">"
                           "<w:zoom w:percent=\"100\"/>"
                           "</w:settings>";

    zip.addFile ("word/settings.xml", QByteArray (settings));
}

void WordGenerator::writeDocumentXml (ZipWriter &zip, const QList<PriceTag> &expandedTags)
{
    const QString xml = buildDocumentXml (expandedTags);

    zip.addFile ("word/document.xml", xml.toUtf8 ());
}

// Remove XML 1.0 invalid chars: 0x0-0x8, 0xB, 0xC, 0xE-0x1F
static QString sanitizeForXml (const QString &s)
{
    QString out;
    out.reserve (s.size ());

    for (QChar ch : s)
    {
        ushort u = ch.unicode ();

        if ((u >= 0x20) || u == 0x09 || u == 0x0A || u == 0x0D)
            out.append (ch);
    }


    return out;
}

static QString xmlEscapeLocal (const QString &s)
{
    QString out = sanitizeForXml (s);

    out.replace ('&', "&amp;");
    out.replace ('<', "&lt;");
    out.replace ('>', "&gt;");
    out.replace ('"', "&quot;");
    out.replace ('\'', "&apos;");


    return out;
}

QString WordGenerator::xmlEscape (const QString &s) { return xmlEscapeLocal (s); }

static inline int ptToTwipsLocal (double pt) { return static_cast<int> (pt * 20.0 + 0.5); }
static inline int mmToTwipsLocal (double mm) { return static_cast<int> (mm * 1440.0 / 25.4 + 0.5); }

static QString paragraph (const QString &text, const char *align = "left", int sizePt = 11, bool bold = false, bool italic = false,
                          bool strike = false)
{
    const int sz = static_cast<int> (sizePt * 2); // w:sz = half-points
    QString rpr;

    rpr += QString ("<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>");


    if (bold)
        rpr += "<w:b/>";

    if (italic)
        rpr += "<w:i/>";

    if (strike)
        rpr += "<w:strike/>";


    rpr += QString ("<w:sz w:val=\"%1\"/></w:rPr>").arg (sz);
    QString ppr = (QString (align) == "center")
            ? "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>"
            : "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr>";

    return QString ("<w:p>%1<w:r>%2<w:t xml:space=\"preserve\">%3</w:t></w:r></w:p>").arg (ppr, rpr, xmlEscapeLocal (text));
}

static QString paragraphWithStyle (const QString &text, const TagTextStyle &st)
{
    const int sz = static_cast<int> (st.fontSizePt * 2);
    QString rpr;

    rpr += QString ("<w:rPr><w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg (xmlEscapeLocal (st.fontFamily));


    if (st.bold)
        rpr += "<w:b/>";

    if (st.italic)
        rpr += "<w:i/>";

    if (st.strike)
        rpr += "<w:strike/>";


    rpr += QString ("<w:sz w:val=\"%1\"/></w:rPr>").arg (sz);

    QString ppr;

    if (st.align == TagTextAlign::Center)
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>";
    else if (st.align == TagTextAlign::Right)
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr>";
    else
    {
        const int indentTw = mmToTwipsLocal (.5); // .5 mm left indent by default
        ppr = QString ("<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:ind w:left=\"%1\"/><w:jc w:val=\"left\"/></w:pPr>")
                      .arg (indentTw);
    }

    return QString ("<w:p>%1<w:r>%2<w:t xml:space=\"preserve\">%3</w:t></w:r></w:p>").arg (ppr, rpr, xmlEscapeLocal (text));
}

// Same as paragraphWithStyle but without left indent for left-aligned paragraphs
static QString paragraphWithStyleNoIndent (const QString &text, const TagTextStyle &st)
{
    const int sz = static_cast<int> (st.fontSizePt * 2);
    QString rpr;

    rpr += QString ("<w:rPr><w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg (xmlEscapeLocal (st.fontFamily));


    if (st.bold)
        rpr += "<w:b/>";

    if (st.italic)
        rpr += "<w:i/>";

    if (st.strike)
        rpr += "<w:strike/>";


    rpr += QString ("<w:sz w:val=\"%1\"/></w:rPr>").arg (sz);

    QString ppr;

    if (st.align == TagTextAlign::Center)
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"center\"/></w:pPr>";
    else if (st.align == TagTextAlign::Right)
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"right\"/></w:pPr>";
    else
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr>";

    return QString ("<w:p>%1<w:r>%2<w:t xml:space=\"preserve\">%3</w:t></w:r></w:p>").arg (ppr, rpr, xmlEscapeLocal (text));
}


// Extract readable label from template text (e.g. "Страна: ..." -> "Страна:")
static QString extractLabelFromTemplate (const QString &tmpl, const QString &fallback)
{
    // Preserve leading spaces exactly as provided in template
    const QString original = tmpl;
    const QString trimmed  = original.trimmed ();

    if (trimmed.isEmpty ())
        return fallback;

    // Detect label up to colon on original string (keeps leading spaces)
    int colon = original.indexOf (':');
    if (colon >= 0)
        return original.left (colon + 1);

    // If there are letters (custom label without colon) – use as is (preserve spaces)
    for (const QChar &ch : trimmed)
    {
        if (ch.isLetter ())
            return original;
    }
    return fallback;
}


static QString createMergedTableRow (int heightTwips, const QString &content, int borderSz)
{
    QString tcBorders;

    if (borderSz > 0)
        tcBorders = QString ("<w:tcBorders><w:top w:val=\"single\" w:sz=\"%1\"/><w:bottom w:val=\"single\" w:sz=\"%1\"/></w:tcBorders>")
                            .arg (borderSz);

    return QString ("<w:tr><w:trPr><w:cantSplit/><w:trHeight w:val=\"%1\" w:hRule=\"exact\"/></w:trPr><w:tc><w:tcPr><w:tcW w:w=\"0\" "
                    "w:type=\"auto\"/><w:gridSpan w:val=\"4\"/>%2</w:tcPr>%3</w:tc></w:tr>")
            .arg (heightTwips)
            .arg (tcBorders)
            .arg (content);
}

static QString createTwoCellTableRow (int heightTwips, const QString &leftContent, const QString &rightContent, bool diagonalBL2TR,
                                      bool rightThickBorder, int borderSz)
{
    // Left cell spans 1 column; right spans 3 columns
    QString leftBorders;

    if (diagonalBL2TR)
        leftBorders += "<w:tr2bl w:val=\"single\" w:sz=\"8\"/>";

    if (borderSz > 0)
        leftBorders += QString ("<w:top w:val=\"single\" w:sz=\"%1\"/><w:bottom w:val=\"single\" w:sz=\"%1\"/>").arg (borderSz);

    QString leftTcPr = QString ("<w:tcPr><w:tcW w:w=\"0\" w:type=\"auto\"/>") +
            (leftBorders.isEmpty () ? QString () : QString ("<w:tcBorders>%1</w:tcBorders>").arg (leftBorders)) + "</w:tcPr>";


    QString rightTcPr = "<w:tcPr><w:tcW w:w=\"0\" w:type=\"auto\"/><w:gridSpan w:val=\"3\"/>";


    if (rightThickBorder)
        rightTcPr += "<w:tcBorders><w:top w:val=\"single\" w:sz=\"12\"/><w:left w:val=\"single\" w:sz=\"12\"/>"
                     "<w:bottom w:val=\"single\" w:sz=\"12\"/><w:right w:val=\"single\" w:sz=\"12\"/></w:tcBorders>";
    else if (borderSz > 0)
        rightTcPr += QString ("<w:tcBorders><w:top w:val=\"single\" w:sz=\"%1\"/><w:bottom w:val=\"single\" w:sz=\"%1\"/></w:tcBorders>")
                             .arg (borderSz);
    rightTcPr += "</w:tcPr>";


    return QString ("<w:tr><w:trPr><w:cantSplit/><w:trHeight w:val=\"%1\" w:hRule=\"exact\"/></w:trPr>"
                    "<w:tc>%2%3</w:tc>"
                    "<w:tc>%4%5</w:tc>"
                    "</w:tr>")
            .arg (heightTwips)
            .arg (leftTcPr)
            .arg (leftContent)
            .arg (rightTcPr)
            .arg (rightContent);
}

static QString createTableStructure (int tableWidth)
{
    QString xml;

    xml += "<w:tbl><w:tblPr><w:tblW w:w=\"%1\" w:type=\"dxa\"/><w:tblLayout w:type=\"fixed\"/>";
    xml = xml.arg (tableWidth);
    xml += "<w:tblCellMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right "
           "w:w=\"0\" w:type=\"dxa\"/></w:tblCellMar><w:tblBorders>";
    xml += "<w:top w:val=\"single\" w:sz=\"8\"/><w:left w:val=\"single\" w:sz=\"8\"/><w:bottom w:val=\"single\" w:sz=\"8\"/><w:right "
           "w:val=\"single\" w:sz=\"8\"/>";
    xml += "<w:insideH w:val=\"single\" w:sz=\"4\"/><w:insideV w:val=\"single\" w:sz=\"4\"/></w:tblBorders></w:tblPr>";


    return xml;
}

static QString createTableGrid (const int colTw[4])
{
    QString xml = "<w:tblGrid>";

    for (int i = 0; i < 4; ++i)
        xml += QString ("<w:gridCol w:w=\"%1\"/>").arg (colTw[i]);
    xml += "</w:tblGrid>";


    return xml;
}

static QString addCompanyHeaderRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    return createMergedTableRow (
            ptToTwipsLocal (rowHeightPt),
            paragraphWithStyle (tpl.textOrDefault (TagField::CompanyHeader), tpl.styleOrDefault (TagField::CompanyHeader)), 4);
}

static QString addBrandRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    return createMergedTableRow (ptToTwipsLocal (rowHeightPt), paragraphWithStyle (t.getBrand (), tpl.styleOrDefault (TagField::Brand)), 4);
}

static QString addCategoryRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    QString category	= t.getCategory ();
    bool appendedGender = false;

    if (! t.getGender ().isEmpty () && category.length () <= 12)
    {
        category += " " + t.getGender ();
        appendedGender = true;
    }

    if (appendedGender && ! t.getSize ().isEmpty ())
        category += " " + t.getSize ();


    return createMergedTableRow (ptToTwipsLocal (rowHeightPt), paragraphWithStyle (category, tpl.styleOrDefault (TagField::CategoryGender)),
                                 4);
}

static QString addBrandCountryRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    const QString label = extractLabelFromTemplate (tpl.textOrDefault (TagField::BrandCountry), QString::fromUtf8 ("Страна:"));

    return createMergedTableRow (ptToTwipsLocal (rowHeightPt),
                                 paragraphWithStyle (label + " " + t.getBrandCountry (), tpl.styleOrDefault (TagField::BrandCountry)), 4);
}

static QString addManufacturingPlaceRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    const QString label = extractLabelFromTemplate (tpl.textOrDefault (TagField::ManufacturingPlace), QString::fromUtf8 ("Место:"));

    return createMergedTableRow (
            ptToTwipsLocal (rowHeightPt),
            paragraphWithStyle (label + " " + t.getManufacturingPlace (), tpl.styleOrDefault (TagField::ManufacturingPlace)), 4);
}

static QString addMaterialRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    const QString label = extractLabelFromTemplate (tpl.textOrDefault (TagField::MaterialLabel), QString::fromUtf8 ("Матер-л:"));
    QString left		= paragraphWithStyle (label, tpl.styleOrDefault (TagField::MaterialLabel));
    QString right		= paragraphWithStyle (t.getMaterial (), tpl.styleOrDefault (TagField::MaterialValue));

    return createTwoCellTableRow (ptToTwipsLocal (rowHeightPt), left, right, false, false, 4);
}

static QString addArticleRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    const QString label = extractLabelFromTemplate (tpl.textOrDefault (TagField::ArticleLabel), QString::fromUtf8 ("Артикул:"));
    QString left		= paragraphWithStyle (label, tpl.styleOrDefault (TagField::ArticleLabel));
    QString right		= paragraphWithStyle (t.getArticle (), tpl.styleOrDefault (TagField::ArticleValue));

    return createTwoCellTableRow (ptToTwipsLocal (rowHeightPt), left, right, false, false, 2);
}

static QString addPriceRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    if (t.getPrice2 () > 0)
    {
        // Left cell: old price number only with strike and diagonal TL->BR

        TagTextStyle leftSt = tpl.styleOrDefault (TagField::PriceLeft);
        leftSt.strike		= true;

        QString leftContent = paragraphWithStyle (QString::number (t.getPrice (), 'f', 0), leftSt);
        QString rightContent =
                paragraphWithStyle (QString::number (t.getPrice2 (), 'f', 0) + " =", tpl.styleOrDefault (TagField::PriceRight));


        return createTwoCellTableRow (ptToTwipsLocal (rowHeightPt), leftContent, rightContent, true, true, 0);
    }
    else
    { // Left cell: forced label "Цена:"; Right cell: current price
        TagTextStyle leftSt = tpl.styleOrDefault (TagField::PriceLeft);
        leftSt.align		   = TagTextAlign::Left; // force left alignment

        QString leftContent	 = paragraphWithStyleNoIndent (QString::fromUtf8 ("Цена: "), leftSt);
        QString rightContent =
                paragraphWithStyle (QString::number (t.getPrice (), 'f', 0) + " =", tpl.styleOrDefault (TagField::PriceRight));

        return createTwoCellTableRow (ptToTwipsLocal (rowHeightPt), leftContent, rightContent, false, true, 0);
    }
}

static QString addSupplierRow (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt)
{
    const QString label = extractLabelFromTemplate (tpl.textOrDefault (TagField::SupplierLabel), QString::fromUtf8 ("Поставщик:"));
    QString left		= paragraphWithStyle (label, tpl.styleOrDefault (TagField::SupplierLabel));
    QString right		= paragraphWithStyle (t.getSupplier (), tpl.styleOrDefault (TagField::SupplierValue));

    return createTwoCellTableRow (ptToTwipsLocal (rowHeightPt), left, right, false, false, 2);
}

// Structure to hold split address lines
struct AddressLines
{
    QString line1;
    QString line2;
};

static AddressLines splitAddressIntoLines (const QString &address)
{
    AddressLines lines;
    QStringList words	 = address.simplified ().split (' ', Qt::SkipEmptyParts);
    const int charBudget = 40;
    int iWord			 = 0;

    // Fill first line
    while (iWord < words.size ())
    {
        const QString &w = words[iWord];
        int nextLen		 = (lines.line1.isEmpty () ? 0 : lines.line1.size () + 1) + w.size ();

        if (nextLen <= charBudget)
        {
            lines.line1 = lines.line1.isEmpty () ? w : lines.line1 + " " + w;
            ++iWord;
        }
        else
        {
            if (lines.line1.isEmpty ())
            {
                lines.line1 = w;
                ++iWord;
            }

            break;
        }
    }

    // Fill second line with remaining words
    while (iWord < words.size ())
    {
        const QString &w = words[iWord];
        int nextLen		 = (lines.line2.isEmpty () ? 0 : lines.line2.size () + 1) + w.size ();

        if (nextLen <= charBudget)
        {
            lines.line2 = lines.line2.isEmpty () ? w : lines.line2 + " " + w;
            ++iWord;
        }
        else
        {
            if (lines.line2.isEmpty ())
            {
                lines.line2 = w;
                ++iWord;
            }

            break;
        }
    }


    return lines;
}

static QString createAddressTableRow (const QString &addressText, double rowHeightPt, const TagTemplate &tpl, int borderSz = 2)
{
    return createMergedTableRow (ptToTwipsLocal (rowHeightPt), paragraphWithStyle (addressText, tpl.styleOrDefault (TagField::Address)),
                                 borderSz);
}

static QString addAddressRows (const PriceTag &t, const TagTemplate &tpl, double rowHeightPt1, double rowHeightPt2)
{
    // Split address into two lines with proper text wrapping
    const AddressLines lines = splitAddressIntoLines (t.getAddress ());

    // Create XML for both address rows
    QString xml;
    xml += createAddressTableRow (lines.line1, rowHeightPt1, tpl, 2);
    xml += createAddressTableRow (lines.line2, rowHeightPt2, tpl, 2);

    return xml;
}

static QString makeInnerTagTable (const PriceTag &t, const TagTemplate &tpl, int outerCellWidthTwips)
{
    // Heights in points for 11 rows
    const double pt[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};

    // Column widths in mm to mirror Excel
    const double colMm[4] = {77.1, 35.7, 35.7, 27.1}; // cm

    // Compute scaled column widths to exactly fit parent cell (no centering/shrink)
    const int targetWidth = qMax (outerCellWidthTwips, 1);
    const double sumMm	  = colMm[0] + colMm[1] + colMm[2] + colMm[3];
    const int sumTwips	  = mmToTwipsLocal (sumMm);
    const double k		  = sumTwips > 0 ? static_cast<double> (targetWidth) / static_cast<double> (sumTwips) : 1.0;

    int colTw[4];

    for (int i = 0; i < 3; ++i)
        colTw[i] = qMax (1, static_cast<int> (std::llround (mmToTwipsLocal (colMm[i]) * k)));
    colTw[3] = qMax (1, targetWidth - (colTw[0] + colTw[1] + colTw[2]));

    QString xml = createTableStructure (targetWidth);
    xml += createTableGrid (colTw);

    // Add all table rows using specialized methods
    xml += addCompanyHeaderRow (t, tpl, pt[0]);
    xml += addBrandRow (t, tpl, pt[1]);
    xml += addCategoryRow (t, tpl, pt[2]);
    xml += addBrandCountryRow (t, tpl, pt[3]);
    xml += addManufacturingPlaceRow (t, tpl, pt[4]);
    xml += addMaterialRow (t, tpl, pt[5]);
    xml += addArticleRow (t, tpl, pt[6]);
    xml += addPriceRow (t, tpl, pt[7]);
    xml += addSupplierRow (t, tpl, pt[8]);
    xml += addAddressRows (t, tpl, pt[9], pt[10]);

    xml += "</w:tbl>";


    return xml;
}

WordGenerator::DocumentDimensions WordGenerator::calculateDocumentDimensions (const DocxLayoutConfig &layoutConfig) const
{
    WordGenerator::DocumentDimensions dims;

    int columns = 1, rows = 1;
    computeGrid (layoutConfig, columns, rows);
    dims.columns = columns;
    dims.rows	 = rows;

    dims.pageWidthTwips	 = mmToTwipsLocal (210.0);
    dims.pageHeightTwips = mmToTwipsLocal (297.0);
    dims.marginLeft		 = mmToTwipsLocal (layoutConfig.marginLeftMm);
    dims.marginRight	 = mmToTwipsLocal (layoutConfig.marginRightMm);
    dims.marginTop		 = mmToTwipsLocal (layoutConfig.marginTopMm);
    dims.marginBottom	 = mmToTwipsLocal (layoutConfig.marginBottomMm);
    dims.tagWidth		 = mmToTwipsLocal (layoutConfig.tagWidthMm);


    return dims;
}

QString WordGenerator::createDocumentHeader () const
{
    QString xml;

    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    xml += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
           "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">";
    xml += "<w:body>";


    return xml;
}

QString WordGenerator::createOuterTableStructure (int tableWidth) const
{
    QString xml = "<w:tbl>";

    xml += QString ("<w:tblPr><w:tblW w:w=\"%1\" w:type=\"dxa\"/><w:tblLayout w:type=\"fixed\"/>").arg (tableWidth);
    xml += "<w:tblCellMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right "
           "w:w=\"0\" w:type=\"dxa\"/></w:tblCellMar><w:tblBorders>";
    // make only the right outer border thicker (12 = 25% thinner than 16)
    xml += "<w:top w:val=\"single\" w:sz=\"4\"/><w:left w:val=\"single\" w:sz=\"4\"/><w:bottom w:val=\"single\" w:sz=\"4\"/><w:right "
           "w:val=\"single\" w:sz=\"12\"/>";
    xml += "<w:insideH w:val=\"single\" w:sz=\"4\"/><w:insideV w:val=\"single\" w:sz=\"4\"/></w:tblBorders></w:tblPr>";


    return xml;
}

QString WordGenerator::createOuterTableGrid (int columns, int tagWidth) const
{
    QString xml = "<w:tblGrid>";

    for (int c = 0; c < columns; ++c)
        xml += QString ("<w:gridCol w:w=\"%1\"/>").arg (tagWidth);
    xml += "</w:tblGrid>";


    return xml;
}

static QString createTableCellProperties (int tagWidth)
{
    return QString ("<w:tc><w:tcPr><w:tcW w:w=\"%1\" w:type=\"dxa\"/><w:tcMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" "
                    "w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right w:w=\"0\" w:type=\"dxa\"/></w:tcMar></w:tcPr>")
            .arg (tagWidth);
}

QString WordGenerator::addTableRows (const QList<PriceTag> &expandedTags, int columns, int tagWidth) const
{
    QString xml;
    int idx = 0;

    const int total		= expandedTags.size ();
    const int totalRows = (total + columns - 1) / columns;


    for (int r = 0; r < totalRows; ++r)
    {
        xml += "<w:tr><w:trPr><w:cantSplit/></w:trPr>";

        for (int c = 0; c < columns; ++c)
        {
            xml += createTableCellProperties (tagWidth);

            if (idx < total)
                xml += makeInnerTagTable (expandedTags[idx], tagTemplate, tagWidth);
            else
                xml += paragraph ("");

            xml += "</w:tc>";
            ++idx;
        }

        xml += "</w:tr>";
    }


    return xml;
}

QString WordGenerator::addSectionProperties (const DocumentDimensions &dims) const
{
    QString xml;
    xml += "<w:p/>"; // Ensure there is a paragraph before section properties for maximum Word compatibility

    xml += QString ("<w:sectPr><w:pgSz w:w=\"%1\" w:h=\"%2\"/><w:pgMar w:top=\"%3\" w:right=\"%4\" w:bottom=\"%5\" "
                    "w:left=\"%6\"/></w:sectPr>")
                   .arg (dims.pageWidthTwips)
                   .arg (dims.pageHeightTwips)
                   .arg (dims.marginTop)
                   .arg (dims.marginRight)
                   .arg (dims.marginBottom)
                   .arg (dims.marginLeft);

    xml += "</w:body></w:document>";


    return xml;
}


QString WordGenerator::buildDocumentXml (const QList<PriceTag> &expandedTags)
{
    const WordGenerator::DocumentDimensions dims = calculateDocumentDimensions (layoutConfig);
    const int outerTableWidth					 = dims.tagWidth * dims.columns;

    // Build XML document structure using specialized methods
    QString xml = createDocumentHeader ();
    xml += createOuterTableStructure (outerTableWidth);
    xml += createOuterTableGrid (dims.columns, dims.tagWidth);
    xml += addTableRows (expandedTags, dims.columns, dims.tagWidth);
    xml += "</w:tbl>";
    xml += addSectionProperties (dims);


    return xml;
}
