#include "ExcelRenderer.h"


namespace ExcelGen
{

    void renderTag (QXlsx::Document &xlsx, int row, int col, int tagCols, int tagRows, const PriceTag &tag, const TagTemplate &tagTemplate,
                    const ExcelGenerator::ExcelLayoutConfig &layoutConfig, const TagFormats &tf)
    {
        Q_UNUSED (layoutConfig);


        configureColumnsForTagWidth (xlsx, col, layoutConfig);
        setTagRowHeights (xlsx, row, tagRows, layoutConfig);

        writeCompanyHeaderRow (xlsx, row, col, tagCols, tagTemplate, tf);
        writeBrandRow (xlsx, row, col, tagCols, tag, tf);
        writeCategoryRow (xlsx, row, col, tagCols, tag, tf);
        writeBrandCountryRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
        writeManufacturingPlaceRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
        writeMaterialRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
        writeArticleRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
        writePriceRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);
        writeSupplierRow (xlsx, row, col, tagCols, tag, tagTemplate, tf);


        const auto pair = splitAddressTwoLines (tag.getAddress ());


        writeAddressRows (xlsx, row, col, tagCols, pair.first, pair.second, tf);
    }

} // namespace ExcelGen
