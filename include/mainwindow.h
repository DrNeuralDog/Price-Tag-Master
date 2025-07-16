#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QString>
#include <QProgressBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QComboBox>

#include "excelparser.h"
#include "wordgenerator.h"
#include "pricetag.h"
#include "excelgenerator.h"


class MainWindow: public QMainWindow
{
    Q_OBJECT //


public:
    explicit MainWindow (QWidget *parent = nullptr);
    ~MainWindow ();


protected:
    void dragEnterEvent (QDragEnterEvent *event) override;
    void dropEvent (QDropEvent *event) override;


private slots:
    void openFile ();
    void generateDocument ();
    void processFile (const QString &filePath);
    void updatePreview ();
    void showStatistics ();


private:
    void setupUI ();
    void setupMainTab ();
    void setupPreviewTab ();
    void setupStatisticsTab ();
    void updateStatistics ();

    QWidget *centralWidget;
    QTabWidget *tabWidget;

    // Main tab
    QLabel *dropArea;
    QPushButton *openButton;
    QPushButton *generateButton;
    QProgressBar *progressBar;

    // Preview tab
    QTableWidget *previewTable;
    QPushButton *refreshPreviewButton;

    // Statistics tab
    QTextEdit *statisticsText;
    QPushButton *refreshStatsButton;

    QString currentFilePath;
    ExcelParser *excelParser;
    WordGenerator *wordGenerator;
    ExcelGenerator *excelGenerator;
    QList<PriceTag> priceTags;
    QComboBox *outputFormatComboBox;
};

