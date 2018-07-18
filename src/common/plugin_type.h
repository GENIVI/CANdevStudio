#ifndef PLUGIN_TYPE_H_AGCRIYX2
#define PLUGIN_TYPE_H_AGCRIYX2

#include <QColor>
#include <QCheckBox>
#include <typestring.hh>

template<typename Name, int Color, int Space>
struct PluginBase {
    static const char* sectionName()
    {
        return Name::data();
    }

    static QColor sectionColor() 
    {
        return { Color };
    }

    static int spacing()
    {
        return Space;
    }
};

using DevicePlugin = PluginBase<typestring_is("Device Layer"), 0xf7aa1b, 43>;
using CanRawPlugin = PluginBase<typestring_is("Raw Layer"), 0x90bb3e, 58>;

#endif /* end of include guard: PLUGIN_TYPE_H_AGCRIYX2 */
