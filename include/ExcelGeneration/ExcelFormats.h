#pragma once

#include <xlsxformat.h>

#include "tagtemplate.h"

namespace ExcelGen
{

struct TagFormats
{
    QXlsx::Format headerFormat;
    QXlsx::Format brandFormat;
    QXlsx::Format categoryFormat;
    QXlsx::Format brendCountryFormat;
    QXlsx::Format developCountryFormat;
    QXlsx::Format materialHeaderFormat;
    QXlsx::Format materialValueFormat;
    QXlsx::Format articulHeaderFormat;
    QXlsx::Format articulValueFormat;
    QXlsx::Format priceFormatCell1;
    QXlsx::Format priceFormatCell2;
    QXlsx::Format strikePriceFormat;
    QXlsx::Format supplierFormat;
    QXlsx::Format addressFormat;
};

QXlsx::Format::HorizontalAlignment toQXlsxHAlign(TagTextAlign a);
void applyTextStyle(QXlsx::Format &fmt, const TagTextStyle &st);
QXlsx::Format withOuterEdges(const QXlsx::Format &base, bool left, bool right, bool top, bool bottom);
TagFormats createTagFormats(const TagTemplate &tagTemplate);

}


