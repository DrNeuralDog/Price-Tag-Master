#pragma once

#include <QString>
#include "tagtemplate.h"


class ConfigManager
{
public:
    // Returns full path to the template configuration JSON file
    static QString templateConfigFilePath ();

    // Load template from JSON file; returns false if file missing or invalid
    static bool loadTemplate (TagTemplate &outTemplate);

    // Save template to JSON file (creates directories as needed)
    static bool saveTemplate (const TagTemplate &tpl);
};


