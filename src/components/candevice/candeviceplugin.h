#ifndef CANDEVICEPLUGIN_H_R6ODXXIJ
#define CANDEVICEPLUGIN_H_R6ODXXIJ

#include "plugin_type.h"

struct CANDevicePlugin {
    using Model = CanDeviceModel;
    static constexpr const char* name = "CanDevice";
    using PluginType = DevicePlugin;
};

#endif /* end of include guard: CANDEVICEPLUGIN_H_R6ODXXIJ */
