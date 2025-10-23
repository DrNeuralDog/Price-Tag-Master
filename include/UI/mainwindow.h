#pragma once

#include "CommonIncludes.h"

#include "ExcelGenerator.h"
#include "ExcelParser.h"
#include "WordGenerator.h"
#include "configmanager.h"
#include "templateeditor.h"
#include "templateeditordialog.h"
#include "thememanager.h"


struct StatisticsData
{
    int totalProducts		  = 0;
    int totalTags			  = 0;
    int productsWithDiscount  = 0;
    double totalValue		  = 0.0;
    double totalDiscountValue = 0.0;
    QMap<QString, int> brandCount;
    QMap<QString, int> categoryCount;
    QSet<QString> suppliers;
};


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
    void dragMoveEvent (QDragMoveEvent *event) override;
    void dropEvent (QDropEvent *event) override;
    void dragLeaveEvent (QDragLeaveEvent *event) override;
    bool eventFilter (QObject *obj, QEvent *event) override;


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
    QToolButton *gearButton	  = nullptr;

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

    // Supported languages ​​"EN" or "RU"
    QString uiLanguage = "EN";


#ifdef USE_QT_CHARTS
    QWidget *chartsContainer	 = nullptr;
    QHBoxLayout *chartsLayout	 = nullptr;
    QWidget *brandChartView		 = nullptr;
    QWidget *categoryChartView	 = nullptr;
    QWidget *summaryBarChartView = nullptr;
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


    // UI helpers
    int mmToPxX (double mm) const;
    int mmToPxY (double mm) const;


    // Toolbar helpers
    void initThemeButtonWithWrapper (int twoMmPx);

    void addToolbarLeftPadding ();
    void addLangButtonWithWrapper (int twoMmPx);
    void addToolbarExpandingSpacer ();

    void setupOpenEditorAction ();
    void setupGearToolButton ();


    // Editor helpers
    QIcon createGearIcon () const;

    void openTemplateEditor ();

    void connectTemplateEditorSignals (TemplateEditorDialog *editor);


    // Drag-n-Drop helpers
    bool mimeHasXlsx (const QMimeData *mime) const;

    QString firstXlsxFromMime (const QMimeData *mime) const;

    QPoint mapGlobalToDropArea (const QPoint &globalPos) const;

    bool isInsideDropArea (const QPoint &posInDrop) const;

    void updateDropVisualOnEnter (const QPoint &posInDrop, QDragEnterEvent *event);
    void updateDropVisualOnMove (const QPoint &posInDrop, QDragMoveEvent *event);


    // Statistics helpers
    StatisticsData aggregateStatistics () const;
    QString formatStatisticsText (const StatisticsData &data) const;
    QString buildStatisticsText () const;


#ifdef USE_QT_CHARTS
    // Charts helpers - declarations avoid exposing QtCharts types in signatures
    void clearChartsLayout ();
    void aggregateChartData (QMap<QString, int> &brandCount, QMap<QString, int> &categoryCount, int &totalProducts, int &totalTags,
                             int &productsWithDiscount) const;
    void buildBrandChart (const QMap<QString, int> &brandCount);
    void buildCategoryChart (const QMap<QString, int> &categoryCount);
    void buildSummaryBarChart (int totalProducts, int totalTags, int productsWithDiscount);
#endif


    // Theme styling helpers
    void styleToolbarFrame (int twoMmPx, const QString &borderColor);
    void styleThemeButton (bool isDark);
    void styleLanguageButton (bool isDark);

    void updateGearButtonIconAndSize (bool isDark);


    template <typename ConfigType> void copyTemplateGeometryToConfig (const TagTemplate &tagTemplate, ConfigType &generatorConfig) const;
};


// Template implementation must be available to all translation units
template <typename ConfigType>
void MainWindow::copyTemplateGeometryToConfig (const TagTemplate &tagTemplate, ConfigType &generatorConfig) const
{
    generatorConfig.tagWidthMm	   = tagTemplate.tagWidthMm;
    generatorConfig.tagHeightMm	   = tagTemplate.tagHeightMm;
    generatorConfig.marginLeftMm   = tagTemplate.marginLeftMm;
    generatorConfig.marginTopMm	   = tagTemplate.marginTopMm;
    generatorConfig.marginRightMm  = tagTemplate.marginRightMm;
    generatorConfig.marginBottomMm = tagTemplate.marginBottomMm;
    generatorConfig.spacingHMm	   = tagTemplate.spacingHMm;
    generatorConfig.spacingVMm	   = tagTemplate.spacingVMm;
}
