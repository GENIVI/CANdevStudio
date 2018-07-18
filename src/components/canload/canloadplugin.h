#ifndef CANLOADPLUGIN_H_8T2GEZMW
#define CANLOADPLUGIN_H_8T2GEZMW

#include "plugin_type.h"
#include "canloadmodel.h"

struct CANLoadPlugin {
    using Model = CanLoadModel;
    static constexpr const char* name = "CanLoad";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANLOADPLUGIN_H_8T2GEZMW */
