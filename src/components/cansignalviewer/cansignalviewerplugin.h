#ifndef CANSIGNALVIEWERPLUGIN_H
#define CANSIGNALVIEWERPLUGIN_H

#include "plugin_type.h"
#include "cansignalviewermodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using SignalPlugin = PluginBase<typestring_is("Signal Layer"), 0x1492ca, 47>;

struct CanSignalViewerPlugin {
    using Model = CanSignalViewerModel;
    static constexpr const char* name = "CanSignalViewer";
    using PluginType = SignalPlugin;
};

#endif // CANSIGNALVIEWERPLUGIN_H
