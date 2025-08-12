#include "mainwindow.h"
#include <QFileInfo>
#include <QHeaderView>
#include "excelgenerator.h"


MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
{
    excelParser	   = new ExcelParser (this);
    wordGenerator  = new WordGenerator (this);
    excelGenerator = new ExcelGenerator (this);

    setupUI ();
    setupToolbar ();

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

    // Preview tab deprecated; Template Editor available from toolbar

    setupStatisticsTab ();
}

void MainWindow::setupToolbar ()
{
    mainToolbar = addToolBar (tr ("Toolbar"));
    mainToolbar->setMovable (false);
    toggleThemeAction = new QAction (tr ("Light/Dark"), this);
    toggleThemeAction->setToolTip (tr ("Toggle theme"));
    toggleThemeAction->setCheckable (false);
    connect (toggleThemeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    mainToolbar->addAction (toggleThemeAction);

    QWidget *spacer = new QWidget (this);
    spacer->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainToolbar->addWidget (spacer);

    QAction *openEditorAction = new QAction (tr ("Template Editor"), this);
    connect (openEditorAction, &QAction::triggered, this,
             [this] ()
             {
                 if (! templateEditorDialog)
                 {
                     templateEditorDialog = new TemplateEditorDialog (this);
                 }
                 templateEditorDialog->show ();
                 templateEditorDialog->raise ();
                 templateEditorDialog->activateWindow ();
             });
    mainToolbar->addAction (openEditorAction);
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
    generateButton = new QPushButton (tr ("Generate Price Tags"));
    generateButton->setObjectName ("primary"); // map to primary style
    generateButton->setEnabled (false);

    progressBar = new QProgressBar ();
    progressBar->setVisible (false);

    mainTabLayout->addWidget (dropArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout ();
    buttonLayout->addWidget (openButton);
    buttonLayout->addWidget (generateButton);

    mainTabLayout->addLayout (buttonLayout);

    outputFormatComboBox = new QComboBox ();
    outputFormatComboBox->addItem (tr ("Word/DOCX"));
    outputFormatComboBox->addItem (tr ("Excel/XLSX"));
    mainTabLayout->addWidget (outputFormatComboBox);

    mainTabLayout->addWidget (progressBar);

    connect (openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect (generateButton, &QPushButton::clicked, this, &MainWindow::generateDocument);

    tabWidget->addTab (mainTab, tr ("Main"));
}


void MainWindow::setupPreviewTab () { /* deprecated */ }

void MainWindow::setupStatisticsTab ()
{
    QWidget *statsTab		 = new QWidget ();
    QVBoxLayout *statsLayout = new QVBoxLayout (statsTab);

    statisticsText = new QTextEdit ();
    statisticsText->setReadOnly (true);

    refreshStatsButton = new QPushButton (tr ("Refresh Statistics"));
    refreshStatsButton->setEnabled (false);

    statsLayout->addWidget (statisticsText, 1);
    statsLayout->addWidget (refreshStatsButton);

    connect (refreshStatsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);

    tabWidget->addTab (statsTab, tr ("Statistics"));
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
    const QString borderColor = isDark ? "#475569" : "#E2E8F0"; // border tokens
    const QString subtleBg	  = isDark ? "#1F2937" : "#F1F5F9";
    dropArea->setStyleSheet (QString ("QLabel { border: 2px dashed %1; border-radius: 4px; background: %2; }").arg (borderColor, subtleBg));
    if (mainToolbar)
    {
        mainToolbar->setStyleSheet (QString ("QToolBar { border-bottom: 1px solid %1; } ").arg (borderColor));
    }
}


void MainWindow::dragEnterEvent (QDragEnterEvent *event)
{
    if (event->mimeData ()->hasUrls ())
    {
        QList<QUrl> urls = event->mimeData ()->urls ();
        if (urls.size () == 1)
        {
            QString filePath = urls[0].toLocalFile ();
            if (filePath.endsWith (".xlsx", Qt::CaseInsensitive))
            {
                event->acceptProposedAction ();
                const bool isDark	 = (ThemeManager::currentTheme () == AppTheme::Dark);
                const QString border = "#3B82F6";									 // brand/primary
                const QString bg	 = isDark ? "rgba(96,165,250,0.20)" : "#DBEAFE"; // highlight/bg
                dropArea->setStyleSheet (
                        QString ("QLabel { border: 2px dashed %1; border-radius: 4px; background: %2; }").arg (border, bg));
                return;
            }
        }
    }

    event->ignore ();
}

void MainWindow::dropEvent (QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData ()->urls ();
    QString filePath = urls[0].toLocalFile ();

    updateThemeStyles ();

    processFile (filePath);

    event->acceptProposedAction ();
}


void MainWindow::openFile ()
{
    QString filePath = QFileDialog::getOpenFileName (this, tr ("Open Excel File"), QString (), tr ("Excel Files (*.xlsx)"));

    if (! filePath.isEmpty ())
    {
        processFile (filePath);
    }
}


void MainWindow::processFile (const QString &filePath)
{
    currentFilePath = filePath;
    dropArea->setText (tr ("File: %1").arg (QFileInfo (filePath).fileName ()));


    priceTags.clear ();
    bool success = excelParser->parseExcelFile (filePath, priceTags);

    if (success && ! priceTags.isEmpty ())
    {
        int totalTags = 0;
        for (const PriceTag &tag : priceTags)
        {
            totalTags += tag.getQuantity ();
        }

        dropArea->setText (tr ("Loaded %1 products with a total of %2 price tags.").arg (priceTags.size ()).arg (totalTags));
        generateButton->setEnabled (true);
        refreshStatsButton->setEnabled (true);


        updatePreview ();
        updateStatistics ();
    }
    else
    {
        QMessageBox::warning (this, tr ("Error"), tr ("Failed to parse the Excel file or no products found."));
        dropArea->setText (tr ("Drag and drop Excel file here or use the Open button below"));
        generateButton->setEnabled (false);
        refreshStatsButton->setEnabled (false);
    }
}

void MainWindow::generateDocument ()
{
    if (priceTags.isEmpty ())
    {
        QMessageBox::warning (this, tr ("Error"), tr ("No price tags to generate."));
        return;
    }
    QString filePath;
    bool success = false;
    if (outputFormatComboBox->currentIndex () == 0)
    {
        filePath = QFileDialog::getSaveFileName (this, tr ("Save Document"), QString (), tr ("Word DOCX Files (*.docx)"));

        if (filePath.isEmpty ())
            return;

        progressBar->setVisible (true);
        progressBar->setValue (0);
        success = wordGenerator->generateWordDocument (priceTags, filePath);
        progressBar->setValue (100);
    }
    else
    {
        filePath = QFileDialog::getSaveFileName (this, tr ("Save Excel Document"), QString (), tr ("Excel Files (*.xlsx)"));
        if (filePath.isEmpty ())
            return;

        progressBar->setVisible (true);
        progressBar->setValue (0);
        success = excelGenerator->generateExcelDocument (priceTags, filePath);
        progressBar->setValue (100);
    }
    if (success)
    {
        QMessageBox::information (this, tr ("Success"), tr ("Document has been generated successfully."));
    }
    else
    {
        QMessageBox::warning (this, tr ("Error"), tr ("Failed to generate the document."));
    }
    progressBar->setVisible (false);
}


void MainWindow::updatePreview ()
{
    // Placeholder: later bind template parameters to layout engine and data preview
}

void MainWindow::showStatistics () { updateStatistics (); }

void MainWindow::updateStatistics ()
{
    if (priceTags.isEmpty ())
    {
        statisticsText->setPlainText (tr ("No data loaded"));

        return;
    }

    QString stats;
    stats += tr ("=== PRICE TAG STATISTICS ===\n\n");

    // Basic statistics
    int totalProducts		  = priceTags.size ();
    int totalTags			  = 0;
    double totalValue		  = 0.0;
    double totalDiscountValue = 0.0;
    int productsWithDiscount  = 0;

    QSet<QString> brands;
    QSet<QString> categories;
    QSet<QString> suppliers;

    for (const PriceTag &tag : priceTags)
    {
        totalTags += tag.getQuantity ();
        totalValue += tag.getPrice () * tag.getQuantity ();

        if (tag.hasDiscount ())
        {
            totalDiscountValue += tag.getPrice2 () * tag.getQuantity ();
            productsWithDiscount++;
        }

        if (! tag.getBrand ().isEmpty ())
            brands.insert (tag.getBrand ());
        if (! tag.getCategory ().isEmpty ())
            categories.insert (tag.getCategory ());
        if (! tag.getSupplier ().isEmpty ())
            suppliers.insert (tag.getSupplier ());
    }

    stats += tr ("Total Products: %1\n").arg (totalProducts);
    stats += tr ("Total Price Tags: %1\n").arg (totalTags);
    stats += tr ("Total Value: %1 ₽\n").arg (QString::number (totalValue, 'f', 0));
    stats += tr ("Products with Discount: %1\n").arg (productsWithDiscount);
    stats += tr ("Total Discount Value: %1 ₽\n").arg (QString::number (totalDiscountValue, 'f', 0));
    stats += tr ("Unique Brands: %1\n").arg (brands.size ());
    stats += tr ("Unique Categories: %1\n").arg (categories.size ());
    stats += tr ("Unique Suppliers: %1\n\n").arg (suppliers.size ());


    // Brand breakdown
    if (! brands.isEmpty ())
    {
        stats += tr ("=== BRANDS ===\n");
        for (const QString &brand : brands)
            stats += tr ("• %1\n").arg (brand);
        stats += "\n";
    }

    // Category breakdown
    if (! categories.isEmpty ())
    {
        stats += tr ("=== CATEGORIES ===\n");
        for (const QString &category : categories)
        {
            stats += tr ("• %1\n").arg (category);
        }
        stats += "\n";
    }


    statisticsText->setPlainText (stats);
}
