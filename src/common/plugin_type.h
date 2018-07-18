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

#endif /* end of include guard: PLUGIN_TYPE_H_AGCRIYX2 */
