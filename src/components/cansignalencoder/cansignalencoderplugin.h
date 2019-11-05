#ifndef CANSIGNALENCODERPLUGIN_H
#define CANSIGNALENCODERPLUGIN_H

#include "plugin_type.h"
#include "cansignalencodermodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using SignalPlugin = PluginBase<typestring_is("Signal Layer"), 0x1492ca, 47>;

struct CanSignalEncoderPlugin {
    using Model = CanSignalEncoderModel;
    static constexpr const char* name = "CanSignalEncoder";
    using PluginType = SignalPlugin;
};

#endif // CANSIGNALENCODERPLUGIN_H
