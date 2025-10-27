#pragma once

#include <xlsxdocument.h>
#include <xlsxcellrange.h>

#include "ui/tagtemplate.h"
#include "models/pricetag.h"
#include "ExcelFormats.h"
#include "ExcelUtils.h"

namespace ExcelGen
{

void writeCompanyHeaderRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const TagTemplate &tagTemplate,
                           const TagFormats &tf);
void writeBrandRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf);
void writeCategoryRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagFormats &tf);
void writeBrandCountryRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                          const TagFormats &tf);
void writeManufacturingPlaceRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                                const TagFormats &tf);
void writeMaterialRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                      const TagFormats &tf);
void writeArticleRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                     const TagFormats &tf);
void writePriceRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                   const TagFormats &tf);
void writeSupplierRow(QXlsx::Document &xlsx, int row, int col, int tagCols, const PriceTag &tag, const TagTemplate &tagTemplate,
                      const TagFormats &tf);
void writeAddressRows(QXlsx::Document &xlsx, int row, int col, int tagCols, const QString &line1, const QString &line2,
                      const TagFormats &tf);

}


