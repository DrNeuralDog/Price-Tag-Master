#pragma once

#include <QString>
#include <QList>
#include <QFile>
#include <QXmlStreamWriter>
#include <QDir>
#include <QDebug>
#include <QSize>
#include <QSizeF>
#include <QBuffer>
#include <QPrinter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextCharFormat>

#include "pricetag.h"


class WordGenerator: public QObject
{
    Q_OBJECT //


public:
    explicit WordGenerator (QObject *parent = nullptr);
    ~WordGenerator ();

    bool generateWordDocument (const QList<PriceTag> &priceTags, const QString &outputPath);


private:
    const int pageWidth  = 11906;  // 210mm
    const int pageHeight = 16838; // 297mm

    const int tagWidth  = 3969;   // 70mm
    const int tagHeight = 2551;  // 45mm

    const int tagsPerRow = 2;    // Assuming 2 tags per row for A4 page


    void createPriceTagCell (QTextTable *table, int row, int col, const PriceTag &priceTag);
    QString formatPrice (double price);

    // Новые методы для форматирования ценников
    void addBrandInfo (QTextCursor &cursor, const PriceTag &priceTag);
    void addCategoryInfo (QTextCursor &cursor, const PriceTag &priceTag);
    void addPriceInfo (QTextCursor &cursor, const PriceTag &priceTag);
    void addProductInfo (QTextCursor &cursor, const PriceTag &priceTag);
    void addSupplierInfo (QTextCursor &cursor, const PriceTag &priceTag);

    // Утилитарные методы
    void addTextWithFormat (QTextCursor &cursor, const QString &text, const QTextCharFormat &format, bool newLine = true);
    QTextCharFormat createFormat (int fontSize, bool bold = false, const QColor &color = Qt::black);
};

