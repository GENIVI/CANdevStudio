#ifndef CANRAWPLAYERPLUGIN_H_G8RLJWGO
#define CANRAWPLAYERPLUGIN_H_G8RLJWGO

#include "plugin_type.h"
#include "canrawplayermodel.h"

using CanRawPlugin = PluginBase<typestring_is("Raw Layer"), 0x90bb3e, 58>;

struct CanRawPlayerPlugin {
    using Model = CanRawPlayerModel;
    static constexpr const char* name = "CanRawPlayer";
    using PluginType = CanRawPlugin;
};


#endif /* end of include guard: CANRAWPLAYERPLUGIN_H_G8RLJWGO */
