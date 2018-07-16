#ifndef CANRAWFILTERPLUGIN_H_MGJSE2A9
#define CANRAWFILTERPLUGIN_H_MGJSE2A9

#include "plugin_type.h"
#include "canrawfiltermodel.h"

struct CanRawFilterPlugin {
    using Model = CanRawFilterModel;
    static constexpr const char* name = "CanRawFilter";
    using PluginType = CANRawPlugin;
};

#endif /* end of include guard: CANRAWFILTERPLUGIN_H_MGJSE2A9 */
