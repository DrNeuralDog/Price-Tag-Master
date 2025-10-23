#include "ExcelWriters.h"


namespace ExcelGen
{

    void writeCompanyHeaderRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const TagTemplate &tagTemplate, const TagFormats &tf)
    {
        QXlsx::Format fmt = withOuterEdges (tf.headerFormat, true, true, true, false);
        const QString txt = tagTemplate.textOrDefault (TagField::CompanyHeader);

        xlsx.mergeCells (QXlsx::CellRange (row, col, row, col + tagCols - 1), fmt);

        writeWithInvisiblePad (xlsx, row, col, fmt, txt);
    }

    void writeBrandRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf)
    {
        Q_UNUSED (tagCols);
        QXlsx::Format fmt = withOuterEdges (tf.brandFormat, true, true, false, false);

        xlsx.mergeCells (QXlsx::CellRange (row + 1, col, row + 1, col + tagCols - 1), fmt);

        const QString txt = tag.getBrand ();
        const int lead	  = countLeadingSpacesGeneric (txt);

        if (lead > 0)
            fmt.setIndent (qMin (15, lead));

        writeWithInvisiblePad (xlsx, row + 1, col, fmt, txt.mid (lead));
    }

    void writeCategoryRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf)
    {
        QXlsx::Format fmt = withOuterEdges (tf.categoryFormat, true, true, false, false);
        xlsx.mergeCells (QXlsx::CellRange (row + 2, col, row + 2, col + tagCols - 1), fmt);

        QString categoryText = tag.getCategory ();
        bool appendedGender	 = false;

        if (! tag.getGender ().isEmpty () && categoryText.length () <= 12)
        {
            categoryText += " " + tag.getGender ();
            appendedGender = true;
        }

        if (appendedGender && ! tag.getSize ().isEmpty ())
            categoryText += " " + tag.getSize ();

        const int lead = countLeadingSpacesGeneric (categoryText);
        if (lead > 0)
            fmt.setIndent (qMin (15, lead));

        writeWithInvisiblePad (xlsx, row + 2, col, fmt, categoryText.mid (lead));
    }

    void writeBrandCountryRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                               const TagFormats &tf)
    {
        QXlsx::Format fmt = withOuterEdges (tf.brendCountryFormat, true, true, false, false);
        xlsx.mergeCells (QXlsx::CellRange (row + 3, col, row + 3, col + tagCols - 1), fmt);

        const QString labelRaw =
                extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::BrandCountry), QString::fromUtf8 ("Страна:"));

        writeWithInvisiblePad (xlsx, row + 3, col, fmt, labelRaw + " " + tag.getBrandCountry ());
    }

    void writeManufacturingPlaceRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag,
                                     const TagTemplate &tagTemplate, const TagFormats &tf)
    {
        QXlsx::Format fmt = withOuterEdges (tf.developCountryFormat, true, true, false, false);
        xlsx.mergeCells (QXlsx::CellRange (row + 4, col, row + 4, col + tagCols - 1), fmt);

        const QString labelRaw =
                extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ManufacturingPlace), QString::fromUtf8 ("Место:"));

        writeWithInvisiblePad (xlsx, row + 4, col, fmt, labelRaw + " " + tag.getManufacturingPlace ());
    }

    void writeMaterialRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                           const TagFormats &tf)
    {
        QXlsx::Format fmtLeft  = withOuterEdges (tf.materialHeaderFormat, true, false, false, false);
        QXlsx::Format fmtRight = withOuterEdges (tf.materialValueFormat, false, true, false, false);

        const QString labelRaw =
                extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::MaterialLabel), QString::fromUtf8 ("Матер-л:"));
        writeWithInvisiblePad (xlsx, row + 5, col, fmtLeft, labelRaw);
        xlsx.mergeCells (QXlsx::CellRange (row + 5, col + 1, row + 5, col + tagCols - 1), fmtRight);

        const QString val	 = tag.getMaterial ();
        const int valLeadMat = countLeadingSpacesGeneric (val);

        if (valLeadMat > 0)
            fmtRight.setIndent (qMin (15, valLeadMat));

        writeWithInvisiblePad (xlsx, row + 5, col + 1, fmtRight, val.mid (valLeadMat));
    }

    void writeArticleRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                          const TagFormats &tf)
    {
        QXlsx::Format fmtLeft  = withOuterEdges (tf.articulHeaderFormat, true, false, false, false);
        QXlsx::Format fmtRight = withOuterEdges (tf.articulValueFormat, false, true, false, false);

        const QString labelRaw =
                extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::ArticleLabel), QString::fromUtf8 ("Артикул:"));

        writeWithInvisiblePad (xlsx, row + 6, col, fmtLeft, labelRaw);
        xlsx.mergeCells (QXlsx::CellRange (row + 6, col + 1, row + 6, col + tagCols - 1), fmtRight);

        const QString val	 = tag.getArticle ();
        const int valLeadArt = countLeadingSpacesGeneric (val);

        if (valLeadArt > 0)
            fmtRight.setIndent (qMin (15, valLeadArt));

        writeWithInvisiblePad (xlsx, row + 6, col + 1, fmtRight, val.mid (valLeadArt));
    }

    void writePriceRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                        const TagFormats &tf)
    {
        Q_UNUSED (tagTemplate);


        if (tag.getPrice2 () > 0)
        {
            QString priceText			 = QString::number (tag.getPrice ());
            QXlsx::Format fmtLeft		 = withOuterEdges (tf.strikePriceFormat, true, false, false, false);
            const QXlsx::Format fmtRight = withOuterEdges (tf.priceFormatCell2, false, true, false, false);
            const int lead				 = countLeadingSpacesGeneric (priceText);

            if (lead > 0)
                fmtLeft.setIndent (qMin (15, lead));

            xlsx.write (row + 7, col, priceText.mid (lead), fmtLeft);
            xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
            xlsx.write (row + 7, col + 1, QString::number (tag.getPrice2 ()) + " =", fmtRight);
        }
        else
        {
            QXlsx::Format fmtLeft		 = withOuterEdges (tf.priceFormatCell1, true, false, false, false);
            const QXlsx::Format fmtRight = withOuterEdges (tf.priceFormatCell2, false, true, false, false);

            fmtLeft.setHorizontalAlignment (QXlsx::Format::AlignLeft);
            fmtLeft.setIndent (0);

            writeWithInvisiblePad (xlsx, row + 7, col, fmtLeft, QString::fromUtf8 ("Цена: "));

            xlsx.mergeCells (QXlsx::CellRange (row + 7, col + 1, row + 7, col + tagCols - 1), fmtRight);
            xlsx.write (row + 7, col + 1, QString::number (tag.getPrice ()) + " =", fmtRight);
        }
    }

    void writeSupplierRow (QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                           const TagFormats &tf)
    {
        QXlsx::Format fmtLeft  = withOuterEdges (tf.supplierFormat, true, false, false, false);
        QXlsx::Format fmtRight = withOuterEdges (tf.supplierFormat, false, true, false, false);
        const QString labelRaw =
                extractLabelFromTemplate (tagTemplate.textOrDefault (TagField::SupplierLabel), QString::fromUtf8 ("Поставщик:"));

        writeWithInvisiblePad (xlsx, row + 8, col, fmtLeft, labelRaw);
        xlsx.mergeCells (QXlsx::CellRange (row + 8, col + 1, row + 8, col + tagCols - 1), fmtRight);

        const QString val	 = tag.getSupplier ();
        const int valLeadSup = countLeadingSpacesGeneric (val);

        if (valLeadSup > 0)
            fmtRight.setIndent (qMin (15, valLeadSup));

        writeWithInvisiblePad (xlsx, row + 8, col + 1, fmtRight, val.mid (valLeadSup));
    }

    void writeAddressRows (QXlsx::Document &xlsx, int row, int col, int tagCols, const QString &line1, const QString &line2,
                           const TagFormats &tf)
    {
        {
            QXlsx::Format fmt = withOuterEdges (tf.addressFormat, true, true, false, false);
            xlsx.mergeCells (QXlsx::CellRange (row + 9, col, row + 9, col + tagCols - 1), fmt);

            const int lead = countLeadingSpacesGeneric (line1);

            if (lead > 0)
                fmt.setIndent (qMin (15, lead));

            writeWithInvisiblePad (xlsx, row + 9, col, fmt, line1.mid (lead));
        }


        {
            QXlsx::Format fmt = withOuterEdges (tf.addressFormat, true, true, false, true);
            xlsx.mergeCells (QXlsx::CellRange (row + 10, col, row + 10, col + tagCols - 1), fmt);

            const int lead = countLeadingSpacesGeneric (line2);
            if (lead > 0)
                fmt.setIndent (qMin (15, lead));

            writeWithInvisiblePad (xlsx, row + 10, col, fmt, line2.mid (lead));
        }
    }

} // namespace ExcelGen
