#pragma once

#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxrichstring.h>


namespace ExcelGen
{

    QString extractLabelFromTemplate (const QString &tmpl, const QString &fallback);

    QString preserveLeadingSpacesExcel (const QString &s);

    int countLeadingSpacesGeneric (const QString &s);

    void writeWithInvisiblePad (QXlsx::Document &xlsx, int row, int col, const QXlsx::Format &cellFmt, const QString &text);

    QString replaceLeadingSpacesWithThin (const QString &s);

    std::pair<QString, QString> splitAddressTwoLines (const QString &address);

} // namespace ExcelGen
