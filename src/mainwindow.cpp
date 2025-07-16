#include "mainwindow.h"
#include <QFileInfo>
#include <QHeaderView>
#include "excelgenerator.h"


MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
{
    excelParser    = new ExcelParser (this);
    wordGenerator  = new WordGenerator (this);
    excelGenerator = new ExcelGenerator (this);

    setupUI ();

    setAcceptDrops (true);
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

    setupPreviewTab ();

    setupStatisticsTab ();
}

void MainWindow::setupMainTab ()
{
    QWidget *mainTab           = new QWidget ();
    QVBoxLayout *mainTabLayout = new QVBoxLayout (mainTab);

    // Create drop area label
    dropArea = new QLabel (tr ("Drag and drop Excel file here or use the Open button below"));
    dropArea->setAlignment (Qt::AlignCenter);
    dropArea->setMinimumHeight (200);
    dropArea->setStyleSheet ("QLabel { border: 2px dashed #aaa; border-radius: 5px; }");

    // Create buttons
    openButton     = new QPushButton (tr ("Open Excel File"));
    generateButton = new QPushButton (tr ("Generate Price Tags"));
    generateButton->setEnabled (false);

    // Create progress bar
    progressBar = new QProgressBar ();
    progressBar->setVisible (false);

    // Add widgets to layout
    mainTabLayout->addWidget (dropArea, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout ();
    buttonLayout->addWidget (openButton);
    buttonLayout->addWidget (generateButton);

    mainTabLayout->addLayout (buttonLayout);

    // Формат вывода
    outputFormatComboBox = new QComboBox ();
    outputFormatComboBox->addItem (tr ("Word/ODT"));
    outputFormatComboBox->addItem (tr ("Excel/XLSX"));
    mainTabLayout->addWidget (outputFormatComboBox);

    mainTabLayout->addWidget (progressBar);

    // Connect signals
    connect (openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect (generateButton, &QPushButton::clicked, this, &MainWindow::generateDocument);

    tabWidget->addTab (mainTab, tr ("Main"));
}

void MainWindow::setupPreviewTab ()
{
    QWidget *previewTab        = new QWidget ();
    QVBoxLayout *previewLayout = new QVBoxLayout (previewTab);

    // Create preview table
    previewTable = new QTableWidget ();
    previewTable->setColumnCount (8);
    previewTable->setHorizontalHeaderLabels ({
                                                 tr ("Brand"),
                                                 tr ("Category"),
                                                 tr ("Price"),
                                                 tr ("Price 2"),
                                                 tr ("Article"),
                                                 tr ("Size"),
                                                 tr ("Material"),
                                                 tr ("Supplier")});

    // Set table properties
    previewTable->setAlternatingRowColors (true);
    previewTable->horizontalHeader ()->setStretchLastSection (true);
    previewTable->setSelectionBehavior (QAbstractItemView::SelectRows);

    // Create refresh button
    refreshPreviewButton = new QPushButton (tr ("Refresh Preview"));
    refreshPreviewButton->setEnabled (false);

    previewLayout->addWidget (previewTable, 1);
    previewLayout->addWidget (refreshPreviewButton);

    connect (refreshPreviewButton, &QPushButton::clicked, this, &MainWindow::updatePreview);

    tabWidget->addTab (previewTab, tr ("Preview"));
}

void MainWindow::setupStatisticsTab ()
{
    QWidget *statsTab        = new QWidget ();
    QVBoxLayout *statsLayout = new QVBoxLayout (statsTab);

    // Create statistics text area
    statisticsText = new QTextEdit ();
    statisticsText->setReadOnly (true);

    // Create refresh button
    refreshStatsButton = new QPushButton (tr ("Refresh Statistics"));
    refreshStatsButton->setEnabled (false);

    statsLayout->addWidget (statisticsText, 1);
    statsLayout->addWidget (refreshStatsButton);

    connect (refreshStatsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);

    tabWidget->addTab (statsTab, tr ("Statistics"));
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
                dropArea->setStyleSheet ("QLabel { border: 2px dashed #4CAF50; border-radius: 5px; }");
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

    dropArea->setStyleSheet ("QLabel { border: 2px dashed #aaa; border-radius: 5px; }");

    processFile (filePath);

    event->acceptProposedAction ();
}


void MainWindow::openFile ()
{
    QString filePath = QFileDialog::getOpenFileName (this, tr ("Open Excel File"), QString (), tr ("Excel Files (*.xlsx)"));

    if (! filePath.isEmpty ()) { processFile (filePath); }
}


void MainWindow::processFile (const QString &filePath)
{
    currentFilePath = filePath;
    dropArea->setText (tr ("File: %1").arg (QFileInfo (filePath).fileName ()));

    // Parse the Excel file and extract price tags
    priceTags.clear ();
    bool success = excelParser->parseExcelFile (filePath, priceTags);

    if (success && ! priceTags.isEmpty ())
    {
        int totalTags = 0;
        for (const PriceTag &tag : priceTags) { totalTags += tag.getQuantity (); }

        dropArea->setText (tr ("Loaded %1 products with a total of %2 price tags.").arg (priceTags.size ()).arg (totalTags));
        generateButton->setEnabled (true);
        refreshPreviewButton->setEnabled (true);
        refreshStatsButton->setEnabled (true);

        // Update preview and statistics
        updatePreview ();
        updateStatistics ();
    }
    else
    {
        QMessageBox::warning (this, tr ("Error"), tr ("Failed to parse the Excel file or no products found."));
        dropArea->setText (tr ("Drag and drop Excel file here or use the Open button below"));
        generateButton->setEnabled (false);
        refreshPreviewButton->setEnabled (false);
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
        // Word/ODT
        filePath = QFileDialog::getSaveFileName (this, tr ("Save Document"), QString (), tr ("OpenDocument Files (*.odt)"));
        if (filePath.isEmpty ())
            return;
        progressBar->setVisible (true);
        progressBar->setValue (0);
        success = wordGenerator->generateWordDocument (priceTags, filePath);
        progressBar->setValue (100);
    }
    else
    {
        // Excel/XLSX
        filePath = QFileDialog::getSaveFileName (this, tr ("Save Excel Document"), QString (), tr ("Excel Files (*.xlsx)"));
        if (filePath.isEmpty ())
            return;
        progressBar->setVisible (true);
        progressBar->setValue (0);
        success = excelGenerator->generateExcelDocument (priceTags, filePath);
        progressBar->setValue (100);
    }
    if (success) { QMessageBox::information (this, tr ("Success"), tr ("Document has been generated successfully.")); }
    else { QMessageBox::warning (this, tr ("Error"), tr ("Failed to generate the document.")); }
    progressBar->setVisible (false);
}


void MainWindow::updatePreview ()
{
    if (priceTags.isEmpty ())
    {
        previewTable->setRowCount (0);
        return;
    }

    previewTable->setRowCount (priceTags.size ());

    for (int i = 0; i < priceTags.size (); ++i)
    {
        const PriceTag &tag = priceTags[i];

        previewTable->setItem (i, 0, new QTableWidgetItem (tag.getBrand ()));
        previewTable->setItem (i, 1, new QTableWidgetItem (tag.getFormattedCategory ()));
        previewTable->setItem (i, 2, new QTableWidgetItem (QString::number (tag.getPrice (), 'f', 0) + " ₽"));
        previewTable->setItem (i, 3, new QTableWidgetItem (tag.hasDiscount () ? QString::number (tag.getPrice2 (), 'f', 0) + " ₽" : ""));
        previewTable->setItem (i, 4, new QTableWidgetItem (tag.getArticle ()));
        previewTable->setItem (i, 5, new QTableWidgetItem (tag.getSize ()));
        previewTable->setItem (i, 6, new QTableWidgetItem (tag.getMaterial ()));
        previewTable->setItem (i, 7, new QTableWidgetItem (tag.getSupplier ()));
    }

    previewTable->resizeColumnsToContents ();
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
    int totalProducts         = priceTags.size ();
    int totalTags             = 0;
    double totalValue         = 0.0;
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
        for (const QString &category : categories) { stats += tr ("• %1\n").arg (category); }
        stats += "\n";
    }

    statisticsText->setPlainText (stats);
}
