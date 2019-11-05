#ifndef CANSIGNALENCODERPLUGIN_H
#define CANSIGNALENCODERPLUGIN_H

#include "plugin_type.h"
#include "cansignalencodermodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using MiscPlugin = PluginBase<typestring_is("Misc Layer"), 0x555fc3, 57>;

struct CanSignalEncoderPlugin {
    using Model = CanSignalEncoderModel;
    static constexpr const char* name = "CanSignalEncoder";
    using PluginType = MiscPlugin;
};

#endif // CANSIGNALENCODERPLUGIN_H
