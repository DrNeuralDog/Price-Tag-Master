#pragma once

#include <QAction>
#include <QComboBox>
#include <QDragEnterEvent>
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

#include "excelgenerator.h"
#include "excelparser.h"
#include "pricetag.h"
#include "templateeditor.h"
#include "templateeditordialog.h"
#include "thememanager.h"
#include "wordgenerator.h"


class MainWindow: public QMainWindow
{
    Q_OBJECT //


            public: explicit MainWindow (QWidget *parent = nullptr);
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
    void setupPreviewTab (); // no-op (legacy)
    void setupStatisticsTab ();
    void setupToolbar ();
    void updateStatistics ();
    void toggleTheme ();
    void updateThemeStyles ();
    void applyTemplateToGenerators (const TagTemplate &tpl);

    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QToolBar *mainToolbar;
    QAction *toggleThemeAction;

    // Main tab
    QLabel *dropArea;
    QPushButton *openButton;
    QPushButton *generateButton;
    QProgressBar *progressBar;


    TemplateEditorDialog *templateEditorDialog = nullptr; // dialog instance

    QTextEdit *statisticsText;
    QPushButton *refreshStatsButton;

    QString currentFilePath;
    ExcelParser *excelParser;
    WordGenerator *wordGenerator;
    ExcelGenerator *excelGenerator;
    QList<PriceTag> priceTags;
    QComboBox *outputFormatComboBox;

    QSettings settings;

    TagTemplate currentTemplate; // last template from editor
};
