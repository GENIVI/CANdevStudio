#ifndef CANRAWLOGGERPLUGIN_H_QKYDNCFC
#define CANRAWLOGGERPLUGIN_H_QKYDNCFC

#include "plugin_type.h"
#include "canrawloggermodel.h"

using CanRawPlugin = PluginBase<typestring_is("Raw Layer"), 0x90bb3e, 58>;

struct CanRawLoggerPlugin {
    using Model = CanRawLoggerModel;
    static constexpr const char* name = "CanRawLogger";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANRAWLOGGERPLUGIN_H_QKYDNCFC */
