#include "tagtemplate.h"


namespace
{
    QString fieldKey (TagField f)
    {
        switch (f)
        {
            case TagField::CompanyHeader:
                return QStringLiteral ("CompanyHeader");
            case TagField::Brand:
                return QStringLiteral ("Brand");
            case TagField::CategoryGender:
                return QStringLiteral ("CategoryGender");
            case TagField::BrandCountry:
                return QStringLiteral ("BrandCountry");
            case TagField::ManufacturingPlace:
                return QStringLiteral ("ManufacturingPlace");
            case TagField::MaterialLabel:
                return QStringLiteral ("MaterialLabel");
            case TagField::MaterialValue:
                return QStringLiteral ("MaterialValue");
            case TagField::ArticleLabel:
                return QStringLiteral ("ArticleLabel");
            case TagField::ArticleValue:
                return QStringLiteral ("ArticleValue");
            case TagField::PriceLeft:
                return QStringLiteral ("PriceLeft");
            case TagField::PriceRight:
                return QStringLiteral ("PriceRight");
            case TagField::Signature:
                return QStringLiteral ("Signature");
            case TagField::SupplierLabel:
                return QStringLiteral ("SupplierLabel");
            case TagField::SupplierValue:
                return QStringLiteral ("SupplierValue");
            case TagField::Address:
                return QStringLiteral ("Address");
        }


        return QStringLiteral ("Unknown");
    }

    bool keyToField (const QString &key, TagField &out)
    {
        static const QMap<QString, TagField> map = {{QStringLiteral ("CompanyHeader"), TagField::CompanyHeader},
                                                    {QStringLiteral ("Brand"), TagField::Brand},
                                                    {QStringLiteral ("CategoryGender"), TagField::CategoryGender},
                                                    {QStringLiteral ("BrandCountry"), TagField::BrandCountry},
                                                    {QStringLiteral ("ManufacturingPlace"), TagField::ManufacturingPlace},
                                                    {QStringLiteral ("MaterialLabel"), TagField::MaterialLabel},
                                                    {QStringLiteral ("MaterialValue"), TagField::MaterialValue},
                                                    {QStringLiteral ("ArticleLabel"), TagField::ArticleLabel},
                                                    {QStringLiteral ("ArticleValue"), TagField::ArticleValue},
                                                    {QStringLiteral ("PriceLeft"), TagField::PriceLeft},
                                                    {QStringLiteral ("PriceRight"), TagField::PriceRight},
                                                    {QStringLiteral ("Signature"), TagField::Signature},
                                                    {QStringLiteral ("SupplierLabel"), TagField::SupplierLabel},
                                                    {QStringLiteral ("SupplierValue"), TagField::SupplierValue},
                                                    {QStringLiteral ("Address"), TagField::Address}};

        auto it = map.find (key);

        if (it == map.end ())
            return false;

        out = it.value ();


        return true;
    }

    QString alignToString (TagTextAlign a)
    {
        switch (a)
        {
            case TagTextAlign::Left:
                return QStringLiteral ("Left");
            case TagTextAlign::Center:
                return QStringLiteral ("Center");
            case TagTextAlign::Right:
                return QStringLiteral ("Right");
        }


        return QStringLiteral ("Left");
    }

    TagTextAlign stringToAlign (const QJsonValue &v)
    {
        if (v.isString ())
        {
            const QString s = v.toString ();

            if (s.compare (QLatin1String ("Left"), Qt::CaseInsensitive) == 0)
                return TagTextAlign::Left;

            if (s.compare (QLatin1String ("Center"), Qt::CaseInsensitive) == 0)
                return TagTextAlign::Center;

            if (s.compare (QLatin1String ("Right"), Qt::CaseInsensitive) == 0)
                return TagTextAlign::Right;
        }

        if (v.isDouble ())
        {
            const int i = v.toInt (0);

            if (i == static_cast<int> (TagTextAlign::Center))
                return TagTextAlign::Center;

            if (i == static_cast<int> (TagTextAlign::Right))
                return TagTextAlign::Right;

            return TagTextAlign::Left;
        }


        return TagTextAlign::Left;
    }

} // namespace


const TagTextStyle &TagTemplate::styleOrDefault (TagField field) const
{
    const auto it = styles.constFind (field);

    if (it != styles.constEnd ())
        return it.value ();

    return defaultStyle (field);
}


const TagTextStyle &TagTemplate::defaultStyle (TagField field)
{
    static QMap<TagField, TagTextStyle> defaults;

    if (defaults.isEmpty ())
    {
        TagTextStyle base;

        {
            TagTextStyle st = base;

            st.align	  = TagTextAlign::Center;
            st.bold		  = true;
            st.fontSizePt = 13;
            defaults.insert (TagField::CompanyHeader, st);
        }

        {
            TagTextStyle st = base;

            st.align	  = TagTextAlign::Center;
            st.bold		  = true;
            st.fontSizePt = 12;
            defaults.insert (TagField::Brand, st);
        }

        defaults.insert (TagField::CategoryGender, base);
        defaults.insert (TagField::BrandCountry, base);
        defaults.insert (TagField::ManufacturingPlace, base);

        {
            TagTextStyle st = base;

            st.bold = true;
            defaults.insert (TagField::MaterialLabel, st);
        }

        defaults.insert (TagField::MaterialValue, base);

        {
            TagTextStyle st = base;

            st.bold = true;
            defaults.insert (TagField::ArticleLabel, st);
        }

        {
            TagTextStyle st = base;

            st.bold = true;

            defaults.insert (TagField::ArticleValue, st);
        }

        defaults.insert (TagField::PriceLeft, base);
        defaults.insert (TagField::PriceRight, base);
        defaults.insert (TagField::Signature, base);

        {
            TagTextStyle st = base;

            st.bold = true;

            defaults.insert (TagField::SupplierLabel, st);
        }

        defaults.insert (TagField::SupplierValue, base);
        defaults.insert (TagField::Address, base);
    }


    return defaults[field];
}

QString TagTemplate::defaultText (TagField field)
{
    switch (field)
    {
        case TagField::CompanyHeader:
            return QString::fromUtf8 ("Заголовок компании");
        case TagField::Brand:
            return QString::fromUtf8 ("Бренд");
        case TagField::CategoryGender:
            return QString::fromUtf8 ("Категория + Пол");
        case TagField::BrandCountry:
            return QString::fromUtf8 ("Страна:");
        case TagField::ManufacturingPlace:
            return QString::fromUtf8 ("Место:");
        case TagField::MaterialLabel:
            return QString::fromUtf8 ("Матер-л:");
        case TagField::MaterialValue:
            return QString::fromUtf8 ("Материал");
        case TagField::ArticleLabel:
            return QString::fromUtf8 ("Артикул:");
        case TagField::ArticleValue:
            return QString::fromUtf8 ("A-12345");
        case TagField::PriceLeft:
            return QString::fromUtf8 ("");
        case TagField::PriceRight:
            return QString::fromUtf8 ("");
        case TagField::Signature:
            return QString::fromUtf8 ("");
        case TagField::SupplierLabel:
            return QString::fromUtf8 ("Поставщик:");
        case TagField::SupplierValue:
            return QString::fromUtf8 ("ООО Ромашка");
        case TagField::Address:
            return QString::fromUtf8 ("Россия, Москва, ул. Пример, 1\n+7 (000) 000-00-00");
    }


    return QString ();
}


QString TagTemplate::textOrDefault (TagField field) const
{
    const auto it = texts.constFind (field);

    if (it != texts.constEnd ())
        return it.value ();


    return defaultText (field);
}


const QList<TagField> &TagTemplate::allFields ()
{
    static const QList<TagField> fields = {
            TagField::CompanyHeader,	  TagField::Brand,		   TagField::CategoryGender, TagField::BrandCountry,
            TagField::ManufacturingPlace, TagField::MaterialLabel, TagField::MaterialValue,	 TagField::ArticleLabel,
            TagField::ArticleValue,		  TagField::PriceLeft,	   TagField::PriceRight,	 TagField::Signature,
            TagField::SupplierLabel,	  TagField::SupplierValue, TagField::Address};


    return fields;
}


QJsonObject TagTemplate::toJson () const
{
    QJsonObject o;

    o.insert (QStringLiteral ("tagWidthMm"), tagWidthMm);
    o.insert (QStringLiteral ("tagHeightMm"), tagHeightMm);
    o.insert (QStringLiteral ("marginLeftMm"), marginLeftMm);
    o.insert (QStringLiteral ("marginTopMm"), marginTopMm);
    o.insert (QStringLiteral ("marginRightMm"), marginRightMm);
    o.insert (QStringLiteral ("marginBottomMm"), marginBottomMm);
    o.insert (QStringLiteral ("spacingHMm"), spacingHMm);
    o.insert (QStringLiteral ("spacingVMm"), spacingVMm);

    QJsonObject stylesObj;

    for (TagField f : allFields ())
    {
        const TagTextStyle st = styles.contains (f) ? styles.value (f) : defaultStyle (f);
        QJsonObject so;

        so.insert (QStringLiteral ("fontFamily"), st.fontFamily);
        so.insert (QStringLiteral ("fontSizePt"), st.fontSizePt);
        so.insert (QStringLiteral ("bold"), st.bold);
        so.insert (QStringLiteral ("italic"), st.italic);
        so.insert (QStringLiteral ("strike"), st.strike);
        so.insert (QStringLiteral ("align"), alignToString (st.align));

        stylesObj.insert (fieldKey (f), so);
    }

    o.insert (QStringLiteral ("styles"), stylesObj);


    QJsonObject textsObj;

    for (TagField f : allFields ())
    {
        const QString txt = texts.contains (f) ? texts.value (f) : defaultText (f);

        textsObj.insert (fieldKey (f), txt);
    }

    o.insert (QStringLiteral ("texts"), textsObj);


    return o;
}


TagTemplate TagTemplate::fromJson (const QJsonObject &o)
{
    TagTemplate t;

    t.tagWidthMm	 = o.value (QStringLiteral ("tagWidthMm")).toDouble (t.tagWidthMm);
    t.tagHeightMm	 = o.value (QStringLiteral ("tagHeightMm")).toDouble (t.tagHeightMm);
    t.marginLeftMm	 = o.value (QStringLiteral ("marginLeftMm")).toDouble (t.marginLeftMm);
    t.marginTopMm	 = o.value (QStringLiteral ("marginTopMm")).toDouble (t.marginTopMm);
    t.marginRightMm	 = o.value (QStringLiteral ("marginRightMm")).toDouble (t.marginRightMm);
    t.marginBottomMm = o.value (QStringLiteral ("marginBottomMm")).toDouble (t.marginBottomMm);
    t.spacingHMm	 = o.value (QStringLiteral ("spacingHMm")).toDouble (t.spacingHMm);
    t.spacingVMm	 = o.value (QStringLiteral ("spacingVMm")).toDouble (t.spacingVMm);


    if (o.contains (QStringLiteral ("styles")) && o.value (QStringLiteral ("styles")).isObject ())
    {
        const QJsonObject stylesObj = o.value (QStringLiteral ("styles")).toObject ();

        for (auto it = stylesObj.begin (); it != stylesObj.end (); ++it)
        {
            TagField f;

            if (! keyToField (it.key (), f))
                continue;


            const QJsonObject so = it.value ().toObject ();
            TagTextStyle st;

            st.fontFamily = so.value (QStringLiteral ("fontFamily")).toString (st.fontFamily);
            st.fontSizePt = so.value (QStringLiteral ("fontSizePt")).toInt (st.fontSizePt);
            st.bold		  = so.value (QStringLiteral ("bold")).toBool (st.bold);
            st.italic	  = so.value (QStringLiteral ("italic")).toBool (st.italic);
            st.strike	  = so.value (QStringLiteral ("strike")).toBool (st.strike);
            st.align	  = stringToAlign (so.value (QStringLiteral ("align")));

            t.styles.insert (f, st);
        }
    }


    if (o.contains (QStringLiteral ("texts")) && o.value (QStringLiteral ("texts")).isObject ())
    {
        const QJsonObject textsObj = o.value (QStringLiteral ("texts")).toObject ();

        for (auto it = textsObj.begin (); it != textsObj.end (); ++it)
        {
            TagField f;

            if (! keyToField (it.key (), f))
                continue;

            t.texts.insert (f, it.value ().toString ());
        }
    }


    return t;
}
