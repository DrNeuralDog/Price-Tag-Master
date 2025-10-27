#include "ExcelGenerator.h"

#include <QDebug>
#include <QList>
#include <QString>

#include <xlsxcellrange.h>
#include <xlsxdocument.h>
#include <xlsxworksheet.h>

#include "Constants.h"
#include "pricetag.h"

#include "ExcelFormats.h"
#include "ExcelLayout.h"
#include "ExcelRenderer.h"
#include "ExcelUtils.h"


ExcelGenerator::ExcelGenerator (QObject *parent) : QObject (parent) {}
ExcelGenerator::~ExcelGenerator () {}


bool ExcelGenerator::generateExcelDocument (const QList<PriceTag> &priceTags, const QString &outputPath)
{
    qDebug () << "Generating Excel document with" << priceTags.size () << "price tags";

    QXlsx::Document xlsx;
    ExcelGen::GridResult grid = ExcelGen::computeGrid (layoutConfig);
    const int originCol		  = 2;
    const int originRow		  = 2;


    Q_UNUSED (originCol); // kept to mirror old logic footprint
    Q_UNUSED (originRow);


    const ExcelGen::TagFormats tf = ExcelGen::createTagFormats (tagTemplate);


    // Default row height baseline (kept consistent with previous behavior)
    for (int i = 1; i <= 2000; ++i)
        xlsx.setRowHeight (i, mmToRowHeightPt (4.0));


    int tagIndex		  = 0;
    const int rowsPerPage = std::max (1, grid.nRows);


    for (const PriceTag &tag : priceTags)
    {
        const int perPage = grid.nCols * rowsPerPage;
        qDebug () << "perPage: " << perPage;


        for (int q = 0; q < tag.getQuantity (); ++q)
        {
            const int pageIdx	= tagIndex / perPage;
            const int idxInPage = tagIndex % perPage;
            const int gridRow	= idxInPage / grid.nCols;
            const int gridCol	= idxInPage % grid.nCols;


            int col = 0, row = 0, tagCols = 0, tagRows = 0;
            ExcelGen::placeTagCellRange (layoutConfig, gridCol, gridRow, originCol, originRow, col, row, tagCols, tagRows);
            const int pageGapRows = 1;

            row += pageIdx * (rowsPerPage * tagRows + pageGapRows);
            qDebug () << "idxInPage: " << idxInPage;


            if (idxInPage == perPage - 1)
            {
                const double pageH		 = ExcelGen::printableHeightMm (layoutConfig);
                const double safetyPadMm = 6.5;
                double gapMm			 = pageH - grid.pageUsedMm + safetyPadMm;

                if (gapMm < 2.)
                    gapMm = 6.5;

                xlsx.setRowHeight (row + tagRows, mmToRowHeightPt (gapMm));
            }

            qDebug () << "Creating price tag" << tagIndex << "at position (" << row << "," << col << ")";


            ExcelGen::renderTag (xlsx, row, col, tagCols, tagRows, tag, tagTemplate, layoutConfig, tf);

            tagIndex++;
        }
    }


    const QXlsx::CellRange used = xlsx.dimension ();

    if (used.isValid ())
    {
        const QString printArea = QString ("='%1'!%2").arg (xlsx.currentWorksheet ()->sheetName (), used.toString ());

        xlsx.defineName ("_xlnm.Print_Area", printArea);
    }


    bool result = xlsx.saveAs (outputPath);

    qDebug () << "Saving Excel document to:" << outputPath;
    qDebug () << "Save result:" << result;


    return result;
}
