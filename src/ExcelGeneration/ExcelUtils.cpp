#include "ExcelUtils.h"


namespace ExcelGen
{
    QString extractLabelFromTemplate (const QString &tmpl, const QString &fallback)
    {
        const QString original = tmpl;
        const QString trimmed  = original.trimmed ();

        if (trimmed.isEmpty ())
            return fallback;

        int colon = original.indexOf (':');

        if (colon >= 0)
            return original.left (colon + 1);

        for (const QChar &ch : trimmed)
            if (ch.isLetter ())
                return original;


        return fallback;
    }

    QString preserveLeadingSpacesExcel (const QString &s)
    {
        int n = 0;

        while (n < s.size () && s.at (n) == QChar (' '))
            ++n;

        if (n == 0)
            return s;

        return QString (n, QChar (0x00A0)) + s.mid (n);
    }

    int countLeadingSpacesGeneric (const QString &s)
    {
        int n = 0;

        while (n < s.size ())
        {
            const QChar ch = s.at (n);

            if (ch == QChar (' ') || ch == QChar (0x00A0))
                ++n;
            else
                break;
        }


        return n;
    }

    void writeWithInvisiblePad (QXlsx::Document &xlsx, int row, int col, const QXlsx::Format &cellFmt, const QString &text)
    {
        const int lead = countLeadingSpacesGeneric (text);

        if (lead <= 0)
        {
            xlsx.write (row, col, text, cellFmt);

            return;
        }


        QXlsx::Format padFmt = cellFmt;
        padFmt.setFontColor (QColor (255, 255, 255));

        QXlsx::RichString rich;
        rich.addFragment (QString (lead, QChar ('*')), padFmt);
        rich.addFragment (text.mid (lead), cellFmt);
        xlsx.write (row, col, rich, cellFmt);
    }


    QString replaceLeadingSpacesWithThin (const QString &s)
    {
        int n = 0;

        while (n < s.size () && s.at (n) == QChar (' '))
            ++n;

        if (n == 0)
            return s;

        const QChar hair (0x200A);


        return QString (n, hair) + s.mid (n);
    }


    static QStringList splitIntoWordsPreservingLogic (const QString &address);
    static void fillLineWithBudget40 (const QStringList &words, int &iWord, QString &line);

    std::pair<QString, QString> splitAddressTwoLines (const QString &address)
    {
        // Original behavior: word-wrap into two lines with 40-char budget per line
        QStringList words = splitIntoWordsPreservingLogic (address);
        QString line1, line2;
        int iWord = 0;

        fillLineWithBudget40 (words, iWord, line1);
        fillLineWithBudget40 (words, iWord, line2);

        return {line1, line2};
    }

    // Helpers for splitAddressTwoLines – extracted without changing behavior
    static QStringList splitIntoWordsPreservingLogic (const QString &address)
    {
        return address.simplified ().split (' ', Qt::SkipEmptyParts);
    }

    static void fillLineWithBudget40 (const QStringList &words, int &iWord, QString &line)
    {
        const int charBudgetPerLine = 40;

        while (iWord < words.size ())
        {
            const QString &w   = words[iWord];
            int prospectiveLen = (line.isEmpty () ? 0 : line.size () + 1) + w.size ();

            if (prospectiveLen <= charBudgetPerLine)
            {
                line = line.isEmpty () ? w : line + " " + w;
                ++iWord;
            }
            else
            {
                if (line.isEmpty ())
                {
                    line = w;
                    ++iWord;
                }

                break;
            }
        }
    }

} // namespace ExcelGen
