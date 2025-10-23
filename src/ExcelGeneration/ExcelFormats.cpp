#include "ExcelFormats.h"

#include <xlsxformat.h>


namespace ExcelGen
{

    QXlsx::Format::HorizontalAlignment toQXlsxHAlign (TagTextAlign a)
    {
        switch (a)
        {
            case TagTextAlign::Left:
                return QXlsx::Format::AlignLeft;
            case TagTextAlign::Center:
                return QXlsx::Format::AlignHCenter;
            case TagTextAlign::Right:
                return QXlsx::Format::AlignRight;
        }


        return QXlsx::Format::AlignLeft;
    }


    void applyTextStyle (QXlsx::Format &fmt, const TagTextStyle &st)
    {
        fmt.setFontName (st.fontFamily);
        fmt.setFontSize (st.fontSizePt);
        fmt.setFontBold (st.bold);
        fmt.setFontItalic (st.italic);


        if (st.strike)
            fmt.setFontStrikeOut (true);

        fmt.setHorizontalAlignment (toQXlsxHAlign (st.align));
        fmt.setVerticalAlignment (QXlsx::Format::AlignVCenter);
        fmt.setTextWrap (true);


        if (st.align == TagTextAlign::Left)
            fmt.setIndent (0);
    }


    QXlsx::Format withOuterEdges (const QXlsx::Format &base, bool left, bool right, bool top, bool bottom)
    {
        QXlsx::Format f = base;


        if (left)
            f.setLeftBorderStyle (QXlsx::Format::BorderMedium);

        if (right)
            f.setRightBorderStyle (QXlsx::Format::BorderMedium);

        if (top)
            f.setTopBorderStyle (QXlsx::Format::BorderMedium);

        if (bottom)
            f.setBottomBorderStyle (QXlsx::Format::BorderMedium);


        return f;
    }


    TagFormats createTagFormats (const TagTemplate &tagTemplate)
    {
        TagFormats tf;

        const TagTextStyle stCompany   = tagTemplate.styleOrDefault (TagField::CompanyHeader);
        const TagTextStyle stBrand	   = tagTemplate.styleOrDefault (TagField::Brand);
        const TagTextStyle stCategory  = tagTemplate.styleOrDefault (TagField::CategoryGender);
        const TagTextStyle stBrandC	   = tagTemplate.styleOrDefault (TagField::BrandCountry);
        const TagTextStyle stManuf	   = tagTemplate.styleOrDefault (TagField::ManufacturingPlace);
        const TagTextStyle stMatLab	   = tagTemplate.styleOrDefault (TagField::MaterialLabel);
        const TagTextStyle stMatVal	   = tagTemplate.styleOrDefault (TagField::MaterialValue);
        const TagTextStyle stArtLab	   = tagTemplate.styleOrDefault (TagField::ArticleLabel);
        const TagTextStyle stArtVal	   = tagTemplate.styleOrDefault (TagField::ArticleValue);
        const TagTextStyle stPriceL	   = tagTemplate.styleOrDefault (TagField::PriceLeft);
        const TagTextStyle stPriceR	   = tagTemplate.styleOrDefault (TagField::PriceRight);
        const TagTextStyle stSupplierL = tagTemplate.styleOrDefault (TagField::SupplierLabel);
        const TagTextStyle stAddress   = tagTemplate.styleOrDefault (TagField::Address);


        tf.headerFormat.setFontBold (false);
        applyTextStyle (tf.headerFormat, stCompany);
        tf.headerFormat.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.brandFormat, stBrand);
        tf.brandFormat.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.categoryFormat, stCategory);
        tf.categoryFormat.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.brendCountryFormat, stBrandC);
        tf.brendCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.developCountryFormat, stManuf);
        tf.developCountryFormat.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.materialHeaderFormat, stMatLab);
        applyTextStyle (tf.materialValueFormat, stMatVal);

        applyTextStyle (tf.articulHeaderFormat, stArtLab);
        applyTextStyle (tf.articulValueFormat, stArtVal);

        applyTextStyle (tf.priceFormatCell1, stPriceL);
        applyTextStyle (tf.priceFormatCell2, stPriceR);
        tf.priceFormatCell2.setBorderStyle (QXlsx::Format::BorderThin);

        applyTextStyle (tf.strikePriceFormat, stPriceL);
        tf.strikePriceFormat.setFontStrikeOut (true);
        tf.strikePriceFormat.setDiagonalBorderStyle (QXlsx::Format::BorderThin);
        tf.strikePriceFormat.setDiagonalBorderType (QXlsx::Format::DiagonalBorderUp);

        applyTextStyle (tf.supplierFormat, stSupplierL);
        tf.supplierFormat.setTopBorderStyle (QXlsx::Format::BorderThin);
        tf.supplierFormat.setTextWrap (true);

        applyTextStyle (tf.addressFormat, stAddress);
        tf.addressFormat.setTextWrap (true);


        return tf;
    }

} // namespace ExcelGen
