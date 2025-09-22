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
        ppr = "<w:pPr><w:keepLines/><w:spacing w:before=\"0\" w:after=\"0\"/><w:jc w:val=\"left\"/></w:pPr>";

    return QString ("<w:p>%1<w:r>%2<w:t xml:space=\"preserve\">%3</w:t></w:r></w:p>").arg (ppr, rpr, xmlEscapeLocal (text));
}

static QString makeInnerTagTable (const PriceTag &t, const TagTemplate &tpl)
{
    // Heights in points for 11 rows
    const double pt[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};

    // Column widths in mm to mirror Excel
    const double colMm[4] = {77.1, 35.7, 35.7, 27.1}; // cm


    auto trMerged = [] (int twips, const QString &content, int borderSz)
    {
        QString tcBorders;
        if (borderSz > 0)
            tcBorders = QString ("<w:tcBorders><w:top w:val=\"single\" w:sz=\"%1\"/><w:bottom w:val=\"single\" w:sz=\"%1\"/></w:tcBorders>")
                                .arg (borderSz);

        return QString ("<w:tr><w:trPr><w:cantSplit/><w:trHeight w:val=\"%1\" w:hRule=\"exact\"/></w:trPr><w:tc><w:tcPr><w:tcW w:w=\"0\" "
                        "w:type=\"auto\"/><w:gridSpan w:val=\"4\"/>%2</w:tcPr>%3</w:tc></w:tr>")
                .arg (twips)
                .arg (tcBorders)
                .arg (content);
    };


    auto trTwoCells =
            [] (int twips, const QString &leftContent, const QString &rightContent, bool diagonalBL2TR, bool rightThickBorder, int borderSz)
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
            rightTcPr +=
                    QString ("<w:tcBorders><w:top w:val=\"single\" w:sz=\"%1\"/><w:bottom w:val=\"single\" w:sz=\"%1\"/></w:tcBorders>")
                            .arg (borderSz);
        rightTcPr += "</w:tcPr>";


        return QString ("<w:tr><w:trPr><w:cantSplit/><w:trHeight w:val=\"%1\" w:hRule=\"exact\"/></w:trPr>"
                        "<w:tc>%2%3</w:tc>"
                        "<w:tc>%4%5</w:tc>"
                        "</w:tr>")
                .arg (twips)
                .arg (leftTcPr)
                .arg (leftContent)
                .arg (rightTcPr)
                .arg (rightContent);
    };

    QString xml;
    xml += "<w:tbl><w:tblPr><w:tblW w:w=\"0\" w:type=\"auto\"/><w:tblCellMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" "
           "w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right w:w=\"0\" w:type=\"dxa\"/></w:tblCellMar><w:tblBorders>"
           "<w:top w:val=\"single\" w:sz=\"8\"/><w:left w:val=\"single\" w:sz=\"8\"/><w:bottom w:val=\"single\" w:sz=\"8\"/><w:right "
           "w:val=\"single\" w:sz=\"8\"/>"
           "<w:insideH w:val=\"single\" w:sz=\"4\"/><w:insideV w:val=\"single\" w:sz=\"4\"/>"
           "</w:tblBorders></w:tblPr>";

    // Define table grid with 4 columns; allow scaling (pack multiple tags per outer cell width)
    xml += "<w:tblGrid>";

    for (int i = 0; i < 4; ++i)
        xml += QString ("<w:gridCol w:w=\"%1\"/>").arg (mmToTwipsLocal (colMm[i]));

    xml += "</w:tblGrid>";

    xml += trMerged (ptToTwipsLocal (pt[0]), paragraphWithStyle ("ИП Новиков А.В.", tpl.styleOrDefault (TagField::CompanyHeader)), 4);
    xml += trMerged (ptToTwipsLocal (pt[1]), paragraphWithStyle (t.getBrand (), tpl.styleOrDefault (TagField::Brand)), 4);


    QString category	= t.getCategory ();
    bool appendedGender = false;

    if (! t.getGender ().isEmpty () && category.length () <= 12)
    {
        category += " " + t.getGender ();
        appendedGender = true;
    }

    if (appendedGender && ! t.getSize ().isEmpty ())
        category += " " + t.getSize ();

    xml += trMerged (ptToTwipsLocal (pt[2]), paragraphWithStyle (category, tpl.styleOrDefault (TagField::CategoryGender)), 4);
    xml += trMerged (ptToTwipsLocal (pt[3]),
                     paragraphWithStyle ("Страна: " + t.getBrandCountry (), tpl.styleOrDefault (TagField::BrandCountry)), 4);
    xml += trMerged (ptToTwipsLocal (pt[4]),
                     paragraphWithStyle ("Место: " + t.getManufacturingPlace (), tpl.styleOrDefault (TagField::ManufacturingPlace)), 4);


    // Material row split into two cells
    {
        QString left  = paragraphWithStyle ("Матер-л:", tpl.styleOrDefault (TagField::MaterialLabel));
        QString right = paragraphWithStyle (t.getMaterial (), tpl.styleOrDefault (TagField::MaterialValue));

        xml += trTwoCells (ptToTwipsLocal (pt[5]), left, right, false, false, 4);
    }

    // Article row split into two cells; label not bold
    {
        QString left  = paragraphWithStyle ("Артикул:", tpl.styleOrDefault (TagField::ArticleLabel));
        QString right = paragraphWithStyle (t.getArticle (), tpl.styleOrDefault (TagField::ArticleValue));

        xml += trTwoCells (ptToTwipsLocal (pt[6]), left, right, false, false, 2);
    }

    if (t.getPrice2 () > 0)
    { // Left cell: old price number only with strike and diagonal TL->BR

        TagTextStyle leftSt = tpl.styleOrDefault (TagField::PriceLeft);

        leftSt.strike = true;

        QString leftContent = paragraphWithStyle (QString::number (t.getPrice (), 'f', 0), leftSt);
        QString rightContent =
                paragraphWithStyle (QString::number (t.getPrice2 (), 'f', 0) + " =", tpl.styleOrDefault (TagField::PriceRight));

        xml += trTwoCells (ptToTwipsLocal (pt[7]), leftContent, rightContent, true, true, 0);
    }
    else
    { // Left cell: label "Цена"; Right cell: current price

        QString leftContent = paragraphWithStyle ("Цена", tpl.styleOrDefault (TagField::PriceLeft));
        QString rightContent =
                paragraphWithStyle (QString::number (t.getPrice (), 'f', 0) + " =", tpl.styleOrDefault (TagField::PriceRight));

        xml += trTwoCells (ptToTwipsLocal (pt[7]), leftContent, rightContent, false, true, 0);
    }
    // Supplier row split into label and value
    {
        QString left  = paragraphWithStyle ("Поставщик:", tpl.styleOrDefault (TagField::SupplierLabel));
        QString right = paragraphWithStyle (t.getSupplier (), tpl.styleOrDefault (TagField::SupplierValue));

        // Supplier row moved up (index 8)
        xml += trTwoCells (ptToTwipsLocal (pt[8]), left, right, false, false, 2);
    }


    QString address	  = t.getAddress ().simplified ();
    QStringList words = address.split (' ', Qt::SkipEmptyParts);
    QString line1, line2;
    const int charBudget = 40;
    int iWord			 = 0;


    while (iWord < words.size ())
    {
        const QString &w = words[iWord];
        int nextLen		 = (line1.isEmpty () ? 0 : line1.size () + 1) + w.size ();

        if (nextLen <= charBudget)
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
        const QString &w = words[iWord];
        int nextLen		 = (line2.isEmpty () ? 0 : line2.size () + 1) + w.size ();

        if (nextLen <= charBudget)
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

    xml += trMerged (ptToTwipsLocal (pt[9]), paragraphWithStyle (line1, tpl.styleOrDefault (TagField::Address)), 2);
    xml += trMerged (ptToTwipsLocal (pt[10]), paragraphWithStyle (line2, tpl.styleOrDefault (TagField::Address)), 2);

    xml += "</w:tbl>";


    return xml;
}


QString WordGenerator::buildDocumentXml (const QList<PriceTag> &expandedTags)
{
    int nCols = 1, nRows = 1;
    computeGrid (layoutConfig, nCols, nRows);

    const int pageWtw = WordGenerator::mmToTwips (210.0);
    const int pageHtw = WordGenerator::mmToTwips (297.0);
    const int marL	  = WordGenerator::mmToTwips (layoutConfig.marginLeftMm);
    const int marR	  = WordGenerator::mmToTwips (layoutConfig.marginRightMm);
    const int marT	  = WordGenerator::mmToTwips (layoutConfig.marginTopMm);
    const int marB	  = WordGenerator::mmToTwips (layoutConfig.marginBottomMm);
    const int tagW	  = WordGenerator::mmToTwips (layoutConfig.tagWidthMm);

    QString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    xml += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
           "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">";
    xml += "<w:body>";

    // Single outer table; Word paginates automatically. Rows are non-splittable.
    xml += "<w:tbl>";
    xml += "<w:tblPr><w:tblW w:w=\"0\" w:type=\"auto\"/><w:tblCellMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" "
           "w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right w:w=\"0\" w:type=\"dxa\"/></w:tblCellMar><w:tblBorders>"
           "<w:top w:val=\"single\" w:sz=\"4\"/><w:left w:val=\"single\" w:sz=\"4\"/><w:bottom w:val=\"single\" w:sz=\"4\"/><w:right "
           "w:val=\"single\" w:sz=\"4\"/>"
           "<w:insideH w:val=\"single\" w:sz=\"4\"/><w:insideV w:val=\"single\" w:sz=\"4\"/>"
           "</w:tblBorders></w:tblPr>";
    xml += "<w:tblGrid>";

    for (int c = 0; c < nCols; ++c)
        xml += QString ("<w:gridCol w:w=\"%1\"/>").arg (tagW);
    xml += "</w:tblGrid>";

    int idx				= 0;
    const int total		= expandedTags.size ();
    const int totalRows = (total + nCols - 1) / nCols;

    for (int r = 0; r < totalRows; ++r)
    {
        xml += "<w:tr><w:trPr><w:cantSplit/></w:trPr>";

        for (int c = 0; c < nCols; ++c)
        {
            xml += QString ("<w:tc><w:tcPr><w:tcW w:w=\"%1\" w:type=\"dxa\"/><w:tcMar><w:top w:w=\"0\" w:type=\"dxa\"/><w:left w:w=\"0\" "
                            "w:type=\"dxa\"/><w:bottom w:w=\"0\" w:type=\"dxa\"/><w:right w:w=\"0\" w:type=\"dxa\"/></w:tcMar></w:tcPr>")
                           .arg (tagW);

            if (idx < total)
                xml += makeInnerTagTable (expandedTags[idx], tagTemplate);
            else
                xml += paragraph ("");

            xml += "</w:tc>";
            ++idx;
        }

        xml += "</w:tr>";
    }

    xml += "</w:tbl>";

    // Ensure there is a paragraph before section properties for maximum Word compatibility
    xml += "<w:p/>";

    xml += QString ("<w:sectPr><w:pgSz w:w=\"%1\" w:h=\"%2\"/><w:pgMar w:top=\"%3\" w:right=\"%4\" w:bottom=\"%5\" "
                    "w:left=\"%6\"/></w:sectPr>")
                   .arg (pageWtw)
                   .arg (pageHtw)
                   .arg (marT)
                   .arg (marR)
                   .arg (marB)
                   .arg (marL);

    xml += "</w:body></w:document>";

    return xml;
}
