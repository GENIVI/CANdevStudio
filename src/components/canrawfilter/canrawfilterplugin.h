#ifndef CANRAWFILTERPLUGIN_H_MGJSE2A9
#define CANRAWFILTERPLUGIN_H_MGJSE2A9

#include "plugin_type.h"
#include "canrawfiltermodel.h"

using CanRawPlugin = PluginBase<typestring_is("Raw Layer"), 0x90bb3e, 58>;

struct CanRawFilterPlugin {
    using Model = CanRawFilterModel;
    static constexpr const char* name = "CanRawFilter";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANRAWFILTERPLUGIN_H_MGJSE2A9 */
