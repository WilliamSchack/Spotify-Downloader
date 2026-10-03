#ifndef SETTINGSSCREENGUI_H
#define SETTINGSSCREENGUI_H

#include "Config.h"
#include "EasyQProperties.h"

#include <QObject>

class SettingsScreenGUI : public QObject
{
    Q_OBJECT
    ENUM_QPROPERTY(EExtension, EEXTENSION_NAMES, codec, Codec, Config::CODEC_EXTENSION)
    SIMPLE_QPROPERTY(bool, overwrite, Overwrite, Config::OVERWRITE)
};

#endif