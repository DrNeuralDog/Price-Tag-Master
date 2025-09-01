#pragma once

#include <QMap>
#include <QString>


enum class TagTextAlign
{
    Left,
    Center,
    Right
};


struct TagTextStyle
{
    QString fontFamily = "Times New Roman";
    int fontSizePt     = 11;
    bool bold          = false;
    bool italic        = false;
    bool strike        = false;
    TagTextAlign align = TagTextAlign::Left;
};


enum class TagField
{
    CompanyHeader,
    Brand,
    CategoryGender,
    BrandCountry,
    ManufacturingPlace,
    MaterialLabel,
    MaterialValue,
    ArticleLabel,
    ArticleValue,
    PriceLeft,
    PriceRight,
    Signature,
    SupplierLabel,
    SupplierValue,
    Address
};


class TagTemplate
{
public:
    // Geometry in millimeters
    double tagWidthMm      = 50.0;
    double tagHeightMm     = 30.0;
    double marginLeftMm    = 10.0;
    double marginTopMm     = 10.0;
    double marginRightMm   = 10.0;
    double marginBottomMm  = 10.0;
    double spacingHMm      = 5.0;
    double spacingVMm      = 5.0;

    // Styles for individual fields
    QMap<TagField, TagTextStyle> styles;

    const TagTextStyle &styleOrDefault (TagField field) const
    {
        auto it = styles.constFind (field);
        if (it != styles.constEnd ())
            return it.value ();
        return defaultStyle (field);
    }

    static const TagTextStyle &defaultStyle (TagField field)
    {
        static TagTextStyle def; // Times 11 left
        static TagTextStyle center11;
        center11.fontFamily = "Times New Roman";
        center11.fontSizePt = 11;
        center11.align      = TagTextAlign::Center;
        static TagTextStyle centerBold12;
        centerBold12.fontFamily = "Times New Roman";
        centerBold12.fontSizePt = 12;
        centerBold12.bold       = true;
        centerBold12.align      = TagTextAlign::Center;

        static TagTextStyle center12;
        center12.fontFamily = "Times New Roman";
        center12.fontSizePt = 12;
        center12.align      = TagTextAlign::Center;

        static TagTextStyle small7BoldIt;
        small7BoldIt.fontFamily = "Times New Roman";
        small7BoldIt.fontSizePt = 7;
        small7BoldIt.bold       = true;
        small7BoldIt.italic     = true;

        static TagTextStyle small7;
        small7.fontFamily = "Times New Roman";
        small7.fontSizePt = 7;

        static TagTextStyle left10;
        left10.fontFamily = "Times New Roman";
        left10.fontSizePt = 10;
        left10.align      = TagTextAlign::Left;

        static TagTextStyle left11Bold;
        left11Bold.fontFamily = "Times New Roman";
        left11Bold.fontSizePt = 11;
        left11Bold.bold       = true;

        switch (field)
        {
        case TagField::CompanyHeader: return center11;
        case TagField::Brand: return centerBold12;
        case TagField::CategoryGender: return center12;
        case TagField::BrandCountry: return small7BoldIt;
        case TagField::ManufacturingPlace: return small7BoldIt;
        case TagField::MaterialLabel: return left10;
        case TagField::MaterialValue: return left10;
        case TagField::ArticleLabel: return def;
        case TagField::ArticleValue: return left11Bold;
        case TagField::PriceLeft: return center12;
        case TagField::PriceRight: return centerBold12;
        case TagField::Signature: return def;
        case TagField::SupplierLabel: return small7;
        case TagField::SupplierValue: return small7;
        case TagField::Address: return small7BoldIt;
        }
        return def;
    }
};


