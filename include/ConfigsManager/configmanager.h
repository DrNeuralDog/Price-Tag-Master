#pragma once

#include "tagtemplate.h"


class ConfigManager
{
public:
    static QString templateConfigFilePath ();

    static bool loadTemplate (TagTemplate &outTemplate);

    static bool saveTemplate (const TagTemplate &tpl);
};
