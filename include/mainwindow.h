#pragma once

#include <QAction>
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
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

#include "excelgenerator.h"
#include "excelparser.h"
#include "pricetag.h"
#include "templateeditor.h"
#include "templateeditordialog.h"
#include "thememanager.h"
#include "configmanager.h"
#include "wordgenerator.h"

#ifdef USE_QT_CHARTS
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#endif


class MainWindow: public QMainWindow
{
    Q_OBJECT //


            public: explicit MainWindow (QWidget *parent = nullptr);
    ~MainWindow () override;

    void setUiLanguage(const QString& lang); // Added for initial language setup
    QString localized(const QString &english, const QString &russian) const; // Manual bilingual texts


protected:
    void dragEnterEvent (QDragEnterEvent *event) override;
    void dropEvent (QDropEvent *event) override;
    void dragLeaveEvent (QDragLeaveEvent *event) override;


private slots:
    void openFile ();
    void generateDocument ();
    void processFile (const QString &filePath);
    void updatePreview ();
    void showStatistics ();


private:
    void setupUI ();
    void setupMainTab ();
    void setupPreviewTab (); // no-op (legacy)
    void setupStatisticsTab ();
    void setupToolbar ();
    void updateStatistics ();
    void updateCharts ();
    void toggleTheme ();
    void updateThemeStyles ();
    QString buildPrimaryButtonStyle (bool isDark) const;
    void updateButtonsPrimaryStyles ();
    void applyTemplateToGenerators (const TagTemplate &tpl);

    // Language helpers
    void updateLanguageTexts ();
    void toggleLanguage ();

    // Drop area styles
    void setDropAreaDefaultStyle ();
    void setDropAreaHoverStyle ();
    void setDropAreaSuccessStyle ();

    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QToolBar *mainToolbar;
    QAction *toggleThemeAction;
    QPushButton *themeButton = nullptr;
    QAction *openEditorAction = nullptr;
    QPushButton *langButton = nullptr;

    // Main tab
    QLabel *dropArea;
    QPushButton *openButton;
    QPushButton *generateButton;
    QProgressBar *progressBar;


    TemplateEditorDialog *templateEditorDialog = nullptr; // dialog instance

    QTextEdit *statisticsText;
    QPushButton *refreshStatsButton;

#ifdef USE_QT_CHARTS
    QWidget *chartsContainer = nullptr;
    QHBoxLayout *chartsLayout = nullptr;
    QChartView *brandChartView = nullptr;
    QChartView *categoryChartView = nullptr;
    QChartView *summaryBarChartView = nullptr;
#endif

    QString currentFilePath;
    ExcelParser *excelParser;
    WordGenerator *wordGenerator;
    ExcelGenerator *excelGenerator;
    QList<PriceTag> priceTags;
    QComboBox *outputFormatComboBox;

    QSettings settings;

    TagTemplate currentTemplate; // last template from editor

    // UI language: "EN" or "RU"
    QString uiLanguage = "EN";
};
