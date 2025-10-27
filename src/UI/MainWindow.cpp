#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

#ifdef USE_QT_CHARTS
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>

QT_BEGIN_NAMESPACE
namespace QtCharts {}
QT_END_NAMESPACE
using namespace QtCharts;
#endif

#include "ExcelGenerator.h"
#include "ExcelParser.h"
#include "WordGenerator.h"
#include "configmanager.h"
#include "pixmaputils.h"
#include "pricetag.h"
#include "templateeditor.h"
#include "templateeditordialog.h"
#include "thememanager.h"
#include "trimmedhittoolbutton.h"


MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
{
    excelParser	   = new ExcelParser (this);
    wordGenerator  = new WordGenerator (this);
    excelGenerator = new ExcelGenerator (this);

    setupUI ();
    setupToolbar ();

    // Load template configuration (or save defaults if missing)
    {
        TagTemplate loaded;

        if (ConfigManager::loadTemplate (loaded))
            currentTemplate = loaded;
        else // Save current defaults to file
            ConfigManager::saveTemplate (currentTemplate);

        applyTemplateToGenerators (currentTemplate);
    }

    setAcceptDrops (true);
    updateThemeStyles ();
}

MainWindow::~MainWindow () {}


bool MainWindow::eventFilter (QObject *obj, QEvent *event)
{
    if (obj == gearButton && event &&
        (event->type () == QEvent::MouseButtonPress || event->type () == QEvent::MouseButtonRelease ||
         event->type () == QEvent::MouseButtonDblClick))
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint pos = static_cast<QMouseEvent *> (event)->position ().toPoint ();
#else
        const QPoint pos = static_cast<QMouseEvent *> (event)->pos ();
#endif

        // Trim 2mm on left and right from clickable zone
        const int twoMmPx = qRound (this->logicalDpiX () * 2.0 / 25.4);
        QRect allowed	  = gearButton->rect ().adjusted (twoMmPx, 0, -twoMmPx, 0);


        // Ignore click outside allowed rect
        if (! allowed.contains (pos))
            return true;
    }


    return QMainWindow::eventFilter (obj, event);
}


void MainWindow::initThemeButtonWithWrapper (int twoMmPx)
{
    themeButton = new QPushButton (this);

    themeButton->setCursor (Qt::PointingHandCursor);
    themeButton->setMinimumHeight (28);
    themeButton->setMinimumWidth (84);
    themeButton->setText (ThemeManager::currentTheme () == AppTheme::Dark ? tr ("Dark") : tr ("Light"));

    connect (themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);


    QWidget *themeWrap			 = new QWidget (this);
    QVBoxLayout *themeWrapLayout = new QVBoxLayout (themeWrap);


    themeWrapLayout->setContentsMargins (0, twoMmPx, 0, twoMmPx);
    themeWrapLayout->setSpacing (0);
    themeWrapLayout->addWidget (themeButton);

    mainToolbar->addWidget (themeWrap);
}


int MainWindow::mmToPxX (double mm) const { return qRound (this->logicalDpiX () * mm / 25.4); }
int MainWindow::mmToPxY (double mm) const { return qRound (this->logicalDpiY () * mm / 25.4); }


void MainWindow::addToolbarLeftPadding ()
{
    QWidget *leftPad = new QWidget (this);

    leftPad->setFixedWidth (qRound (this->logicalDpiX () * 1.0 / 25.4));
    mainToolbar->addWidget (leftPad);
}

void MainWindow::addLangButtonWithWrapper (int twoMmPx)
{
    QWidget *langSpacer = new QWidget (this);

    langSpacer->setFixedWidth (10);

    mainToolbar->addWidget (langSpacer);

    langButton = new QPushButton (this);
    langButton->setObjectName ("lang");
    langButton->setCursor (Qt::PointingHandCursor);
    langButton->setFixedHeight (28);
    langButton->setMinimumWidth (40);
    langButton->setMaximumWidth (40);

    uiLanguage = "EN";

    langButton->setText (uiLanguage);
    connect (langButton, &QPushButton::clicked, this, &MainWindow::toggleLanguage);

    QWidget *langWrap			= new QWidget (this);
    QVBoxLayout *langWrapLayout = new QVBoxLayout (langWrap);

    langWrapLayout->setContentsMargins (0, twoMmPx, 0, twoMmPx);
    langWrapLayout->setSpacing (0);
    langWrapLayout->addWidget (langButton);

    mainToolbar->addWidget (langWrap);
}

void MainWindow::addToolbarExpandingSpacer ()
{
    QWidget *spacer = new QWidget (this);

    spacer->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainToolbar->addWidget (spacer);
}


void MainWindow::setupUI ()
{
    centralWidget = new QWidget (this);
    setCentralWidget (centralWidget);


    QVBoxLayout *mainLayout = new QVBoxLayout (centralWidget);


    tabWidget = new QTabWidget (this);
    mainLayout->addWidget (tabWidget);

    setupMainTab ();

    setupStatisticsTab ();
}

void MainWindow::setupToolbar ()
{
    mainToolbar = addToolBar (tr ("Toolbar"));
    mainToolbar->setMovable (false);
    mainToolbar->setIconSize (QSize (20, 20));
    mainToolbar->setContentsMargins (0, 0, 0, 0);

    const int twoMmPx = qRound (this->logicalDpiY () * 2.0 / 25.4);

    addToolbarLeftPadding ();
    initThemeButtonWithWrapper (twoMmPx);
    addLangButtonWithWrapper (twoMmPx);
    addToolbarExpandingSpacer ();
    setupOpenEditorAction ();
    setupGearToolButton ();
}

void MainWindow::setupOpenEditorAction ()
{
    openEditorAction = new QAction (createGearIcon (), QString (), this);

    connect (openEditorAction, &QAction::triggered, this, &MainWindow::openTemplateEditor);
}

void MainWindow::setupGearToolButton ()
{
    TrimmedHitToolButton *btn = new TrimmedHitToolButton (this);

    btn->setDefaultAction (openEditorAction);
    btn->setAutoRaise (true);
    btn->setToolButtonStyle (Qt::ToolButtonIconOnly);

    const int diameter = 25;

    btn->setFixedSize (diameter, diameter);
    btn->setIconSize (QSize (diameter, diameter));
    btn->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setCursor (Qt::PointingHandCursor);
    btn->setStyleSheet ("QToolButton { background: transparent; border: none; padding: 0px; }"
                        "QToolButton:hover { background: transparent; }");
    btn->setContentsMargins (0, 0, 0, 0);

    const int fourMmPx = 0;

    btn->setHorizontalTrimPx (fourMmPx);

    PixmapUtils::applyCircularMask (btn);
    mainToolbar->addWidget (btn);


    QWidget *rightPad = new QWidget (this);

    rightPad->setFixedWidth (qRound (this->logicalDpiX () * 3.0 / 25.4));

    mainToolbar->addWidget (rightPad);

    gearButton = btn;
}

void MainWindow::setupMainTab ()
{
    QWidget *mainTab		   = new QWidget ();
    QVBoxLayout *mainTabLayout = new QVBoxLayout (mainTab);

    dropArea = new QLabel (tr ("Drag and drop Excel file here or use the Open button below"));
    dropArea->setAlignment (Qt::AlignCenter);
    dropArea->setMinimumHeight (200);
    dropArea->setStyleSheet ("QLabel { border: 2px dashed #CBD5E1; border-radius: 4px; background: rgba(59,130,246,0.08); }");

    openButton	   = new QPushButton (tr ("Open Excel File"));
    generateButton = new QPushButton (QIcon (":/sprites/tag.png"), tr ("  Generate Price Tags"));
    generateButton->setIconSize (QSize (18, 18));

    generateButton->setEnabled (false);

    progressBar = new QProgressBar ();
    progressBar->setVisible (false);

    mainTabLayout->addWidget (dropArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout ();
    buttonLayout->addWidget (openButton);
    buttonLayout->addWidget (generateButton);

    mainTabLayout->addLayout (buttonLayout);

    outputFormatComboBox = new QComboBox (this);
    outputFormatComboBox->addItem (tr ("XLSX")); // 0 = XLSX
    outputFormatComboBox->addItem (tr ("DOCX")); // 1 = DOCX
    outputFormatComboBox->setCurrentIndex (0);	 // Default to XLSX
    mainTabLayout->addWidget (outputFormatComboBox);

    mainTabLayout->addWidget (progressBar);

    connect (openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect (generateButton, &QPushButton::clicked, this, &MainWindow::generateDocument);

    tabWidget->addTab (mainTab, tr ("Main"));
}

void MainWindow::setupStatisticsTab ()
{
    QWidget *statsTab		 = new QWidget ();
    QVBoxLayout *statsLayout = new QVBoxLayout (statsTab);

    statisticsText = new QTextEdit ();
    statisticsText->setReadOnly (true);

    refreshStatsButton = new QPushButton (tr ("Refresh Statistics"));
    refreshStatsButton->setEnabled (false);

#ifdef USE_QT_CHARTS
    // Charts container at the top
    chartsContainer = new QWidget (statsTab);
    chartsLayout	= new QHBoxLayout (chartsContainer);

    chartsLayout->setContentsMargins (0, 0, 0, 0);
    chartsLayout->setSpacing (8);
    statsLayout->addWidget (chartsContainer, 1);
#endif

    // Text statistics below charts
    statsLayout->addWidget (statisticsText, 1);
    statsLayout->addWidget (refreshStatsButton);

    connect (refreshStatsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);

    tabWidget->addTab (statsTab, tr ("Statistics"));
}


void MainWindow::dragEnterEvent (QDragEnterEvent *event)
{
    if (! event || ! event->mimeData () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
    {
        if (event)
            event->ignore ();

        return;
    }

    if (! mimeHasXlsx (event->mimeData ()))
    {
        event->ignore ();

        return;
    }

    // Accept drag immediately for valid data
    event->acceptProposedAction ();

    // Provide visual hint only when actually over the drop area
    const QPoint posInDrop = mapGlobalToDropArea (this->mapToGlobal (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
                    ));
    updateDropVisualOnEnter (posInDrop, event);
}

void MainWindow::dragMoveEvent (QDragMoveEvent *event)
{
    if (! event || ! event->mimeData () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
    {
        if (event)
            event->ignore ();

        return;
    }

    if (! mimeHasXlsx (event->mimeData ()))
    {
        event->ignore ();

        return;
    }


    const QPoint posInDrop = mapGlobalToDropArea (this->mapToGlobal (

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
                    ));
    updateDropVisualOnMove (posInDrop, event);
}

void MainWindow::dropEvent (QDropEvent *event)
{
    if (! event || ! event->mimeData () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
        return;

    const QPoint posInDrop = mapGlobalToDropArea (this->mapToGlobal (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
                    ));

    if (! isInsideDropArea (posInDrop))
        return;


    const QString path = firstXlsxFromMime (event->mimeData ());
    if (! path.isEmpty ())
    {
        processFile (path);

        event->acceptProposedAction ();
    }
}

void MainWindow::dragLeaveEvent (QDragLeaveEvent *event)
{
    Q_UNUSED (event);

    setDropAreaDefaultStyle ();
}


QString MainWindow::buildStatisticsText () const { return formatStatisticsText (aggregateStatistics ()); }

QString MainWindow::buildPrimaryButtonStyle (bool isDark) const
{
    const QString baseGrad	= isDark ? "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7C3AED, stop:1 #DB2777)"
                                     : "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #2563EB, stop:1 #22C55E)";
    const QString hoverGrad = isDark ? "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #6D28D9, stop:1 #BE185D)"
                                     : "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1D4ED8, stop:1 #16A34A)";
    const QString pressGrad = isDark ? "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #5B21B6, stop:1 #9D174D)"
                                     : "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1E40AF, stop:1 #15803D)";

    return QString ("QPushButton { border: none; border-radius: 12px; padding: 6px 14px; color: white; background: %1; }"
                    "QPushButton:hover { background: %2; }"
                    "QPushButton:pressed { background: %3; padding-top: 7px; padding-bottom: 5px; }")
            .arg (baseGrad, hoverGrad, pressGrad);
}


void MainWindow::updateDropVisualOnEnter (const QPoint &posInDrop, QDragEnterEvent *event)
{
    Q_UNUSED (event);


    if (isInsideDropArea (posInDrop))
        setDropAreaHoverStyle ();
    else
        setDropAreaDefaultStyle ();
}

void MainWindow::updateDropVisualOnMove (const QPoint &posInDrop, QDragMoveEvent *event)
{
    if (isInsideDropArea (posInDrop))
    {
        event->acceptProposedAction ();
        setDropAreaHoverStyle ();
    }
    else
    {
        event->setDropAction (Qt::IgnoreAction);
        event->accept ();
        setDropAreaDefaultStyle ();
    }
}

void MainWindow::updateThemeStyles ()
{
    const bool isDark		  = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString borderColor = isDark ? "#475569" : "#E2E8F0";
    const QString subtleBg	  = isDark ? "#1F2937" : "#F1F5F9";

    Q_UNUSED (borderColor);
    Q_UNUSED (subtleBg);


    setDropAreaDefaultStyle ();

    if (mainToolbar)
    {
        const int twoMmPx = qRound (this->logicalDpiY () * 2.0 / 25.4);

        styleToolbarFrame (twoMmPx, borderColor);
    }


    // Theme button style + text - neutral without gradient
    if (themeButton)
        styleThemeButton (isDark);

    // Language button neutral style - adjusted for square
    if (langButton)
        styleLanguageButton (isDark);

    // Editor gear icon per theme - use cropped pixmap to maximize visible area
    if (openEditorAction)
        updateGearButtonIconAndSize (isDark);


    updateButtonsPrimaryStyles ();
}

void MainWindow::updateGearButtonIconAndSize (bool isDark)
{
    Q_UNUSED (isDark);


    openEditorAction->setIcon (createGearIcon ());

    if (gearButton)
    {
        const int diameter = 25;

        gearButton->setFixedSize (diameter, diameter);
        gearButton->setIconSize (QSize (diameter, diameter));

        PixmapUtils::applyCircularMask (gearButton);
    }
}

void MainWindow::updateButtonsPrimaryStyles ()
{
    const bool isDark		   = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString primaryStyle = buildPrimaryButtonStyle (isDark);


    // Enforce equal heights regardless of icon/text paddings - match previous Generate visual height
    const int baseHeight = isDark ? 36 : 38;


    if (openButton)
    {
        openButton->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
        openButton->setFixedHeight (baseHeight);
    }

    if (generateButton)
    {
        generateButton->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
        generateButton->setFixedHeight (baseHeight);
        generateButton->setStyleSheet (primaryStyle);
    }

    if (refreshStatsButton)
    {
        refreshStatsButton->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
        refreshStatsButton->setFixedHeight (baseHeight);

        if (refreshStatsButton->isEnabled ())
            refreshStatsButton->setStyleSheet (primaryStyle);
        else
        {
            // Neutral disabled style - not colored
            refreshStatsButton->setStyleSheet (
                    QString ("QPushButton { border: 1px solid %1; border-radius: 12px; padding: 6px 14px; background: %2; color: %3; }")
                            .arg (isDark ? "#374151" : "#E5E7EB", isDark ? "#111827" : "#FFFFFF", isDark ? "#9CA3AF" : "#6B7280"));
        }
    }
}

void MainWindow::updateLanguageTexts ()
{
    if (themeButton)
        themeButton->setText (ThemeManager::currentTheme () == AppTheme::Dark ? localized ("Dark", "Тёмная")
                                                                              : localized ("Light", "Светлая"));
    if (langButton)
        langButton->setText (uiLanguage);

    if (openButton)
        openButton->setText (localized ("Open Excel File", "Открыть файл Excel"));

    if (generateButton)
        generateButton->setText (localized ("  Generate Price Tags", "  Сгенерировать ценники"));

    if (refreshStatsButton)
        refreshStatsButton->setText (localized ("Refresh Statistics", "Обновить статистику"));


    if (templateEditorDialog)
        templateEditorDialog->applyLanguage (uiLanguage);

    if (tabWidget)
    {
        int idxMain = tabWidget->indexOf (tabWidget->widget (0));
        if (idxMain >= 0)
            tabWidget->setTabText (idxMain, localized ("Main", "Основное"));

        int idxStats = tabWidget->indexOf (tabWidget->widget (tabWidget->count () - 1));
        if (idxStats >= 0)
            tabWidget->setTabText (idxStats, localized ("Statistics", "Статистика"));
    }

    if (dropArea)
        dropArea->setText (localized ("Drag and drop Excel file here or use the Open button below",
                                      "Перетащите файл Excel сюда или используйте кнопку Открыть ниже"));
}


void MainWindow::setDropAreaDefaultStyle ()
{
    const bool isDark		  = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString borderColor = isDark ? "#475569" : "#CBD5E1"; // slightly darker neutral
    const QString subtleBg	  = isDark ? "#1F2937" : "#E5E7EB"; // slightly grayer to stand out in light theme

    dropArea->setStyleSheet (
            QString ("QLabel { border: 2px dashed %1; border-radius: 12px; background: %2; }").arg (borderColor, subtleBg));
}

void MainWindow::setDropAreaHoverStyle ()
{
    const bool isDark	 = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString border = "#2563EB"; // blue-600
    const QString bg	 = isDark ? "rgba(96,165,250,0.20)" : "#DBEAFE";

    dropArea->setStyleSheet (QString ("QLabel { border: 2px dashed %1; border-radius: 12px; background: %2; }").arg (border, bg));
}

void MainWindow::setDropAreaSuccessStyle ()
{
    const bool isDark	 = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString border = "#10B981"; // green
    const QString bg	 = isDark ? "rgba(52,211,153,0.18)" : "#D1FAE5";

    dropArea->setStyleSheet (QString ("QLabel { border: 2px dashed %1; border-radius: 12px; background: %2; }").arg (border, bg));
}

void MainWindow::setUiLanguage (const QString &lang)
{
    uiLanguage = lang;
    updateLanguageTexts ();
}


void MainWindow::styleToolbarFrame (int twoMmPx, const QString &borderColor)
{
    mainToolbar->setStyleSheet (QString ("QToolBar { background: palette(window); border: none; border-bottom: 1px solid %1; } "
                                         "QToolBar QToolButton { margin-top: %2px; margin-bottom: %2px; padding: 0px; border: none; "
                                         "background: transparent; } "
                                         "QToolBar QToolButton:hover { background: transparent; } ")
                                        .arg (borderColor)
                                        .arg (twoMmPx));
}

void MainWindow::styleThemeButton (bool isDark)
{
    themeButton->setText (isDark ? localized ("Dark", "Тёмная") : localized ("Light", "Светлая"));

    const QString tbStyle =
            QString ("QPushButton { border: 1px solid %1; border-radius: 14px; padding: 4px 12px; color: %2; background: %3; }") +
            QString ("QPushButton:hover { background: %4; }") +
            QString ("QPushButton:pressed { padding-top: 5px; padding-bottom: 3px; }")
                    .arg (isDark ? "#475569" : "#CBD5E1", isDark ? "#E5E7EB" : "#111827", isDark ? "#1F2937" : "#FFFFFF",
                          isDark ? "#111827" : "#F3F4F6");

    themeButton->setStyleSheet (tbStyle);
}

void MainWindow::styleLanguageButton (bool isDark)
{
    // Force exact geometry and remove horizontal padding by setting direct styles on the widget
    const QString lbStyleBase =
            QString ("min-width: 40px; max-width: 40px; min-height: 28px; max-height: 28px; height: 28px; "
                     "padding-left: 0px; padding-right: 0px; padding-top: 0px; padding-bottom: 0px; "
                     "border: 1px solid %1; border-radius: 14px; background: %2; color: %3; font-weight: 600; ")
                    .arg (isDark ? "#475569" : "#CBD5E1", isDark ? "#1F2937" : "#FFFFFF", isDark ? "#E5E7EB" : "#111827");

    const QString lbStyleHover = QString ("QPushButton:hover { background: %1; }").arg (isDark ? "#111827" : "#F3F4F6");
    const QString lbStylePress = QString ("QPushButton:pressed { padding-top: 1px; }");
    const QString lbStyle	   = lbStyleBase + lbStyleHover + lbStylePress;

    langButton->setStyleSheet (lbStyle);
    langButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    langButton->setFixedSize (40, 28);
    langButton->setText (uiLanguage);
}


void MainWindow::applyTemplateToGenerators (const TagTemplate &tpl)
{
    // Layout to generators from template geometry
    WordGenerator::DocxLayoutConfig wcfg = wordGenerator->layout ();

    copyTemplateGeometryToConfig (tpl, wcfg);
    wordGenerator->setLayoutConfig (wcfg);
    wordGenerator->setTagTemplate (tpl);


    ExcelGenerator::ExcelLayoutConfig ecfg = excelGenerator->layout ();

    copyTemplateGeometryToConfig (tpl, ecfg);
    excelGenerator->setLayoutConfig (ecfg);
    excelGenerator->setTagTemplate (tpl);
}


void MainWindow::toggleLanguage ()
{
    uiLanguage = (uiLanguage == "EN") ? "RU" : "EN";

    settings.setValue ("ui/language", uiLanguage);

    updateLanguageTexts ();
}

void MainWindow::toggleTheme ()
{
    const AppTheme current = ThemeManager::currentTheme ();
    const AppTheme next	   = (current == AppTheme::Light) ? AppTheme::Dark : AppTheme::Light;

    ThemeManager::applyThemeToApplication (next);
    ThemeManager::saveThemeToSettings (settings, next);

    updateThemeStyles ();
}


bool MainWindow::isInsideDropArea (const QPoint &posInDrop) const { return dropArea && dropArea->rect ().contains (posInDrop); }


QString MainWindow::localized (const QString &english, const QString &russian) const { return (uiLanguage == "RU") ? russian : english; }


QIcon MainWindow::createGearIcon () const
{
    const QPixmap gearPmRaw		= (ThemeManager::currentTheme () == AppTheme::Dark) ? QPixmap (":/sprites/SettingsGear.png")
                                                                                    : QPixmap (":/sprites/BlackSettingsGear.png");
    const QPixmap gearPmCropped = PixmapUtils::cropTransparentMargins (gearPmRaw);


    return QIcon (gearPmCropped);
}


void MainWindow::generateDocument ()
{
    if (priceTags.isEmpty ())
    {
        QMessageBox::warning (this, localized ("No Data", "Нет данных"),
                              localized ("Please load an Excel file first.", "Пожалуйста, сначала загрузите файл Excel."));

        return;
    }

    const bool toExcel	 = (outputFormatComboBox && outputFormatComboBox->currentIndex () == 0);
    const QString filter = toExcel ? localized ("XLSX (*.xlsx)", "XLSX (*.xlsx)") : localized ("DOCX (*.docx)", "DOCX (*.docx)");
    QString suggested	 = toExcel ? localized ("out.xlsx", "out.xlsx") : localized ("out.docx", "out.docx");
    const QString outPath = QFileDialog::getSaveFileName (this, localized ("Save Output", "Сохранить вывод"), suggested, filter);

    if (outPath.isEmpty ())
        return;


    bool ok = false;

    if (toExcel)
        ok = excelGenerator->generateExcelDocument (priceTags, outPath);
    else
        ok = wordGenerator->generateWordDocument (priceTags, outPath);


    if (ok)
        QMessageBox::information (this, localized ("Success", "Успех"), localized ("Saved to: %1", "Сохранено в: %1").arg (outPath));
    else
        QMessageBox::critical (this, localized ("Error", "Ошибка"),
                               localized ("Failed to generate output.", "Не удалось сгенерировать вывод."));
}


void MainWindow::processFile (const QString &filePath)
{
    if (filePath.isEmpty ())
        return;


    QList<PriceTag> parsed;
    if (! excelParser->parseExcelFile (filePath, parsed))
    {
        QMessageBox::critical (this, localized ("Error", "Ошибка"),
                               localized ("Failed to parse Excel file.", "Не удалось разобрать файл Excel."));

        return;
    }

    currentFilePath = filePath;
    priceTags		= parsed;

    if (generateButton)
        generateButton->setEnabled (true);
    if (refreshStatsButton)
        refreshStatsButton->setEnabled (true);

    // Apply primary styling after both buttons are enabled
    updateButtonsPrimaryStyles ();


    if (dropArea)
    {
        setDropAreaSuccessStyle ();

        const QFileInfo fi (filePath);

        dropArea->setText (localized ("Loaded: %1", "Загружено: %1").arg (fi.fileName ()));
    }

    showStatistics ();
}


void MainWindow::showStatistics ()
{
    if (! statisticsText)
        return;

    if (priceTags.isEmpty ())
        statisticsText->setPlainText (localized ("No data loaded.", "Данные не загружены."));
    else
        statisticsText->setPlainText (buildStatisticsText ());


#ifdef USE_QT_CHARTS
    updateCharts ();
#endif
}


void MainWindow::openTemplateEditor ()
{
    if (! templateEditorDialog)
    {
        templateEditorDialog = new TemplateEditorDialog (this);
        templateEditorDialog->templateEditor ()->setTagTemplate (currentTemplate);
        templateEditorDialog->applyLanguage (uiLanguage);
        connectTemplateEditorSignals (templateEditorDialog);
    }
    else
        templateEditorDialog->applyLanguage (uiLanguage);


    templateEditorDialog->show ();
    templateEditorDialog->raise ();
    templateEditorDialog->activateWindow ();
}

void MainWindow::openFile ()
{
    const QString filePath = QFileDialog::getOpenFileName (this, localized ("Open Excel File", "Открыть файл Excel"), QString (),
                                                           localized ("Excel (*.xlsx)", "Excel (*.xlsx)"));
    if (filePath.isEmpty ())
        return;


    processFile (filePath);
}


void MainWindow::connectTemplateEditorSignals (TemplateEditorDialog *editor)
{
    if (! editor || ! editor->templateEditor ())
        return;


    connect (editor->templateEditor (), &TemplateEditorWidget::templateChanged, this,
             [this] (const TagTemplate &tpl)
             {
                 currentTemplate = tpl;

                 applyTemplateToGenerators (tpl);

                 ConfigManager::saveTemplate (currentTemplate);
             });
}


StatisticsData MainWindow::aggregateStatistics () const
{
    StatisticsData data;

    data.totalProducts = priceTags.size ();

    for (const PriceTag &tag : priceTags)
    {
        const int q = tag.getQuantity ();

        data.totalTags += q;

        if (tag.hasDiscount ())
        {
            data.productsWithDiscount++;
            data.totalDiscountValue += (tag.getPrice () - tag.getPrice2 ()) * q;
        }

        const double unit = (tag.getPrice2 () > 0.0 ? tag.getPrice2 () : tag.getPrice ());

        data.totalValue += unit * q;


        if (! tag.getBrand ().isEmpty ())
            data.brandCount[tag.getBrand ()]++;

        if (! tag.getCategory ().isEmpty ())
            data.categoryCount[tag.getCategory ()]++;

        if (! tag.getSupplier ().isEmpty ())
            data.suppliers.insert (tag.getSupplier ());
    }


    return data;
}


QString MainWindow::formatStatisticsText (const StatisticsData &data) const
{
    QString text;

    text += localized ("=== PRICE TAG STATISTICS ===\n\n", "=== СТАТИСТИКА ПО ЦЕННИКАМ ===\n\n");
    text += localized ("Total Products: %1\n", "Всего товаров: %1\n").arg (data.totalProducts);
    text += localized ("Total Price Tags: %1\n", "Всего ценников: %1\n").arg (data.totalTags);
    text += localized ("Total Value: %1 ₽\n", "Общая сумма: %1 ₽\n").arg (QString::number (data.totalValue, 'f', 0));
    text += localized ("Products with Discount: %1\n", "Товаров со скидкой: %1\n").arg (data.productsWithDiscount);
    text += localized ("Total Discount Value: %1 ₽\n", "Сумма скидок: %1 ₽\n").arg (QString::number (data.totalDiscountValue, 'f', 0));
    text += localized ("Unique Brands: %1\n", "Уникальных брендов: %1\n").arg (data.brandCount.size ());
    text += localized ("Unique Categories: %1\n", "Уникальных категорий: %1\n").arg (data.categoryCount.size ());
    text += localized ("Unique Suppliers: %1\n", "Уникальных поставщиков: %1\n").arg (data.suppliers.size ());
    text += "\n";
    text += localized ("=== BRANDS ===\n", "=== БРЕНДЫ ===\n");


    for (auto it = data.brandCount.constBegin (); it != data.brandCount.constEnd (); ++it)
        text += QString ("• %1\n").arg (it.key ());


    return text;
}


bool MainWindow::mimeHasXlsx (const QMimeData *mime) const
{
    if (! mime || ! mime->hasUrls ())
        return false;

    const QList<QUrl> urls = mime->urls ();
    if (urls.isEmpty ())
        return false;


    const QString path = urls.first ().toLocalFile ();


    return path.endsWith (".xlsx", Qt::CaseInsensitive);
}


QString MainWindow::firstXlsxFromMime (const QMimeData *mime) const
{
    if (! mime || ! mime->hasUrls ())
        return {};


    const QList<QUrl> urls = mime->urls ();

    for (const QUrl &url : urls)
    {
        const QString path = url.toLocalFile ();

        if (path.endsWith (".xlsx", Qt::CaseInsensitive))
            return path;
    }


    return {};
}


QPoint MainWindow::mapGlobalToDropArea (const QPoint &globalPos) const
{
    return dropArea ? dropArea->mapFromGlobal (globalPos) : QPoint ();
}


#ifdef USE_QT_CHARTS
void MainWindow::clearChartsLayout ()
{
    if (! chartsLayout)
        return;

    while (QLayoutItem *item = chartsLayout->takeAt (0))
    {
        if (QWidget *w = item->widget ())
            w->deleteLater ();

        delete item;
    }
}

void MainWindow::aggregateChartData (QMap<QString, int> &brandCount, QMap<QString, int> &categoryCount, int &totalProducts, int &totalTags,
                                     int &productsWithDiscount) const
{
    const StatisticsData data = aggregateStatistics ();

    totalProducts		 = data.totalProducts;
    totalTags			 = data.totalTags;
    productsWithDiscount = data.productsWithDiscount;
    brandCount			 = data.brandCount;
    categoryCount		 = data.categoryCount;
}


void MainWindow::buildBrandChart (const QMap<QString, int> &brandCount)
{
    const int maxSlices		= 8;
    QPieSeries *brandSeries = new QPieSeries ();
    QList<QPair<QString, int>> brandPairs;

    for (auto it = brandCount.begin (); it != brandCount.end (); ++it)
        brandPairs.append (qMakePair (it.key (), it.value ()));


    std::sort (brandPairs.begin (), brandPairs.end (), [] (const auto &a, const auto &b) { return a.second > b.second; });
    int others = 0;

    for (int i = 0; i < brandPairs.size (); ++i)
    {
        if (i < maxSlices)
            brandSeries->append (brandPairs[i].first, brandPairs[i].second);
        else
            others += brandPairs[i].second;
    }

    if (others > 0)
        brandSeries->append (localized ("Others", "Другие"), others);


    QChart *brandChart = new QChart ();
    brandChart->addSeries (brandSeries);
    brandChart->setTitle (localized ("Brands Distribution", "Распределение брендов"));
    brandChart->legend ()->setVisible (true);
    brandChart->legend ()->setAlignment (Qt::AlignBottom);

    QChartView *brandView = new QChartView (brandChart);
    brandView->setRenderHint (QPainter::Antialiasing);
    chartsLayout->addWidget (brandView, 1);
    brandChartView = brandView;
}

void MainWindow::buildCategoryChart (const QMap<QString, int> &categoryCount)
{
    const int maxSlices		   = 8;
    QPieSeries *categorySeries = new QPieSeries ();
    QList<QPair<QString, int>> categoryPairs;


    for (auto it = categoryCount.begin (); it != categoryCount.end (); ++it)
        categoryPairs.append (qMakePair (it.key (), it.value ()));

    std::sort (categoryPairs.begin (), categoryPairs.end (), [] (const auto &a, const auto &b) { return a.second > b.second; });

    int others = 0;


    for (int i = 0; i < categoryPairs.size (); ++i)
    {
        if (i < maxSlices)
            categorySeries->append (categoryPairs[i].first, categoryPairs[i].second);
        else
            others += categoryPairs[i].second;
    }

    if (others > 0)
        categorySeries->append (localized ("Others", "Другие"), others);


    QChart *categoryChart = new QChart ();
    categoryChart->addSeries (categorySeries);
    categoryChart->setTitle (localized ("Categories Distribution", "Распределение категорий"));
    categoryChart->legend ()->setVisible (true);
    categoryChart->legend ()->setAlignment (Qt::AlignBottom);

    QChartView *categoryView = new QChartView (categoryChart);
    categoryView->setRenderHint (QPainter::Antialiasing);
    chartsLayout->addWidget (categoryView, 1);
    categoryChartView = categoryView;
}

void MainWindow::buildSummaryBarChart (int totalProducts, int totalTags, int productsWithDiscount)
{
    QBarSet *productsSet  = new QBarSet (localized ("Products", "Товары"));
    QBarSet *tagsSet	  = new QBarSet (localized ("Tags", "Ценники"));
    QBarSet *discountsSet = new QBarSet (localized ("With Discount", "Со скидкой"));


    *productsSet << totalProducts;
    *tagsSet << totalTags;
    *discountsSet << productsWithDiscount;


    QBarSeries *barSeries = new QBarSeries ();
    barSeries->append (productsSet);
    barSeries->append (tagsSet);
    barSeries->append (discountsSet);

    QChart *barChart = new QChart ();
    barChart->addSeries (barSeries);
    barChart->setTitle (localized ("Summary", "Сводка"));

    QStringList categoriesAxis;
    categoriesAxis << localized ("Total", "Всего");

    QBarCategoryAxis *axisX = new QBarCategoryAxis ();
    axisX->append (categoriesAxis);
    barChart->addAxis (axisX, Qt::AlignBottom);
    barSeries->attachAxis (axisX);

    QValueAxis *axisY = new QValueAxis ();
    axisY->setLabelFormat ("%d");
    axisY->setTitleText (localized ("Count", "Количество"));
    barChart->addAxis (axisY, Qt::AlignLeft);
    barSeries->attachAxis (axisY);

    QChartView *summaryView = new QChartView (barChart);
    summaryView->setRenderHint (QPainter::Antialiasing);
    chartsLayout->addWidget (summaryView, 1);
    summaryBarChartView = summaryView;
}


void MainWindow::updateCharts ()
{
    if (! chartsLayout)
        return;

    clearChartsLayout ();

    if (priceTags.isEmpty ())
        return;

    const StatisticsData data = aggregateStatistics ();

    buildBrandChart (data.brandCount);
    buildCategoryChart (data.categoryCount);
    buildSummaryBarChart (data.totalProducts, data.totalTags, data.productsWithDiscount);
}

#endif
