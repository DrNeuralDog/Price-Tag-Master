#pragma once


inline constexpr double points = 2.834645669; // Excel row height is in points (1/72 inch). 1 mm = 2.83465 points


inline double mmToExcelColumnWidth (double mm) { return mm / 2.4; }

inline double mmToRowHeightPt (double mm) { return mm * points; }


// Constants (for A4 Portrait)
inline constexpr double pageA4WidthMm  = 210.0;
inline constexpr double pageA4HeightMm = 297.0;
inline constexpr double excelHeaderMm  = 7.62;
inline constexpr double excelFooterMm  = 7.62;
