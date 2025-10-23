#pragma once

#include <xlsxdocument.h>

#include "Constants.h"
#include "ExcelGenerator.h"


namespace ExcelGen
{

    struct GridResult
    {
        int nCols		  = 1;
        int nRows		  = 1;
        double pageUsedMm = 0.0;
    };


    GridResult computeGrid (const ExcelGenerator::ExcelLayoutConfig &cfg);

    void configureColumnsForTagWidth (QXlsx::Document &xlsx, int col, const ExcelGenerator::ExcelLayoutConfig &layoutConfig);

    void placeTagCellRange (const ExcelGenerator::ExcelLayoutConfig &cfg, int gridCol, int gridRow, int originCol, int originRow,
                            int &startCol, int &startRow, int &tagCols, int &tagRows);

    void setTagRowHeights (QXlsx::Document &xlsx, int row, int tagRows, const ExcelGenerator::ExcelLayoutConfig &layoutConfig);

    double printableHeightMm (const ExcelGenerator::ExcelLayoutConfig &cfg);

} // namespace ExcelGen
