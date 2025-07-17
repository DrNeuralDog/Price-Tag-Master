#include "excelparser.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo>

ExcelParser::ExcelParser (QObject *parent) : QObject (parent) {}

ExcelParser::~ExcelParser () {}

bool ExcelParser::parseExcelFile (const QString &filePath, QList<PriceTag> &priceTags)
{
    qDebug () << "Starting to parse Excel file:" << filePath;

    // Проверяем, что файл существует
    QFileInfo fileInfo (filePath);
    if (! fileInfo.exists ())
    {
        qDebug () << "File does not exist:" << filePath;
        return false;
    }

    // Используем QXlsx для чтения файла
    QXlsx::Document xlsx (filePath);
    if (! xlsx.load ())
    {
        qDebug () << "Failed to load XLSX file with QXlsx";
        return false;
    }

    // Получаем размеры документа
    QXlsx::CellRange range = xlsx.dimension ();
    if (! range.isValid ())
    {
        qDebug () << "Invalid document range";
        return false;
    }

    qDebug () << "Document range:" << range.toString ();

    // Ищем заголовки по всему документу
    ColumnMapping columnMapping;
    if (! findHeaders (&xlsx, range, columnMapping))
    {
        qDebug () << "Failed to find required headers";
        return false;
    }

    // Парсим данные под найденными заголовками
    QString currentSupplier;
    QString currentAddress;

    // Находим максимальную строку с данными
    int maxDataRow = findMaxDataRow (&xlsx, range, columnMapping);

    for (int row = 1; row <= maxDataRow; ++row)
    {
        PriceTag priceTag;
        if (parseDataRow (&xlsx, row, columnMapping, currentSupplier, currentAddress, priceTag))
        {
            if (validatePriceTag (priceTag))
            {
                priceTags.append (priceTag);
                qDebug () << "Added price tag:" << priceTag.getBrand () << priceTag.getCategory () << "Price:" << priceTag.getPrice ();
            }
        }
    }

    qDebug () << "Parsed" << priceTags.size () << "price tags";
    return ! priceTags.isEmpty ();
}

QString ExcelParser::normalizeText (const QString &text)
{
    // Простая нормализация для UTF-8 текста
    QString normalized = text;

    // Убираем лишние пробелы и символы
    normalized = normalized.simplified ();

    // Убираем кавычки и другие лишние символы
    normalized = normalized.remove (QRegularExpression ("^[\"']+|[\"']+$"));

    // Пробуем исправить кодировку если текст содержит кракозябры
    if (normalized.contains (QRegularExpression ("[\\x80-\\xFF]")))
    {
        // Пробуем перекодировать из CP1251 в UTF-8
        QByteArray bytes = normalized.toLocal8Bit ();
        QString decoded  = QString::fromLocal8Bit (bytes);
        if (! decoded.contains (QRegularExpression ("[\\x80-\\xFF]"))) { normalized = decoded; }
    }

    return normalized;
}

bool ExcelParser::findHeaders (QXlsx::Document *xlsx, const QXlsx::CellRange &range, ColumnMapping &mapping)
{
    qDebug () << "Searching for headers in entire document...";

    // Ожидаемые названия колонок из API документа
    QMap<QString, int*> columnMap = {
        {"Поставщик", &mapping.supplierColumn},
        {"Адрес", &mapping.supplierAddressColumn},
        {"Фирма", &mapping.nameColumn}, // Используем Фирма как бренд
        {"Категория", &mapping.categoryColumn}, // Категория товара
        {"Прочие данные", &mapping.additionalDataColumn}, // Дополнительные данные
        {"Пол", &mapping.genderColumn}, // Пол
        {"Страна бренда", &mapping.brandCountryColumn}, // Страна бренда
        {"Место производства", &mapping.manufacturingPlaceColumn}, // Место производства
        {"Материал", &mapping.materialColumn}, // Материал
        {"Размер", &mapping.sizeColumn}, // Размер
        {"Артикул", &mapping.articleColumn}, // Артикул
        {"Цена", &mapping.priceColumn},
        {"Цена 2", &mapping.price2Column}, // Цена 2
        {"Количество", &mapping.quantityColumn}};

    // Инициализируем все колонки как -1 (не найдены)
    mapping.nameColumn               = -1;
    mapping.priceColumn              = -1;
    mapping.quantityColumn           = -1;
    mapping.supplierColumn           = -1;
    mapping.supplierAddressColumn    = -1;
    mapping.categoryColumn           = -1;
    mapping.additionalDataColumn     = -1;
    mapping.genderColumn             = -1;
    mapping.brandCountryColumn       = -1;
    mapping.manufacturingPlaceColumn = -1;
    mapping.materialColumn           = -1;
    mapping.sizeColumn               = -1;
    mapping.articleColumn            = -1;
    mapping.price2Column             = -1;

    // Ищем заголовки по всему документу
    for (int row = range.firstRow (); row <= range.lastRow (); ++row)
    {
        for (int col = range.firstColumn (); col <= range.lastColumn (); ++col)
        {
            QVariant cellValue = xlsx->read (row, col);
            if (cellValue.isValid ())
            {
                QString cellText           = cellValue.toString ().trimmed ();
                QString normalizedCellText = normalizeText (cellText);

                // Отладочная информация для всех ячеек
                if (! cellText.isEmpty ())
                {
                    qDebug () << "Cell at row" << row << "col" << col << ":" << cellText;
                    qDebug () << "Normalized:" << normalizedCellText;

                    // Дополнительная отладка кодировки
                    if (normalizedCellText.contains (QRegularExpression ("[\\x80-\\xFF]")))
                    {
                        qDebug () << "Contains encoding issues, trying to fix...";
                        QByteArray bytes = cellText.toLocal8Bit ();
                        QString decoded  = QString::fromLocal8Bit (bytes);
                        qDebug () << "Decoded:" << decoded;
                    }
                }

                // Ищем совпадение с ожидаемыми заголовками
                for (auto it = columnMap.begin (); it != columnMap.end (); ++it)
                {
                    QString expectedHeader = it.key ();

                    if (normalizedCellText.compare (expectedHeader, Qt::CaseInsensitive) == 0)
                    {
                        *(it.value ()) = col;
                        qDebug () << "Found header '" << expectedHeader << "' at row" << row << "column" << col;
                        break;
                    }
                }
            }
        }
    }

    // Проверяем обязательные колонки
    if (mapping.nameColumn == -1 || mapping.priceColumn == -1)
    {
        qDebug () << "Missing required columns: Фирма or Цена";
        qDebug () << "Фирма column:" << mapping.nameColumn << "Цена column:" << mapping.priceColumn;
        return false;
    }

    qDebug () << "Header search completed successfully";
    return true;
}

int ExcelParser::findMaxDataRow (QXlsx::Document *xlsx, const QXlsx::CellRange &range, const ColumnMapping &mapping)
{
    int maxRow = range.firstRow ();

    // Ищем последнюю строку с данными в обязательных колонках
    for (int row = range.firstRow (); row <= range.lastRow (); ++row)
    {
        bool hasData = false;

        // Проверяем название
        if (mapping.nameColumn > 0)
        {
            QVariant cellValue = xlsx->read (row, mapping.nameColumn);
            if (cellValue.isValid () && ! cellValue.toString ().trimmed ().isEmpty ()) { hasData = true; }
        }

        // Проверяем цену
        if (mapping.priceColumn > 0)
        {
            QVariant cellValue = xlsx->read (row, mapping.priceColumn);
            if (cellValue.isValid () && ! cellValue.toString ().trimmed ().isEmpty ()) { hasData = true; }
        }

        if (hasData) { maxRow = row; }
    }

    qDebug () << "Max data row found:" << maxRow;
    return maxRow;
}

bool ExcelParser::parseDataRow (QXlsx::Document *xlsx, int row, const ColumnMapping &mapping, QString &currentSupplier,
                                QString &currentAddress, PriceTag &priceTag)
{
    bool hasData = false;

    // Читаем бренд (Фирма)
    if (mapping.nameColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.nameColumn);
        if (cellValue.isValid ())
        {
            QString brand = cellValue.toString ().trimmed ();
            if (! brand.isEmpty ())
            {
                priceTag.setBrand (brand);
                hasData = true;
            }
        }
    }

    // Читаем цену
    if (mapping.priceColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.priceColumn);
        if (cellValue.isValid ())
        {
            QString priceStr = cellValue.toString ().trimmed ();
            if (! priceStr.isEmpty ())
            {
                // Убираем символы валюты и пробелы
                priceStr.remove (QRegularExpression ("[^0-9.,]"));
                priceStr.replace (",", ".");
                double price = priceStr.toDouble ();
                if (price > 0)
                {
                    priceTag.setPrice (price);
                    hasData = true;
                }
            }
        }
    }

    // Если нет основных данных, пропускаем строку
    if (! hasData) { return false; }

    // Читаем количество
    if (mapping.quantityColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.quantityColumn);
        if (cellValue.isValid ())
        {
            bool ok;
            int quantity = cellValue.toInt (&ok);
            if (ok && quantity > 0) { priceTag.setQuantity (quantity); }
            else
            {
                priceTag.setQuantity (1); // По умолчанию
            }
        }
        else
        {
            priceTag.setQuantity (1); // По умолчанию
        }
    }
    else
    {
        priceTag.setQuantity (1); // По умолчанию
    }

    // Читаем поставщика
    if (mapping.supplierColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.supplierColumn);
        if (cellValue.isValid ())
        {
            QString supplier = cellValue.toString ().trimmed ();
            if (! supplier.isEmpty ()) { currentSupplier = supplier; }
        }
    }
    priceTag.setSupplier (currentSupplier);

    // Читаем адрес поставщика
    if (mapping.supplierAddressColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.supplierAddressColumn);
        if (cellValue.isValid ())
        {
            QString address = cellValue.toString ().trimmed ();
            if (! address.isEmpty ()) { currentAddress = address; }
        }
    }
    priceTag.setAddress (currentAddress);

    // Читаем категорию
    if (mapping.categoryColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.categoryColumn);
        if (cellValue.isValid ())
        {
            QString category = cellValue.toString ().trimmed ();
            if (! category.isEmpty ()) { priceTag.setCategory (category); }
        }
    }

    // Читаем дополнительные данные
    if (mapping.additionalDataColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.additionalDataColumn);
        if (cellValue.isValid ())
        {
            QString data = cellValue.toString ().trimmed ();
            if (! data.isEmpty ()) { priceTag.setAdditionalData (data); }
        }
    }

    // Читаем пол
    if (mapping.genderColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.genderColumn);
        if (cellValue.isValid ())
        {
            QString gender = cellValue.toString ().trimmed ();
            if (! gender.isEmpty ()) { priceTag.setGender (gender); }
        }
    }

    // Читаем страну бренда
    if (mapping.brandCountryColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.brandCountryColumn);
        if (cellValue.isValid ())
        {
            QString country = cellValue.toString ().trimmed ();

            if (! country.isEmpty ()) { priceTag.setBrandCountry (country); }
        }
    }

    // Читаем место производства
    if (mapping.manufacturingPlaceColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.manufacturingPlaceColumn);
        if (cellValue.isValid ())
        {
            QString place = cellValue.toString ().trimmed ();
            if (! place.isEmpty ()) { priceTag.setManufacturingPlace (place); }
        }
    }

    // Читаем материал
    if (mapping.materialColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.materialColumn);
        if (cellValue.isValid ())
        {
            QString material = cellValue.toString ().trimmed ();
            if (! material.isEmpty ()) { priceTag.setMaterial (material); }
        }
    }

    // Читаем размер
    if (mapping.sizeColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.sizeColumn);
        if (cellValue.isValid ())
        {
            QString size = cellValue.toString ().trimmed ();
            if (! size.isEmpty ()) { priceTag.setSize (size); }
        }
    }

    // Читаем артикул
    if (mapping.articleColumn > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.articleColumn);
        if (cellValue.isValid ())
        {
            QString article = cellValue.toString ().trimmed ();
            if (! article.isEmpty ()) { priceTag.setArticle (article); }
        }
    }

    // Читаем цену 2
    if (mapping.price2Column > 0)
    {
        QVariant cellValue = xlsx->read (row, mapping.price2Column);
        if (cellValue.isValid ())
        {
            QString priceStr = cellValue.toString ().trimmed ();
            if (! priceStr.isEmpty ())
            {
                // Убираем символы валюты и пробелы
                priceStr.remove (QRegularExpression ("[^0-9.,]"));
                priceStr.replace (",", ".");
                double price2 = priceStr.toDouble ();
                if (price2 > 0) { priceTag.setPrice2 (price2); }
            }
        }
    }

    return true;
}

bool ExcelParser::validatePriceTag (const PriceTag &priceTag)
{
    if (priceTag.getBrand ().isEmpty ())
    {
        qDebug () << "Price tag validation failed: empty brand";
        return false;
    }

    if (priceTag.getPrice () <= 0)
    {
        qDebug () << "Price tag validation failed: invalid price" << priceTag.getPrice ();
        return false;
    }

    if (priceTag.getQuantity () <= 0)
    {
        qDebug () << "Price tag validation failed: invalid quantity" << priceTag.getQuantity ();
        return false;
    }

    return true;
}

// Удаляем старые методы, которые больше не нужны
bool ExcelParser::extractXlsxFile (const QString &filePath, const QString &extractDir)
{
    // Этот метод больше не используется
    return false;
}

bool ExcelParser::parseSharedStrings (const QString &sharedStringsPath, QMap<int, QString> &sharedStrings)
{
    // Этот метод больше не используется
    return false;
}

bool ExcelParser::parseWorksheet (const QString &worksheetPath, QMap<int, QString> &sharedStrings, QList<PriceTag> &priceTags)
{
    // Этот метод больше не используется
    return false;
}

QString ExcelParser::getColumnValue (const QString &cellRef)
{
    // Этот метод больше не используется
    return QString ();
}

QString ExcelParser::getColumnLetter (int columnIndex)
{
    // Этот метод больше не используется
    return QString ();
} 
