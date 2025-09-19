#pragma once

#include <QMap>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMap>


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

    // Preview/static texts for fields (used in editor preview and for labels)
    QMap<TagField, QString> texts;

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

    static QString defaultText (TagField field)
    {
        switch (field)
        {
        case TagField::CompanyHeader: return QObject::tr("ИП Новиков А.В.");
        case TagField::Brand: return QObject::tr("BRAND");
        case TagField::CategoryGender: return QObject::tr("Category + Gender + Size");
        case TagField::BrandCountry: return QObject::tr("Страна: ...");
        case TagField::ManufacturingPlace: return QObject::tr("Место: ...");
        case TagField::MaterialLabel: return QObject::tr("Матер-л:");
        case TagField::MaterialValue: return QObject::tr("Материал");
        case TagField::ArticleLabel: return QObject::tr("Артикул:");
        case TagField::ArticleValue: return QObject::tr("VFG0005-5");
        case TagField::PriceLeft: return QObject::tr("9 990");
        case TagField::PriceRight: return QObject::tr("8 490 =");
        case TagField::Signature: return QObject::tr("");
        case TagField::SupplierLabel: return QObject::tr("Поставщик:");
        case TagField::SupplierValue: return QObject::tr("ООО Рога и Копыта");
        case TagField::Address: return QObject::tr("г. Москва, ул. Пушкина 1\nТЦ Пример, бутик 5");
        }
        return {};
    }

    QString textOrDefault (TagField field) const
    {
        auto it = texts.constFind (field);
        if (it != texts.constEnd ())
            return it.value ();
        return defaultText (field);
    }

    static const QList<TagField> &allFields ()
    {
        static QList<TagField> fields = {TagField::CompanyHeader, TagField::Brand,          TagField::CategoryGender,
                                         TagField::BrandCountry,  TagField::ManufacturingPlace, TagField::MaterialLabel,
                                         TagField::MaterialValue, TagField::ArticleLabel,  TagField::ArticleValue,
                                         TagField::PriceLeft,     TagField::PriceRight,    TagField::Signature,
                                         TagField::SupplierLabel, TagField::SupplierValue, TagField::Address};
        return fields;
    }

    // JSON (de)serialization
    QJsonObject toJson () const
    {
        QJsonObject o;
        o["tagWidthMm"]      = tagWidthMm;
        o["tagHeightMm"]     = tagHeightMm;
        o["marginLeftMm"]    = marginLeftMm;
        o["marginTopMm"]     = marginTopMm;
        o["marginRightMm"]   = marginRightMm;
        o["marginBottomMm"]  = marginBottomMm;
        o["spacingHMm"]      = spacingHMm;
        o["spacingVMm"]      = spacingVMm;

        QJsonArray stylesArr;
        // Emit style for EVERY field (falling back to defaults when not overridden)
        for (TagField f : allFields ())
        {
            const TagTextStyle &st = styleOrDefault (f);
            QJsonObject s;
            s["field"]      = static_cast<int> (f);
            s["fontFamily"] = st.fontFamily;
            s["fontSizePt"] = st.fontSizePt;
            s["bold"]       = st.bold;
            s["italic"]     = st.italic;
            s["strike"]     = st.strike;
            s["align"]      = static_cast<int> (st.align);
            stylesArr.append (s);
        }
        o["styles"] = stylesArr;

        QJsonArray textsArr;
        for (TagField f : allFields ())
        {
            const QString txt = textOrDefault (f);
            QJsonObject t;
            t["field"] = static_cast<int> (f);
            t["text"]  = txt;
            textsArr.append (t);
        }
        o["texts"] = textsArr;
        return o;
    }

    static TagTemplate fromJson (const QJsonObject &o)
    {
        TagTemplate t;
        t.tagWidthMm      = o.value ("tagWidthMm").toDouble (t.tagWidthMm);
        t.tagHeightMm     = o.value ("tagHeightMm").toDouble (t.tagHeightMm);
        t.marginLeftMm    = o.value ("marginLeftMm").toDouble (t.marginLeftMm);
        t.marginTopMm     = o.value ("marginTopMm").toDouble (t.marginTopMm);
        t.marginRightMm   = o.value ("marginRightMm").toDouble (t.marginRightMm);
        t.marginBottomMm  = o.value ("marginBottomMm").toDouble (t.marginBottomMm);
        t.spacingHMm      = o.value ("spacingHMm").toDouble (t.spacingHMm);
        t.spacingVMm      = o.value ("spacingVMm").toDouble (t.spacingVMm);

        t.styles.clear ();
        const QJsonArray stylesArr = o.value ("styles").toArray ();
        for (const QJsonValue &v : stylesArr)
        {
            const QJsonObject s = v.toObject ();
            const int fieldInt   = s.value ("field").toInt (-1);
            if (fieldInt < 0)
                continue;
            TagTextStyle st;
            st.fontFamily = s.value ("fontFamily").toString (TagTextStyle{}.fontFamily);
            st.fontSizePt = s.value ("fontSizePt").toInt (TagTextStyle{}.fontSizePt);
            st.bold       = s.value ("bold").toBool (TagTextStyle{}.bold);
            st.italic     = s.value ("italic").toBool (TagTextStyle{}.italic);
            st.strike     = s.value ("strike").toBool (TagTextStyle{}.strike);
            st.align      = static_cast<TagTextAlign> (s.value ("align").toInt (static_cast<int> (TagTextAlign::Left)));
            t.styles.insert (static_cast<TagField> (fieldInt), st);
        }
        t.texts.clear ();
        const QJsonArray textsArr = o.value ("texts").toArray ();
        for (const QJsonValue &v : textsArr)
        {
            const QJsonObject to = v.toObject ();
            const int fieldInt    = to.value ("field").toInt (-1);
            if (fieldInt < 0) continue;
            const QString txt = to.value ("text").toString (defaultText (static_cast<TagField> (fieldInt)));
            t.texts.insert (static_cast<TagField> (fieldInt), txt);
        }
        return t;
    }
};


