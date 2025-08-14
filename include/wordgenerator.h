#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include <xlsxzipwriter_p.h>

#include "pricetag.h"


class WordGenerator: public QObject
{
    Q_OBJECT //


            public: explicit WordGenerator (QObject *parent = nullptr);
    ~WordGenerator ();


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
    DocxLayoutConfig layout () const { return layoutConfig; }


    bool generateWordDocument (const QList<PriceTag> &priceTags, const QString &outputPath);


private:
    DocxLayoutConfig layoutConfig{};


    static inline int mmToTwips (double mm) { return static_cast<int> (mm * 1440.0 / 25.4 + 0.5); }


    static void computeGrid (const DocxLayoutConfig &cfg, int &nCols, int &nRows);


    void writeContentTypes (QXlsx::ZipWriter &zip);
    void writeRelsRoot (QXlsx::ZipWriter &zip);
    void writeDocProps (QXlsx::ZipWriter &zip);
    void writeStyles (QXlsx::ZipWriter &zip);
    void writeSettings (QXlsx::ZipWriter &zip);
    void writeDocumentXml (QXlsx::ZipWriter &zip, const QList<PriceTag> &expandedTags);


    QString buildDocumentXml (const QList<PriceTag> &expandedTags);


    // Utilities:
    static QString xmlEscape (const QString &s);
    static QString formatPrice (double price);
};
