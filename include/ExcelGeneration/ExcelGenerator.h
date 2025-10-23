#pragma once

#include "pricetag.h"
#include "tagtemplate.h"


class ExcelGenerator: public QObject
{
    Q_OBJECT


public:
    explicit ExcelGenerator (QObject *parent = nullptr);
    ~ExcelGenerator ();


    // Geometry in millimeters (A4 portrait is fixed: 210 x 297)
    struct ExcelLayoutConfig
    {
        double tagWidthMm	  = 38.0;
        double tagHeightMm	  = 28.0;
        double marginLeftMm	  = 8.0;
        double marginTopMm	  = 8.0;
        double marginRightMm  = 8.0;
        double marginBottomMm = 8.0;
        double spacingHMm	  = 3.0;
        double spacingVMm	  = 3.0;
    };


    bool generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath);

    ExcelLayoutConfig layout () const { return layoutConfig; }

    TagTemplate tagTpl () const { return tagTemplate; }

    void setLayoutConfig (const ExcelLayoutConfig &cfg) { layoutConfig = cfg; }
    void setTagTemplate (const TagTemplate &tpl) { tagTemplate = tpl; }


private:
    ExcelLayoutConfig layoutConfig{};
    TagTemplate tagTemplate{};
};
