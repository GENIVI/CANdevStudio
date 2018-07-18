#ifndef CANRAWLOGGERPLUGIN_H_QKYDNCFC
#define CANRAWLOGGERPLUGIN_H_QKYDNCFC

#include "plugin_type.h"
#include "canrawloggermodel.h"

struct CanRawLoggerPlugin {
    using Model = CanRawLoggerModel;
    static constexpr const char* name = "CanRawLogger";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANRAWLOGGERPLUGIN_H_QKYDNCFC */
