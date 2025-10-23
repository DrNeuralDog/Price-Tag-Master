#pragma once

#include "CommonIncludes.h"


enum class AppTheme
{
    Light = 0,
    Dark  = 1
};


class ThemeManager
{
public:
    static AppTheme loadThemeFromSettings (QSettings &settings);

    static void saveThemeToSettings (QSettings &settings, AppTheme theme);

    static void applyThemeToApplication (AppTheme theme);

    static AppTheme currentTheme ();


private:
    static QPalette buildLightPalette ();
    static QPalette buildDarkPalette ();

    static QString buildLightQss ();
    static QString buildDarkQss ();
};


