#pragma once

#include <QList>
#include <QMap>
#include <QString>

// Forward declarations
class QJsonObject;


enum class TagTextAlign
{
    Left,
    Center,
    Right
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


struct TagTextStyle
{
    int fontSizePt	   = 11;
    bool bold		   = false;
    bool italic		   = false;
    bool strike		   = false;
    QString fontFamily = "Times New Roman";
    TagTextAlign align = TagTextAlign::Left;
};


class TagTemplate
{
public:
    double tagWidthMm	  = 46.0;
    double tagHeightMm	  = 51.0;
    double marginLeftMm	  = 10.0;
    double marginTopMm	  = 10.0;
    double marginRightMm  = 10.0;
    double marginBottomMm = 10.0;
    double spacingHMm	  = 5.0;
    double spacingVMm	  = 5.0;

    QMap<TagField, TagTextStyle> styles;
    QMap<TagField, QString> texts;


    const TagTextStyle &styleOrDefault (TagField field) const;

    static const TagTextStyle &defaultStyle (TagField field);
    static QString defaultText (TagField field);

    QString textOrDefault (TagField field) const;

    static const QList<TagField> &allFields ();


    // JSON de/serialization
    QJsonObject toJson () const;
    static TagTemplate fromJson (const QJsonObject &o);
};
