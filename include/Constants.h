#pragma once


const double points = 2.834645669; // Excel row height is in points (1/72 inch). 1 mm = 2.83465 points


static double mmToExcelColumnWidth (double mm) { return mm / 2.4; }

static double mmToRowHeightPt (double mm) { return mm * points; }


// Constants (A4 Portrait)
const double pageA4WidthMm	= 210.0;
const double pageA4HeightMm = 297.0;
const double excelHeaderMm	= 7.62;
const double excelFooterMm	= 7.62;
