#ifndef CANDEVICEPLUGIN_H_R6ODXXIJ
#define CANDEVICEPLUGIN_H_R6ODXXIJ

#include "plugin_type.h"
#include "candevicemodel.h"

using DevicePlugin = PluginBase<typestring_is("Device Layer"), 0xf7aa1b, 43>;

struct CanDevicePlugin {
    using Model = CanDeviceModel;
    static constexpr const char* name = "CanDevice";
    using PluginType = DevicePlugin;
};

#endif /* end of include guard: CANDEVICEPLUGIN_H_R6ODXXIJ */
