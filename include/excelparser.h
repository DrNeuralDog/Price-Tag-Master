#pragma once

#include <QString>
#include <QList>
#include <QFile>
#include <QMap>
#include <QDebug>
#include <QFileInfo>

// Включаем QXlsx
#include "xlsxdocument.h"
#include "xlsxabstractsheet.h"
#include "xlsxcell.h"
#include "xlsxcellrange.h"

#include "pricetag.h"


class ExcelParser: public QObject
{
    Q_OBJECT //


public:
    explicit ExcelParser (QObject *parent = nullptr);
    ~ExcelParser ();

    bool parseExcelFile (const QString &filePath, QList<PriceTag> &priceTags);


private:
    // Структура для хранения маппинга колонок
    struct ColumnMapping
    {
        int nameColumn               = -1; // Фирма (бренд)
        int priceColumn              = -1; // Цена
        int quantityColumn           = -1; // Количество
        int supplierColumn           = -1; // Поставщик
        int supplierAddressColumn    = -1; // Адрес
        int categoryColumn           = -1; // Категория
        int additionalDataColumn     = -1; // Прочие данные
        int genderColumn             = -1; // Пол
        int brandCountryColumn       = -1; // Страна бренда
        int manufacturingPlaceColumn = -1; // Место производства
        int materialColumn           = -1; // Материал
        int sizeColumn               = -1; // Размер
        int articleColumn            = -1; // Артикул
        int price2Column             = -1; // Цена 2
    };


    // Методы для парсинга с использованием QXlsx
    bool findHeaders (QXlsx::Document *xlsx, const QXlsx::CellRange &range, ColumnMapping &mapping);
    int findMaxDataRow (QXlsx::Document *xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping);
    bool parseDataRow (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier, QString &currentAddress,
                       PriceTag &priceTag);

    // Валидация данных
    bool validatePriceTag (const PriceTag &priceTag);

    // Нормализация текста
    QString normalizeText (const QString &text);

    // Устаревшие методы (оставляем для совместимости, но они не используются)
    bool extractXlsxFile (const QString &filePath, const QString &extractDir);
    bool parseSharedStrings (const QString &sharedStringsPath, QMap<int, QString> &sharedStrings);
    bool parseWorksheet (const QString &worksheetPath, QMap<int, QString> &sharedStrings, QList<PriceTag> &priceTags);
    QString getColumnValue (const QString &cellRef);
    QString getColumnLetter (int columnIndex);
};
