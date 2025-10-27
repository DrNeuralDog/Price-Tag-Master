#pragma once

#include <QObject>

#include "tagtemplate.h"

// Forward declarations
class PriceTag;
class QString;
template <typename T> class QList;

namespace QXlsx {
class ZipWriter;
}


class WordGenerator: public QObject
{
    Q_OBJECT


public:
    explicit WordGenerator (QObject *parent = nullptr);
    ~WordGenerator ();


    // Small tag size to fit multiple per row on A4:
    struct DocxLayoutConfig
    {
        double tagWidthMm	  = 38.0;
        double tagHeightMm	  = 28.0;
        double marginLeftMm	  = 8.0;
        double marginTopMm	  = 8.0;
        double marginRightMm  = 8.0;
        double marginBottomMm = 8.0;
        double spacingHMm	  = 0.0;
        double spacingVMm	  = 0.0;
    };


    void setLayoutConfig (const DocxLayoutConfig &cfg) { layoutConfig = cfg; }
    void setTagTemplate (const TagTemplate &tpl) { tagTemplate = tpl; }

    DocxLayoutConfig layout () const { return layoutConfig; }

    TagTemplate tagTpl () const { return tagTemplate; }

    bool generateWordDocument (const QList<PriceTag> &priceTags, const QString &outputPath);


private:
    DocxLayoutConfig layoutConfig{};

    TagTemplate tagTemplate{};

    static inline int mmToTwips (double mm) { return static_cast<int> (mm * 1440.0 / 25.4 + 0.5); }

    static void computeGrid (const DocxLayoutConfig &cfg, int &nCols, int &nRows);

    void writeContentTypes (QXlsx::ZipWriter &zip);
    void writeRelsRoot (QXlsx::ZipWriter &zip);
    void writeDocProps (QXlsx::ZipWriter &zip);
    void writeStyles (QXlsx::ZipWriter &zip);
    void writeSettings (QXlsx::ZipWriter &zip);
    void writeDocumentXml (QXlsx::ZipWriter &zip, const QList<PriceTag> &expandedTags);

    QString buildDocumentXml (const QList<PriceTag> &expandedTags);


private:
    struct DocumentDimensions
    {
        int pageWidthTwips;
        int pageHeightTwips;

        int marginLeft;
        int marginRight;
        int marginTop;
        int marginBottom;

        int tagWidth;
        int columns;
        int rows;
    };


    // Helper method to calculate document dimensions:
    DocumentDimensions calculateDocumentDimensions (const DocxLayoutConfig &layoutConfig) const;


    // Helper methods for building document XML:
    QString createDocumentHeader () const;
    QString createOuterTableStructure (int tableWidth) const;
    QString createOuterTableGrid (int columns, int tagWidth) const;

    QString addTableRows (const QList<PriceTag> &expandedTags, int columns, int tagWidth) const;
    QString addSectionProperties (const DocumentDimensions &dims) const;


    // Utilities:
    static QString xmlEscape (const QString &s);
    static QString formatPrice (double price);
};
