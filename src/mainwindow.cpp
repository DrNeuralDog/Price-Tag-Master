#include "mainwindow.h"
#include <QBitmap>
#include <QFileInfo>
#include <QHeaderView>
#include <QImage>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QRect>

#ifdef USE_QT_CHARTS
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#endif
#include <QRegion>
#include <QResizeEvent>
#include <QSet>
#include <QToolButton>
#include <QUrl>
#include <algorithm>
#include "excelgenerator.h"


// Helper: crop transparent margins around an icon to maximize visible size within given button
static QPixmap cropTransparentMargins (const QPixmap &src)
{
    if (src.isNull ())
        return src;

    QImage img	= src.toImage ().convertToFormat (QImage::Format_ARGB32);
    const int w = img.width ();
    const int h = img.height ();

    int minX = w, minY = h, maxX = -1, maxY = -1;
    for (int y = 0; y < h; ++y)
    {
        const QRgb *line = reinterpret_cast<const QRgb *> (img.constScanLine (y));
        for (int x = 0; x < w; ++x)
        {
            if (qAlpha (line[x]) > 0)
            {
                if (x < minX)
                    minX = x;
                if (x > maxX)
                    maxX = x;
                if (y < minY)
                    minY = y;
                if (y > maxY)
                    maxY = y;
            }
        }
    }

    if (maxX < minX || maxY < minY)
        return src; // fully transparent or invalid

    const QRect rect (minX, minY, maxX - minX + 1, maxY - minY + 1);
    return QPixmap::fromImage (img.copy (rect));
}

// ToolButton with horizontally trimmed hit area
class TrimmedHitToolButton: public QToolButton
{
public:
    explicit TrimmedHitToolButton (QWidget *parent = nullptr) : QToolButton (parent) {}
    void setHorizontalTrimPx (int px) { horizontalTrimPx = qMax (0, px); }
    void ensureShields ()
    {
        if (! leftShield)
        {
            leftShield = new QWidget (this);
            leftShield->setAttribute (Qt::WA_TransparentForMouseEvents, false);
            leftShield->setMouseTracking (true);
            leftShield->installEventFilter (this);
        }
        if (! rightShield)
        {
            rightShield = new QWidget (this);
            rightShield->setAttribute (Qt::WA_TransparentForMouseEvents, false);
            rightShield->setMouseTracking (true);
            rightShield->installEventFilter (this);
        }
    }

protected:
    bool hitButton (const QPoint &pos) const override
    {
        const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
        const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);
        return allowed.contains (pos);
    }
    void mousePressEvent (QMouseEvent *e) override
    {
        const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
        const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);
        if (! allowed.contains (e->pos ()))
        {
            e->accept ();
            return; // swallow press
        }
        QToolButton::mousePressEvent (e);
    }
    void mouseReleaseEvent (QMouseEvent *e) override
    {
        const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
        const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);
        if (! allowed.contains (e->pos ()))
        {
            e->accept ();
            return; // swallow release
        }
        QToolButton::mouseReleaseEvent (e);
    }
    void mouseDoubleClickEvent (QMouseEvent *e) override
    {
        const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
        const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);
        if (! allowed.contains (e->pos ()))
        {
            e->accept ();
            return; // swallow dblclick
        }
        QToolButton::mouseDoubleClickEvent (e);
    }
    void paintEvent (QPaintEvent *event) override
    {
        Q_UNUSED (event);
        QPainter painter (this);
        painter.setRenderHint (QPainter::SmoothPixmapTransform, true);
        // draw icon at 95% of button diameter, centered, keep aspect ratio
        QPixmap pm = icon ().pixmap (iconSize ());
        if (! pm.isNull ())
        {
            const int base	 = qMin (width (), height ());
            const int target = qMax (1, qRound (base * 0.88));
            const QSize targetSize (target, target);
            const QPixmap scaled = pm.scaled (targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            const int x			 = (width () - scaled.width ()) / 2;
            const int y			 = (height () - scaled.height ()) / 2;
            painter.drawPixmap (x, y, scaled);
        }
    }
    void resizeEvent (QResizeEvent *e) override
    {
        QToolButton::resizeEvent (e);
        updateShieldsGeometry ();
    }
    bool eventFilter (QObject *obj, QEvent *event) override
    {
        if (obj == leftShield || obj == rightShield)
        {
            switch (event->type ())
            {
                case QEvent::MouseButtonPress:
                case QEvent::MouseButtonRelease:
                case QEvent::MouseButtonDblClick:
                case QEvent::HoverEnter:
                case QEvent::HoverMove:
                case QEvent::HoverLeave:
                case QEvent::Wheel:
                    return true; // swallow
                default:
                    break;
            }
        }
        return QToolButton::eventFilter (obj, event);
    }

private:
    int horizontalTrimPx = 0;
    QWidget *leftShield	 = nullptr;
    QWidget *rightShield = nullptr;

    void updateShieldsGeometry ()
    {
        if (horizontalTrimPx <= 0)
        {
            if (leftShield)
                leftShield->setGeometry (0, 0, 0, 0);
            if (rightShield)
                rightShield->setGeometry (width (), 0, 0, height ());
            return;
        }
        ensureShields ();
        const int trim = qMin (horizontalTrimPx, width () / 2 - 1);
        if (leftShield)
        {
            leftShield->setGeometry (0, 0, trim, height ());
            leftShield->setStyleSheet ("background: transparent;");
            leftShield->setAttribute (Qt::WA_NoSystemBackground, true);
        }
        if (rightShield)
        {
            rightShield->setGeometry (width () - trim, 0, trim, height ());
            rightShield->setStyleSheet ("background: transparent;");
            rightShield->setAttribute (Qt::WA_NoSystemBackground, true);
        }
        if (leftShield)
            leftShield->raise ();
        if (rightShield)
            rightShield->raise ();
    }
};

// Build precise clickable region from non-transparent pixels
static QRegion alphaRegionFromPixmap (const QPixmap &src, int alphaThreshold = 1)
{
    if (src.isNull ())
        return {};

    const QImage img = src.toImage ().convertToFormat (QImage::Format_ARGB32);
    const int w		 = img.width ();
    const int h		 = img.height ();

    QRegion region;
    for (int y = 0; y < h; ++y)
    {
        const QRgb *line = reinterpret_cast<const QRgb *> (img.constScanLine (y));
        int x			 = 0;
        while (x < w)
        {
            while (x < w && qAlpha (line[x]) <= alphaThreshold)
                ++x;
            if (x >= w)
                break;
            const int start = x;
            while (x < w && qAlpha (line[x]) > alphaThreshold)
                ++x;
            region |= QRegion (start, y, x - start, 1);
        }
    }

    return region;
}

static void applyIconMaskToToolButton (QToolButton *button, const QIcon &icon, const QSize &iconSize)
{
    if (! button)
        return;
    const QPixmap pm  = icon.pixmap (iconSize);
    const QRegion reg = alphaRegionFromPixmap (pm, 1);
    if (! reg.isEmpty ())
        button->setMask (reg);
}

static void applyCircularMask (QWidget *w)
{
    if (! w)
        return;
    const QSize s = w->size ();
    if (s.isEmpty ())
        return;
    w->setMask (QRegion (0, 0, s.width (), s.height (), QRegion::Ellipse));
}


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
        else
        {
            // Save current defaults to file
            ConfigManager::saveTemplate (currentTemplate);
        }
        applyTemplateToGenerators (currentTemplate);
    }

    setAcceptDrops (true);
    updateThemeStyles ();
}

MainWindow::~MainWindow () {}


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
        if (! allowed.contains (pos))
        {
            // ignore click outside allowed rect
            return true;
        }
    }
    return QMainWindow::eventFilter (obj, event);
}

void MainWindow::setupToolbar ()
{
    mainToolbar = addToolBar (tr ("Toolbar"));
    mainToolbar->setMovable (false);
    mainToolbar->setIconSize (QSize (20, 20));

    // Keep internal margins zero; we'll offset items by 2mm using wrappers and styles
    const int twoMmPx = qRound (this->logicalDpiY () * 2.0 / 25.4);
    mainToolbar->setContentsMargins (0, 0, 0, 0);

    // Left padding ~3mm to shift theme/lang buttons right
    {
        QWidget *leftPad = new QWidget (this);
        leftPad->setFixedWidth (qRound (this->logicalDpiX () * 1.0 / 25.4));
        mainToolbar->addWidget (leftPad);
    }

    // Theme button (neutral look, rounded)
    themeButton = new QPushButton (this);
    themeButton->setCursor (Qt::PointingHandCursor);
    themeButton->setMinimumHeight (28);
    themeButton->setMinimumWidth (84);
    themeButton->setText (ThemeManager::currentTheme () == AppTheme::Dark ? tr ("Dark") : tr ("Light"));
    connect (themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    // Wrap themeButton to enforce 2mm vertical spacing inside toolbar
    QWidget *themeWrap			 = new QWidget (this);
    QVBoxLayout *themeWrapLayout = new QVBoxLayout (themeWrap);
    themeWrapLayout->setContentsMargins (0, twoMmPx, 0, twoMmPx);
    themeWrapLayout->setSpacing (0);
    themeWrapLayout->addWidget (themeButton);
    mainToolbar->addWidget (themeWrap);

    // Fixed spacer between theme and lang (half of lang width: 14px)
    QWidget *langSpacer = new QWidget (this);
    langSpacer->setFixedWidth (14);
    mainToolbar->addWidget (langSpacer);

    // Language button (RU/EN) - square 28x28
    langButton = new QPushButton (this);
    langButton->setCursor (Qt::PointingHandCursor);
    langButton->setFixedSize (28, 28);
    uiLanguage = "EN";
    langButton->setText (uiLanguage);
    connect (langButton, &QPushButton::clicked, this, &MainWindow::toggleLanguage);
    // Wrap langButton similarly for consistent vertical offset
    QWidget *langWrap			= new QWidget (this);
    QVBoxLayout *langWrapLayout = new QVBoxLayout (langWrap);
    langWrapLayout->setContentsMargins (0, twoMmPx, 0, twoMmPx);
    langWrapLayout->setSpacing (0);
    langWrapLayout->addWidget (langButton);
    mainToolbar->addWidget (langWrap);

    QWidget *spacer = new QWidget (this);
    spacer->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainToolbar->addWidget (spacer);

    const bool isDark = (ThemeManager::currentTheme () == AppTheme::Dark);
    // Load gear icon and crop transparent margins to keep real visible size large
    const QPixmap gearPmRaw		= (ThemeManager::currentTheme () == AppTheme::Dark) ? QPixmap (":/icons/SettingsGear.png")
                                                                                    : QPixmap (":/icons/BlackSettingsGear.png");
    const QPixmap gearPmCropped = cropTransparentMargins (gearPmRaw);
    const QIcon gearIcon (gearPmCropped);
    openEditorAction = new QAction (gearIcon, QString (), this);
    connect (openEditorAction, &QAction::triggered, this,
             [this] ()
             {
                 if (! templateEditorDialog)
                 {
                     templateEditorDialog = new TemplateEditorDialog (this);

                     // initialize template defaults to editor
                     templateEditorDialog->templateEditor ()->setTagTemplate (currentTemplate);

                     templateEditorDialog->applyLanguage (uiLanguage);

                     connect (templateEditorDialog->templateEditor (), &TemplateEditorWidget::templateChanged, this,
                              [this] (const TagTemplate &tpl)
                              {
                                  currentTemplate = tpl;
                                  applyTemplateToGenerators (tpl);

                                  // Persist on change
                                  ConfigManager::saveTemplate (currentTemplate);
                              });
                 }
                 else
                 {
                     templateEditorDialog->applyLanguage (uiLanguage);
                 }
                 templateEditorDialog->show ();
                 templateEditorDialog->raise ();
                 templateEditorDialog->activateWindow ();
             });

    // Create custom toolbutton for the action with trimmed horizontal hit area
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
        const int fourMmPx = 0; // disable side trim; use circular mask instead
        btn->setHorizontalTrimPx (fourMmPx);
        applyCircularMask (btn);
        mainToolbar->addWidget (btn);
        // add right margin ~3mm
        QWidget *rightPad = new QWidget (this);
        rightPad->setFixedWidth (qRound (this->logicalDpiX () * 3.0 / 25.4));
        mainToolbar->addWidget (rightPad);
        gearButton = btn;
    }
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
    generateButton = new QPushButton (QIcon (":/icons/tag.png"), tr ("  Generate Price Tags"));
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
void MainWindow::openFile ()
{
    const QString filePath = QFileDialog::getOpenFileName (this, localized ("Open Excel File", "Открыть файл Excel"), QString (),
                                                           localized ("Excel (*.xlsx)", "Excel (*.xlsx)"));
    if (filePath.isEmpty ())
        return;

    processFile (filePath);
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
    {
        int totalProducts		  = priceTags.size ();
        int totalTags			  = 0;
        int productsWithDiscount  = 0;
        double totalValue		  = 0.0;
        double totalDiscountValue = 0.0;

        QMap<QString, int> brandCount;
        QMap<QString, int> categoryCount;
        QSet<QString> suppliers;


        for (const PriceTag &tag : priceTags)
        {
            const int q = tag.getQuantity ();
            totalTags += q;

            if (tag.hasDiscount ())
            {
                productsWithDiscount++;
                totalDiscountValue += (tag.getPrice () - tag.getPrice2 ()) * q;
            }

            const double unit = (tag.getPrice2 () > 0.0 ? tag.getPrice2 () : tag.getPrice ());
            totalValue += unit * q;

            if (! tag.getBrand ().isEmpty ())
                brandCount[tag.getBrand ()]++;
            if (! tag.getCategory ().isEmpty ())
                categoryCount[tag.getCategory ()]++;
            if (! tag.getSupplier ().isEmpty ())
                suppliers.insert (tag.getSupplier ());
        }


        QString text;
        text += localized ("=== PRICE TAG STATISTICS ===\n\n", "=== СТАТИСТИКА ПО ЦЕННИКАМ ===\n\n");
        text += localized ("Total Products: %1\n", "Всего товаров: %1\n").arg (totalProducts);
        text += localized ("Total Price Tags: %1\n", "Всего ценников: %1\n").arg (totalTags);
        text += localized ("Total Value: %1 ₽\n", "Общая сумма: %1 ₽\n").arg (QString::number (totalValue, 'f', 0));
        text += localized ("Products with Discount: %1\n", "Товаров со скидкой: %1\n").arg (productsWithDiscount);
        text += localized ("Total Discount Value: %1 ₽\n", "Сумма скидок: %1 ₽\n").arg (QString::number (totalDiscountValue, 'f', 0));
        text += localized ("Unique Brands: %1\n", "Уникальных брендов: %1\n").arg (brandCount.size ());
        text += localized ("Unique Categories: %1\n", "Уникальных категорий: %1\n").arg (categoryCount.size ());
        text += localized ("Unique Suppliers: %1\n", "Уникальных поставщиков: %1\n").arg (suppliers.size ());
        text += "\n";
        text += localized ("=== BRANDS ===\n", "=== БРЕНДЫ ===\n");

        for (auto it = brandCount.constBegin (); it != brandCount.constEnd (); ++it)
            text += QString ("• %1\n").arg (it.key ());

        statisticsText->setPlainText (text);
    }


#ifdef USE_QT_CHARTS
    updateCharts ();
#endif
}

void MainWindow::dragEnterEvent (QDragEnterEvent *event)
{
    if (! event || ! event->mimeData () || ! event->mimeData ()->hasUrls () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
    {
        if (event)
            event->ignore ();

        return;
    }

    const QList<QUrl> urls = event->mimeData ()->urls ();
    if (urls.isEmpty ())
    {
        event->ignore ();

        return;
    }

    const QString path = urls.first ().toLocalFile ();
    if (! path.endsWith (".xlsx", Qt::CaseInsensitive))
    {
        event->ignore ();

        return;
    }
    // Accept drag immediately for valid data; refine feedback during dragMove
    event->acceptProposedAction ();

    // Provide visual hint only when actually over the drop area
    const QPoint globalPos = this->mapToGlobal (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
    );
    const QPoint posInDrop = dropArea->mapFromGlobal (globalPos);
    if (dropArea->rect ().contains (posInDrop))
        setDropAreaHoverStyle ();
    else
        setDropAreaDefaultStyle ();
}

void MainWindow::dragMoveEvent (QDragMoveEvent *event)
{
    if (! event || ! event->mimeData () || ! event->mimeData ()->hasUrls () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
    {
        if (event)
            event->ignore ();
        return;
    }

    const QList<QUrl> urls = event->mimeData ()->urls ();
    if (urls.isEmpty ())
    {
        event->ignore ();
        return;
    }

    const QString path = urls.first ().toLocalFile ();
    if (! path.endsWith (".xlsx", Qt::CaseInsensitive))
    {
        event->ignore ();
        return;
    }

    const QPoint globalPos = this->mapToGlobal (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
    );

    const QPoint posInDrop = dropArea->mapFromGlobal (globalPos);
    if (dropArea->rect ().contains (posInDrop))
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

void MainWindow::dropEvent (QDropEvent *event)
{
    if (! event || ! event->mimeData () || ! dropArea || ! tabWidget || tabWidget->currentIndex () != 0)
        return;

    const QPoint globalPos = this->mapToGlobal (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            event->position ().toPoint ()
#else
            event->pos ()
#endif
    );

    const QPoint posInDrop = dropArea->mapFromGlobal (globalPos);

    if (! dropArea->rect ().contains (posInDrop))
        return;

    const QList<QUrl> urls = event->mimeData ()->urls ();

    for (const QUrl &url : urls)
    {
        const QString path = url.toLocalFile ();

        if (path.endsWith (".xlsx", Qt::CaseInsensitive))
        {
            processFile (path);
            event->acceptProposedAction ();

            return;
        }
    }
}

void MainWindow::toggleTheme ()
{
    const AppTheme current = ThemeManager::currentTheme ();
    const AppTheme next	   = (current == AppTheme::Light) ? AppTheme::Dark : AppTheme::Light;

    ThemeManager::applyThemeToApplication (next);
    ThemeManager::saveThemeToSettings (settings, next);

    updateThemeStyles ();
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
        // Match toolbar background to window; keep only bottom border; buttons have no extra background
        mainToolbar->setStyleSheet (QString ("QToolBar { background: palette(window); border: none; border-bottom: 1px solid %1; } "
                                             "QToolBar QToolButton { margin-top: %2px; margin-bottom: %2px; padding: 0px; border: none; "
                                             "background: transparent; } "
                                             "QToolBar QToolButton:hover { background: transparent; } ")
                                            .arg (borderColor)
                                            .arg (twoMmPx));
    }

    // Theme button style + text (neutral, no gradient)
    if (themeButton)
    {
        themeButton->setText (isDark ? localized ("Dark", "Тёмная") : localized ("Light", "Светлая"));
        const QString tbStyle =
                QString ("QPushButton { border: 1px solid %1; border-radius: 14px; padding: 4px 12px; color: %2; background: %3; }"
                         "QPushButton:hover { background: %4; }"
                         "QPushButton:pressed { padding-top: 5px; padding-bottom: 3px; }")
                        .arg (isDark ? "#475569" : "#CBD5E1", isDark ? "#E5E7EB" : "#111827", isDark ? "#1F2937" : "#FFFFFF",
                              isDark ? "#111827" : "#F3F4F6");
        themeButton->setStyleSheet (tbStyle);
    }

    // Language button neutral style - adjusted for square
    if (langButton)
    {
        const QString lbStyle =
                QString ("QPushButton { border: 1px solid %1; border-radius: 14px; padding: 0px; background: %2; color: %3; }"
                         "QPushButton:hover { background: %4; }"
                         "QPushButton:pressed { padding-top: 1px; padding-bottom: -1px; }")
                        .arg (isDark ? "#475569" : "#CBD5E1", isDark ? "#1F2937" : "#FFFFFF", isDark ? "#E5E7EB" : "#111827",
                              isDark ? "#111827" : "#F3F4F6");
        langButton->setStyleSheet (lbStyle);
        langButton->setText (uiLanguage);
    }

    // Editor gear icon per theme (use cropped pixmap to maximize visible area)
    if (openEditorAction)
    {
        const QPixmap gearPmRaw = isDark ? QPixmap (":/icons/SettingsGear.png") : QPixmap (":/icons/BlackSettingsGear.png");
        openEditorAction->setIcon (QIcon (cropTransparentMargins (gearPmRaw)));
        if (gearButton)
        {
            const int diameter = 25;
            gearButton->setFixedSize (diameter, diameter);
            gearButton->setIconSize (QSize (diameter, diameter));
            applyCircularMask (gearButton);
        }
    }

    updateButtonsPrimaryStyles ();
}

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

void MainWindow::updateButtonsPrimaryStyles ()
{
    const bool isDark		   = (ThemeManager::currentTheme () == AppTheme::Dark);
    const QString primaryStyle = buildPrimaryButtonStyle (isDark);

    // Enforce equal heights regardless of icon/text paddings
    const int baseHeight = isDark ? 36 : 38; // match previous Generate visual height
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


void MainWindow::dragLeaveEvent (QDragLeaveEvent *event)
{
    Q_UNUSED (event);

    setDropAreaDefaultStyle ();
}

// removed eventFilter override (not used)


void MainWindow::applyTemplateToGenerators (const TagTemplate &tpl)
{
    // Layout to generators from template geometry
    WordGenerator::DocxLayoutConfig wcfg = wordGenerator->layout ();
    wcfg.tagWidthMm						 = tpl.tagWidthMm;
    wcfg.tagHeightMm					 = tpl.tagHeightMm;
    wcfg.marginLeftMm					 = tpl.marginLeftMm;
    wcfg.marginTopMm					 = tpl.marginTopMm;
    wcfg.marginRightMm					 = tpl.marginRightMm;
    wcfg.marginBottomMm					 = tpl.marginBottomMm;
    wcfg.spacingHMm						 = tpl.spacingHMm;
    wcfg.spacingVMm						 = tpl.spacingVMm;
    wordGenerator->setLayoutConfig (wcfg);
    wordGenerator->setTagTemplate (tpl);


    ExcelGenerator::ExcelLayoutConfig ecfg = excelGenerator->layout ();
    ecfg.tagWidthMm						   = tpl.tagWidthMm;
    ecfg.tagHeightMm					   = tpl.tagHeightMm;
    ecfg.marginLeftMm					   = tpl.marginLeftMm;
    ecfg.marginTopMm					   = tpl.marginTopMm;
    ecfg.marginRightMm					   = tpl.marginRightMm;
    ecfg.marginBottomMm					   = tpl.marginBottomMm;
    ecfg.spacingHMm						   = tpl.spacingHMm;
    ecfg.spacingVMm						   = tpl.spacingVMm;
    excelGenerator->setLayoutConfig (ecfg);
    excelGenerator->setTagTemplate (tpl);
}


void MainWindow::toggleLanguage ()
{
    uiLanguage = (uiLanguage == "EN") ? "RU" : "EN";
    QSettings s;
    s.setValue ("ui/language", uiLanguage);
    updateLanguageTexts ();
}


void MainWindow::setUiLanguage (const QString &lang)
{
    uiLanguage = lang;
    updateLanguageTexts ();
}

QString MainWindow::localized (const QString &english, const QString &russian) const { return (uiLanguage == "RU") ? russian : english; }


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

    // Update template editor dialog language if open
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

#ifdef USE_QT_CHARTS
void MainWindow::updateCharts ()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    using namespace QtCharts;
#endif
    // Qt 6: types are available directly after including <QtCharts/...>
    if (! chartsLayout)
        return;


    // Clear previous chart views
    while (QLayoutItem *item = chartsLayout->takeAt (0))
    {
        if (QWidget *w = item->widget ())
            w->deleteLater ();
        delete item;
    }


    if (priceTags.isEmpty ())
        return;


    // Build data aggregations
    QMap<QString, int> brandCount;
    QMap<QString, int> categoryCount;
    int totalProducts		 = priceTags.size ();
    int totalTags			 = 0;
    int productsWithDiscount = 0;


    for (const PriceTag &tag : priceTags)
    {
        totalTags += tag.getQuantity ();

        if (tag.hasDiscount ())
            productsWithDiscount++;
        if (! tag.getBrand ().isEmpty ())
            brandCount[tag.getBrand ()]++;
        if (! tag.getCategory ().isEmpty ())
            categoryCount[tag.getCategory ()]++;
    }


    // Pie: Brands distribution (top N to keep it readable)
    const int maxSlices				  = 8;
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


    // Pie: Categories distribution (top N)
    QPieSeries *categorySeries = new QPieSeries ();
    QList<QPair<QString, int>> categoryPairs;

    for (auto it = categoryCount.begin (); it != categoryCount.end (); ++it)
        categoryPairs.append (qMakePair (it.key (), it.value ()));

    std::sort (categoryPairs.begin (), categoryPairs.end (), [] (const auto &a, const auto &b) { return a.second > b.second; });
    others = 0;

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

    // Bar - Summary
    QBarSet *productsSet	= new QBarSet (localized ("Products", "Товары"));
    QBarSet *tagsSet		= new QBarSet (localized ("Tags", "Ценники"));
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
#endif
