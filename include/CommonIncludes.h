#pragma once


// Constants
#include "Constants.h"

// Qt Base Tools
#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QObject>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QSettings>
#include <QString>
#include <QTranslator>

// Qt Events:
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>

// UI
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFont>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPalette>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
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
#endif

// Necessary for compatibility of builds with MinGw compilers for Qt 5 and 6 versions
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif
// Qt 6: types are available directly after including <QtCharts/...>


// Graphics
#include <QBrush>
#include <QColor>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>
#include <QRegion>

// Drag & Drop
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

// Containers
#include <QBitmap>
#include <QJsonArray>
#include <QList>
#include <QMap>
#include <QSet>
#include <QStringList>

// Files
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>
#include <QUrl>

// Std
#include <algorithm>
#include <cmath>
#include <utility>
