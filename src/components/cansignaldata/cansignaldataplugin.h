#ifndef CANSIGNALDATAPLUGIN_H
#define CANSIGNALDATAPLUGIN_H

#include "plugin_type.h"
#include "cansignaldatamodel.h"

using MiscPlugin = PluginBase<typestring_is("Misc Layer"), 0x555fc3, 57>;

struct CanSignalDataPlugin {
    using Model = CanSignalDataModel;
    static constexpr const char* name = "CanSignalData";
    using PluginType = MiscPlugin;
};

#endif // CANSIGNALDATAPLUGIN_H
