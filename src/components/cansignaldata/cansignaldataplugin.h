#ifndef CANSIGNALDATAPLUGIN_H
#define CANSIGNALDATAPLUGIN_H

#include "plugin_type.h"
#include "cansignaldatamodel.h"

using SignalPlugin = PluginBase<typestring_is("Signal Layer"), 0x1492ca, 47>;

struct CanSignalDataPlugin {
    using Model = CanSignalDataModel;
    static constexpr const char* name = "CanSignalData";
    using PluginType = SignalPlugin;
};

#endif // CANSIGNALDATAPLUGIN_H
