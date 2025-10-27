#pragma once

#include "tagtemplate.h"

// Forward declarations
class QString;


class ConfigManager
{
public:
    static QString templateConfigFilePath ();

    static bool loadTemplate (TagTemplate &outTemplate);

    static bool saveTemplate (const TagTemplate &tpl);
};
