#ifndef PLUGIN_TYPE_H_AGCRIYX2
#define PLUGIN_TYPE_H_AGCRIYX2

#include <QColor>
#include <QCheckBox>
#include <typestring.hh>

enum class PluginType
{
    DevicePlugin = 0,
    CanRawPlugin
};

template<PluginType Type, typename Name, int Color, int Space>
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

    static PluginType type()
    {
        return Type;
    }

    static int typeNdx()
    {
        return static_cast<typename std::underlying_type<PluginType>::type>(Type);
    }
};

using DevicePlugin = PluginBase<PluginType::DevicePlugin, typestring_is("Device Layer"), 0xf7aa1b, 43>;
using CanRawPlugin = PluginBase<PluginType::CanRawPlugin, typestring_is("Raw Layer"), 0x90bb3e, 58>;

#endif /* end of include guard: PLUGIN_TYPE_H_AGCRIYX2 */
