#pragma once

#include <xlsxdocument.h>

#include "ExcelFormats.h"
#include "ExcelLayout.h"
#include "ExcelWriters.h"

namespace ExcelGen
{

void renderTag(QXlsx::Document &xlsx, int row, int col, int tagCols, int tagRows, const PriceTag &tag,
               const TagTemplate &tagTemplate, const ExcelGenerator::ExcelLayoutConfig &layoutConfig, const TagFormats &tf);

}


