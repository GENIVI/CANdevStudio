#ifndef CANSIGNALSENDERPLUGIN_H
#define CANSIGNALSENDERPLUGIN_H

#include "plugin_type.h"
#include "cansignalsendermodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using SignalPlugin = PluginBase<typestring_is("Signal Layer"), 0x1492ca, 47>;

struct CanSignalSenderPlugin {
    using Model = CanSignalSenderModel;
    static constexpr const char* name = "CanSignalSender";
    using PluginType = SignalPlugin;
};

#endif // CANSIGNALSENDERPLUGIN_H
