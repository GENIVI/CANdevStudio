#ifndef CANRAWVIEWPLUGIN_H_C02HULVO
#define CANRAWVIEWPLUGIN_H_C02HULVO

#include "plugin_type.h"
#include "canrawviewmodel.h"

using CanRawPlugin = PluginBase<typestring_is("Raw Layer"), 0x90bb3e, 58>;

struct CanRawViewPlugin {
    using Model = CanRawViewModel;
    static constexpr const char* name = "CanRawView";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANRAWVIEWPLUGIN_H_C02HULVO */
