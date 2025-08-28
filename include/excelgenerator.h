#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include "pricetag.h"
#include "tagtemplate.h"


class ExcelGenerator: public QObject
{
    Q_OBJECT //


            public: explicit ExcelGenerator (QObject *parent = nullptr);
    ~ExcelGenerator ();

    bool generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath);

    struct ExcelLayoutConfig
    {
        // Geometry in millimeters (A4 portrait is fixed: 210 x 297)
        double tagWidthMm	  = 38.0; // width of one price tag
        double tagHeightMm	  = 28.0; // height of one price tag
        double marginLeftMm	  = 8.0;
        double marginTopMm	  = 8.0;
        double marginRightMm  = 8.0;
        double marginBottomMm = 8.0;
        double spacingHMm	  = 3.0; // horizontal spacing between tags
        double spacingVMm	  = 3.0; // vertical spacing between tags
    };

    void setLayoutConfig (const ExcelLayoutConfig &cfg) { layoutConfig = cfg; }
    ExcelLayoutConfig layout () const { return layoutConfig; }

    void setTagTemplate (const TagTemplate &tpl) { tagTemplate = tpl; }
    TagTemplate tagTpl () const { return tagTemplate; }


private:
    ExcelLayoutConfig layoutConfig{};
    TagTemplate tagTemplate{};
};
