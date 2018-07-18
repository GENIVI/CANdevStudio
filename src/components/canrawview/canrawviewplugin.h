#ifndef CANRAWVIEWPLUGIN_H_C02HULVO
#define CANRAWVIEWPLUGIN_H_C02HULVO

#include "plugin_type.h"
#include "canrawviewmodel.h"

struct CanRawViewPlugin {
    using Model = CanRawViewModel;
    static constexpr const char* name = "CanRawView";
    using PluginType = CanRawPlugin;
};

#endif /* end of include guard: CANRAWVIEWPLUGIN_H_C02HULVO */
