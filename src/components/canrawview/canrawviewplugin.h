#ifndef CANRAWVIEWPLUGIN_H_C02HULVO
#define CANRAWVIEWPLUGIN_H_C02HULVO

#include "plugin_type.h"

struct CanRawViewPlugin {
    using Model = CanRawViewModel;
    static constexpr const char* name = "CanRawView";
    using PluginType = CANRawPlugin;
};

#endif /* end of include guard: CANRAWVIEWPLUGIN_H_C02HULVO */
