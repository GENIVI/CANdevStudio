#ifndef CANSIGNALDECODERPLUGIN_H
#define CANSIGNALDECODERPLUGIN_H

#include "plugin_type.h"
#include "cansignaldecodermodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using SignalPlugin = PluginBase<typestring_is("Signal Layer"), 0x1492ca, 47>;

struct CanSignalDecoderPlugin {
    using Model = CanSignalDecoderModel;
    static constexpr const char* name = "CanSignalDecoder";
    using PluginType = SignalPlugin;
};

#endif // CANSIGNALDECODERPLUGIN_H
