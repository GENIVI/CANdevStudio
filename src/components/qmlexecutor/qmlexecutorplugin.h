#ifndef QMLEXECUTORPLUGIN_H
#define QMLEXECUTORPLUGIN_H

#include "plugin_type.h"
#include "qmlexecutormodel.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using MiscPlugin = PluginBase<typestring_is("Misc Layer"), 0x555fc3, 57>;

struct QMLExecutorPlugin {
    using Model = QMLExecutorModel;
    static constexpr const char* name = "QMLExecutor";
    using PluginType = MiscPlugin;
};

#endif // QMLEXECUTORPLUGIN_H
