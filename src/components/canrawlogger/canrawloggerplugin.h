#ifndef CANRAWLOGGERPLUGIN_H_QKYDNCFC
#define CANRAWLOGGERPLUGIN_H_QKYDNCFC

#include "plugin_type.h"

struct CanRawLoggerPlugin {
    using Model = CanRawLoggerModel;
    static constexpr const char* name = "CanRawLogger";
    using PluginType = CANRawPlugin;
};

#endif /* end of include guard: CANRAWLOGGERPLUGIN_H_QKYDNCFC */
