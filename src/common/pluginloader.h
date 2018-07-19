#ifndef PLUGINLOADER_H_QWADUNZ7
#define PLUGINLOADER_H_QWADUNZ7

#include <nodes/DataModelRegistry>

#include "plugin_type.h"

template <class T, class Tuple> struct section_ndx;

template <class T, class... Types> struct section_ndx<T, std::tuple<T, Types...>> {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types> struct section_ndx<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + section_ndx<T, std::tuple<Types...>>::value;
};

template <typename T> void registerModel(QtNodes::DataModelRegistry& registry)
{
    registry.registerModel<typename T::Model>();
}

template <typename W, typename Z, typename... Args> void registerModel(QtNodes::DataModelRegistry& registry)
{
    registerModel<W>(registry);
    registerModel<Z, Args...>(registry);
}

template <typename Array, typename... Args> struct SectionLoader;

template <typename Array, typename... Args> struct SectionLoader<Array, std::tuple<Args...>> {
    SectionLoader(Array& widgets, Ui::ProjectConfigPrivate& ui)
        : _widgets(widgets)
    {
        initSection<Args...>(ui);
    }

private:
    template <typename W, typename Z, typename... Secs> void initSection(Ui::ProjectConfigPrivate& ui)
    {
        initSection<W>(ui);
        initSection<Z, Secs...>(ui);
    }

    template <typename T> void initSection(Ui::ProjectConfigPrivate& ui)
    {
        constexpr size_t ndx = section_ndx<T, std::tuple<Args...>>::value;

        static_assert(ndx < std::tuple_size<std::remove_reference_t<decltype(_widgets)>>::value, "Index out of bounds");

        auto pb = new QCheckBox(T::sectionName(), ui.frame);
        pb->setLayoutDirection(Qt::RightToLeft);
        pb->setStyleSheet(QString("spacing: ") + QString::number(T::spacing()) + "px;");
        pb->setProperty("type", "sectionHeader");
        pb->setChecked(true);
        ui.verticalLayout->addWidget(pb);

        auto wdg = new QWidget(ui.frame);
        wdg->setProperty("type", "sectionWidget");
        auto wdgLayout = new QVBoxLayout(wdg);
        wdgLayout->setSpacing(10);
        wdgLayout->setContentsMargins(0, 0, 0, 0);
        ui.verticalLayout->addWidget(wdg);
        _widgets[ndx] = wdg;

        QObject::connect(pb, &QPushButton::toggled, wdg, &QWidget::setVisible);
    }

    Array& _widgets;
};

template <typename S, typename... Args> struct PluginLoader {
    PluginLoader(QtNodes::DataModelRegistry& registry)
    {
        registerModel<Args...>(registry);
    }

    void initSections(Ui::ProjectConfigPrivate& ui)
    {
        SectionLoader<decltype(_widgets), S> sections(_widgets, ui);

        auto spacer = new QSpacerItem(17, 410, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui.verticalLayout->addItem(spacer);
    }

    void addWidgets(const QColor& bg)
    {
        addWidget<Args...>(bg);
    }

    void clearSections()
    {
        for (auto wdg : _widgets) {
            QLayoutItem* item;
            while ((item = wdg->layout()->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
        }
    };

private:
    template <typename T> void addWidget(const QColor& bg)
    {
        constexpr size_t ndx = section_ndx<typename T::PluginType, S>::value;

        static_assert(ndx < std::tuple_size<decltype(_widgets)>::value, "Index out of bounds");

        if (_widgets[ndx]) {
            _widgets[ndx]->layout()->addWidget(new IconLabel(T::name, T::PluginType::sectionColor(), bg));
        } else {
            cds_error("Sections not initialized");
        }
    }

    template <typename W, typename Z, typename... Plugs> void addWidget(const QColor& bg)
    {
        addWidget<W>(bg);
        addWidget<Z, Plugs...>(bg);
    }

    std::array<QWidget*, std::tuple_size<S>::value> _widgets {};
};

#endif /* end of include guard: PLUGINLOADER_H_QWADUNZ7 */
