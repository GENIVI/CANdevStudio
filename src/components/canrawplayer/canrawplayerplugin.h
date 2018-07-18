#ifndef CANRAWPLAYERPLUGIN_H_G8RLJWGO
#define CANRAWPLAYERPLUGIN_H_G8RLJWGO

#include "plugin_type.h"
#include "canrawplayermodel.h"

struct CanRawPlayerPlugin {
    using Model = CanRawPlayerModel;
    static constexpr const char* name = "CanRawPlayer";
    using PluginType = CanRawPlugin;
};


#endif /* end of include guard: CANRAWPLAYERPLUGIN_H_G8RLJWGO */
