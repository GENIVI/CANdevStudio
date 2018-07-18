#ifndef CANRAWSENDERPLUGIN_H_M78FS6IV
#define CANRAWSENDERPLUGIN_H_M78FS6IV

#include "plugin_type.h"
#include "canrawsendermodel.h"

struct CanRawSenderPlugin {
    using Model = CanRawSenderModel;
    static constexpr const char* name = "CanRawSender";
    using PluginType = CanRawPlugin;
};


#endif /* end of include guard: CANRAWSENDERPLUGIN_H_M78FS6IV */
