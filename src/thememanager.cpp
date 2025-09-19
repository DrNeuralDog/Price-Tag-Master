#include "thememanager.h"

#include <QGuiApplication>
#include <QPalette>


namespace
{
    constexpr const char *kSettingsKey = "ui/theme";
}


AppTheme ThemeManager::loadThemeFromSettings (QSettings &settings)
{
    const int value = settings.value (kSettingsKey, static_cast<int> (AppTheme::Light)).toInt ();

    return static_cast<AppTheme> (value);
}

void ThemeManager::saveThemeToSettings (QSettings &settings, AppTheme theme) { settings.setValue (kSettingsKey, static_cast<int> (theme)); }

void ThemeManager::applyThemeToApplication (AppTheme theme)
{
    const bool isDark = (theme == AppTheme::Dark);
    QPalette palette  = isDark ? buildDarkPalette () : buildLightPalette ();
    qApp->setPalette (palette);


    const QString qss = isDark ? buildDarkQss () : buildLightQss ();
    qApp->setStyleSheet (qss);
}

AppTheme ThemeManager::currentTheme ()
{
    const QColor windowColor = qApp->palette ().color (QPalette::Window);
    // Dark window colors are typically near #0F172A; threshold by lightness


    return (windowColor.lightness () < 64) ? AppTheme::Dark : AppTheme::Light;
}

QPalette ThemeManager::buildLightPalette ()
{
    QPalette p;

    p.setColor (QPalette::Window, QColor (0xF8, 0xFA, 0xFC)); // bg/canvas
    p.setColor (QPalette::Base, QColor (0xFF, 0xFF, 0xFF));	  // surface
    p.setColor (QPalette::AlternateBase, QColor (0xF1, 0xF5, 0xF9));
    p.setColor (QPalette::Text, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::WindowText, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::Button, QColor (0xFF, 0xFF, 0xFF));
    p.setColor (QPalette::ButtonText, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::ToolTipBase, QColor (0xFF, 0xFF, 0xC0));
    p.setColor (QPalette::ToolTipText, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::BrightText, QColor (0xFF, 0xFF, 0xFF));
    p.setColor (QPalette::Highlight, QColor (0x3B, 0x82, 0xF6)); // brand/primary
    p.setColor (QPalette::HighlightedText, QColor (0xFF, 0xFF, 0xFF));
    p.setColor (QPalette::Disabled, QPalette::Text, QColor (0x94, 0xA3, 0xB8));
    p.setColor (QPalette::Disabled, QPalette::ButtonText, QColor (0x94, 0xA3, 0xB8));


    return p;
}

QPalette ThemeManager::buildDarkPalette ()
{
    QPalette p;
    p.setColor (QPalette::Window, QColor (0x0B, 0x12, 0x20)); // bg/canvas
    p.setColor (QPalette::Base, QColor (0x0F, 0x17, 0x2A));	  // surface
    p.setColor (QPalette::AlternateBase, QColor (0x1F, 0x29, 0x37));
    p.setColor (QPalette::Text, QColor (0xE5, 0xE7, 0xEB));
    p.setColor (QPalette::WindowText, QColor (0xE5, 0xE7, 0xEB));
    p.setColor (QPalette::Button, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::ButtonText, QColor (0xE5, 0xE7, 0xEB));
    p.setColor (QPalette::ToolTipBase, QColor (0x33, 0x41, 0x55));
    p.setColor (QPalette::ToolTipText, QColor (0xE5, 0xE7, 0xEB));
    p.setColor (QPalette::BrightText, QColor (0xFF, 0xFF, 0xFF));
    p.setColor (QPalette::Highlight, QColor (0x60, 0xA5, 0xFA)); // focus
    p.setColor (QPalette::HighlightedText, QColor (0x0F, 0x17, 0x2A));
    p.setColor (QPalette::Disabled, QPalette::Text, QColor (0x64, 0x74, 0x8B));
    p.setColor (QPalette::Disabled, QPalette::ButtonText, QColor (0x64, 0x74, 0x8B));
    return p;
}

QString ThemeManager::buildLightQss ()
{
    const char *qss = R"(
     QWidget { color: #0F172A; background-color: #F8FAFC; }
     QToolBar { background: #FFFFFF; border-bottom: 1px solid #E2E8F0; }
     QTabWidget::pane { border: 1px solid #E2E8F0; background: #FFFFFF; border-radius: 10px; }
     QTabBar::tab { background: #FFFFFF; color: #0F172A; padding: 6px 10px; border: 1px solid #E2E8F0; border-bottom: none; border-top-left-radius: 10px; border-top-right-radius: 10px; }
     QTabBar::tab:selected { background: #F1F5F9; }
     QPushButton { height: 32px; padding: 0 12px; border-radius: 10px; border: 1px solid #CBD5E1; background: #FFFFFF; color: #0F172A; font-weight: 600; }
     QPushButton:hover { background: #F1F5F9; }
     QPushButton:disabled { background: #F1F5F9; color: #94A3B8; border-color: #E2E8F0; }
     /* Primary button helper via objectName="primary" */
     QPushButton#primary {
         color: #FFFFFF;
         border: 1px solid #3B82F6; /* blue-500 */
         /* diagonal gradient TL->BR, smaller contrast */
         background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                     stop:0 #2C67EE, /* slightly lighter than blue-600 */
                                     stop:1 #7FB6FD  /* muted blue-300 */);
     }
     QPushButton#primary:hover {
         filter: brightness(1.05);
     }
     QPushButton#primary:pressed {
         background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                     stop:0 #1F4ED8,
                                     stop:1 #5AA3FA);
     }
     QLineEdit, QComboBox, QTextEdit, QPlainTextEdit { height: 32px; border: 1px solid #E2E8F0; border-radius: 10px; background: #FFFFFF; color: #0F172A; padding: 2px 8px; }
     QProgressBar { border: 1px solid #E2E8F0; border-radius: 10px; text-align: center; background: #FFFFFF; }
     QProgressBar::chunk { background-color: #3B82F6; }
     QTableWidget { background: #FFFFFF; alternate-background-color: #F1F5F9; gridline-color: #E2E8F0; }
     QHeaderView::section { background: #F1F5F9; color: #0F172A; border: 1px solid #E2E8F0; padding: 4px 6px; border-radius: 6px; }
     QToolTip { background: #FFFFC0; color: #0F172A; }
     QGroupBox { border: 1px solid #E2E8F0; border-radius: 10px; margin-top: 8px; }
     QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }
     QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
     QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 5px; min-height: 24px; }
     QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
     QScrollBar::handle:horizontal { background: #CBD5E1; border-radius: 5px; min-width: 24px; }
     )";

    return QString::fromUtf8 (qss);
}


QString ThemeManager::buildDarkQss ()
{
    const char *qss = R"(
     QWidget { color: #E5E7EB; background-color: #0B1220; }
     QToolBar { background: #0F172A; border-bottom: 1px solid #334155; }
     QTabWidget::pane { border: 1px solid #334155; background: #0F172A; border-radius: 10px; }
     QTabBar::tab { background: #0F172A; color: #E5E7EB; padding: 6px 10px; border: 1px solid #334155; border-bottom: none; border-top-left-radius: 10px; border-top-right-radius: 10px; }
     QTabBar::tab:selected { background: #1F2937; }
     QPushButton { height: 32px; padding: 0 12px; border-radius: 10px; border: 1px solid #475569; background: #0F172A; color: #E5E7EB; font-weight: 600; }
     QPushButton:hover { background: #1F2937; }
     QPushButton:disabled { background: #111827; color: #64748B; border-color: #334155; }
     /* Primary action: violet-pink gradient */
     QPushButton#primary {
         color: #FFFFFF;
         border: 1px solid #7C3AED; /* violet-600 */
         background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                     stop:0 #EC4899, /* pink-500 */
                                     stop:1 #8B5CF6  /* violet-500 */);
     }
     QPushButton#primary:hover {
         filter: brightness(1.05);
     }
     QPushButton#primary:pressed {
         background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                     stop:0 #DB2777,
                                     stop:1 #7C3AED);
     }
     QLineEdit, QComboBox, QTextEdit, QPlainTextEdit { height: 32px; border: 1px solid #334155; border-radius: 10px; background: #0F172A; color: #E5E7EB; padding: 2px 8px; }
     QProgressBar { border: 1px solid #334155; border-radius: 10px; text-align: center; background: #0F172A; }
     QProgressBar::chunk { background-color: #60A5FA; }
     QTableWidget { background: #0F172A; alternate-background-color: #1F2937; gridline-color: #334155; }
     QHeaderView::section { background: #1F2937; color: #E5E7EB; border: 1px solid #334155; padding: 4px 6px; border-radius: 6px; }
     QToolTip { background: #334155; color: #E5E7EB; }
     QGroupBox { border: 1px solid #334155; border-radius: 10px; margin-top: 8px; }
     QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }
     QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
     QScrollBar::handle:vertical { background: #475569; border-radius: 5px; min-height: 24px; }
     QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
     QScrollBar::handle:horizontal { background: #475569; border-radius: 5px; min-width: 24px; }
     )";

    return QString::fromUtf8 (qss);
}
