#include "ExcelLayout.h"

#include <xlsxcellrange.h>
#include <xlsxdocument.h>
#include <xlsxworksheet.h>

#include "Constants.h"
#include "ExcelUtils.h"


namespace ExcelGen
{

    GridResult computeGrid (const ExcelGenerator::ExcelLayoutConfig &cfg)
    {
        GridResult gr{};

        const double excelLeftMm			 = pageA4WidthMm * .01;
        const double excelRightMm			 = pageA4WidthMm * .01;
        const double effLeftMm				 = std::max (cfg.marginLeftMm, excelLeftMm);
        const double effRightMm				 = std::max (cfg.marginRightMm, excelRightMm);
        const double headerMm				 = excelHeaderMm;
        const double footerMm				 = excelFooterMm;
        const double effTopMm				 = cfg.marginTopMm + headerMm;
        const double effBottomMm			 = cfg.marginBottomMm + footerMm;
        const double availW					 = pageA4WidthMm - effLeftMm - effRightMm;
        const double originTopBlankMm		 = 4.0;
        const double pageH					 = pageA4HeightMm - effTopMm - effBottomMm - originTopBlankMm;
        const double effectiveHorizSpacingMm = 0.0;


        gr.nCols = std::max (
                1, static_cast<int> (std::floor ((availW + effectiveHorizSpacingMm) / (cfg.tagWidthMm + effectiveHorizSpacingMm))));


        const double rhPts[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};
        double baseTagPt	   = 0.0;


        for (double v : rhPts)
            baseTagPt += v;


        const double baseTagMm				= baseTagPt / points;
        const double tagHeightMmActual		= (cfg.tagHeightMm > 0.0) ? cfg.tagHeightMm : baseTagMm;
        const double effectiveVertSpacingMm = 0.0;
        const double minGapMm				= 1.0;
        const double denom					= tagHeightMmActual + effectiveVertSpacingMm;
        const double numerator				= std::max (0.0, pageH - minGapMm) + effectiveVertSpacingMm;

        gr.nRows	  = (denom > 0.0) ? static_cast<int> (std::floor (numerator / denom)) : 1;
        gr.nRows	  = std::max (1, gr.nRows);
        gr.pageUsedMm = gr.nRows * tagHeightMmActual;


        return gr;
    }

    double printableHeightMm (const ExcelGenerator::ExcelLayoutConfig &cfg)
    {
        Q_UNUSED (cfg);

        const double headerMm		  = excelHeaderMm;
        const double footerMm		  = excelFooterMm;
        const double effTopMm		  = cfg.marginTopMm + headerMm;
        const double effBottomMm	  = cfg.marginBottomMm + footerMm;
        const double originTopBlankMm = 4.0;


        return std::max (0.0, pageA4HeightMm - effTopMm - effBottomMm - originTopBlankMm);
    }

    void placeTagCellRange (const ExcelGenerator::ExcelLayoutConfig &cfg, int gridCol, int gridRow, int originCol, int originRow,
                            int &startCol, int &startRow, int &tagCols, int &tagRows)
    {
        Q_UNUSED (cfg);


        tagCols = 4;
        tagRows = 11;

        startCol = originCol + gridCol * tagCols;
        startRow = originRow + gridRow * tagRows;
    }

    void configureColumnsForTagWidth (QXlsx::Document &xlsx, int col, const ExcelGenerator::ExcelLayoutConfig &layoutConfig)
    {
        const double colMm[4] = {77.1, 35.7, 35.7, 27.1};
        const double sumMm	  = colMm[0] + colMm[1] + colMm[2] + colMm[3];
        const double targetMm = layoutConfig.tagWidthMm;
        const double k		  = (sumMm > 0.0) ? (targetMm / sumMm) : 1.0;
        const double w0		  = mmToExcelColumnWidth (colMm[0] * k);
        const double w1		  = mmToExcelColumnWidth (colMm[1] * k);
        const double w2		  = mmToExcelColumnWidth (colMm[2] * k);
        const double w3		  = mmToExcelColumnWidth (colMm[3] * k);

        xlsx.setColumnWidth (col + 0, w0);
        xlsx.setColumnWidth (col + 1, w1);
        xlsx.setColumnWidth (col + 2, w2);
        xlsx.setColumnWidth (col + 3, w3);
    }

    void setTagRowHeights (QXlsx::Document &xlsx, int row, int tagRows, const ExcelGenerator::ExcelLayoutConfig &layoutConfig)
    {
        const double rhPts[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};
        double basePt		   = 0.0;

        for (int i = 0; i < tagRows; ++i)
            basePt += rhPts[i];


        const double baseMm	   = basePt / points;
        const double desiredMm = layoutConfig.tagHeightMm > 0.0 ? layoutConfig.tagHeightMm : baseMm;
        const double k		   = (baseMm > 0.0) ? (desiredMm / baseMm) : 1.0;

        for (int r = 0; r < tagRows; ++r)
            xlsx.setRowHeight (row + r, rhPts[r] * k);
    }

} // namespace ExcelGen
