#ifndef PLUGINLOADER_H_QWADUNZ7
#define PLUGINLOADER_H_QWADUNZ7

#include <nodes/DataModelRegistry>

template <typename T> void registerModel(QtNodes::DataModelRegistry& registry)
{
    registry.registerModel<typename T::Model>();
}

template <typename W, typename Z, typename... Args> void registerModel(QtNodes::DataModelRegistry& registry)
{
    registerModel<W>(registry);
    registerModel<Z, Args...>(registry);
}

template <typename T>
std::enable_if_t<std::is_same<typename T::PluginType, DevicePlugin>::value> addWidget(
    Ui::ProjectConfigPrivate& ui, const QColor& bg)
{
    ui.deviceWidget->layout()->addWidget(new IconLabel(T::name, T::Model::headerColor(), bg));
}
template <typename T>
std::enable_if_t<std::is_same<typename T::PluginType, CANRawPlugin>::value> addWidget(
    Ui::ProjectConfigPrivate& ui, const QColor& bg)
{
    ui.rawWidget->layout()->addWidget(new IconLabel(T::name, T::Model::headerColor(), bg));
}

template <typename W, typename Z, typename... Args> void addWidget(Ui::ProjectConfigPrivate& ui, const QColor& bg)
{
    addWidget<W>(ui, bg);
    addWidget<Z, Args...>(ui, bg);
}

template <typename... Args> struct PluginLoader {
    PluginLoader(QtNodes::DataModelRegistry& registry)
    {
        registerModel<Args...>(registry);
    }

    void addWidgets(Ui::ProjectConfigPrivate& ui, const QColor& bg)
    {
        addWidget<Args...>(ui, bg);
    }
};

#endif /* end of include guard: PLUGINLOADER_H_QWADUNZ7 */
