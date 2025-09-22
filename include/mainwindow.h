#pragma once

#include <QAction>
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

#include "configmanager.h"
#include "excelgenerator.h"
#include "excelparser.h"
#include "pricetag.h"
#include "templateeditor.h"
#include "templateeditordialog.h"
#include "thememanager.h"
#include "wordgenerator.h"

#ifdef USE_QT_CHARTS
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#endif


class MainWindow: public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow (QWidget *parent = nullptr);
    ~MainWindow () override;

    // For initial language setup
    void setUiLanguage (const QString &lang);

    // Manual bilingual texts
    QString localized (const QString &english, const QString &russian) const;


protected:
    void dragEnterEvent (QDragEnterEvent *event) override;
    void dropEvent (QDropEvent *event) override;
    void dragLeaveEvent (QDragLeaveEvent *event) override;


private slots:
    void openFile ();
    void generateDocument ();
    void processFile (const QString &filePath);
    void showStatistics ();


private:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QToolBar *mainToolbar;
    QAction *toggleThemeAction;
    QPushButton *themeButton  = nullptr;
    QAction *openEditorAction = nullptr;
    QPushButton *langButton	  = nullptr;

    // Main tab
    QLabel *dropArea;
    QPushButton *openButton;
    QPushButton *generateButton;
    QProgressBar *progressBar;

    TemplateEditorDialog *templateEditorDialog = nullptr;

    QTextEdit *statisticsText;
    QPushButton *refreshStatsButton;

    QString currentFilePath;
    ExcelParser *excelParser;
    WordGenerator *wordGenerator;
    ExcelGenerator *excelGenerator;
    QList<PriceTag> priceTags;
    QComboBox *outputFormatComboBox;

    QSettings settings;

    TagTemplate currentTemplate;

    // UI language "EN" or "RU"
    QString uiLanguage = "EN";


#ifdef USE_QT_CHARTS
    QWidget *chartsContainer		= nullptr;
    QHBoxLayout *chartsLayout		= nullptr;
    QChartView *brandChartView		= nullptr;
    QChartView *categoryChartView	= nullptr;
    QChartView *summaryBarChartView = nullptr;
#endif


    void setupUI ();
    void setupMainTab ();
    void setupStatisticsTab ();
    void setupToolbar ();

    void updateStatistics ();
    void updateCharts ();
    void updateThemeStyles ();
    void updateButtonsPrimaryStyles ();

    void toggleTheme ();

    void applyTemplateToGenerators (const TagTemplate &tpl);

    QString buildPrimaryButtonStyle (bool isDark) const;

    // Language helpers:
    void updateLanguageTexts ();
    void toggleLanguage ();

    // Drop area styles:
    void setDropAreaDefaultStyle ();
    void setDropAreaHoverStyle ();
    void setDropAreaSuccessStyle ();
};
