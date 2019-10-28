#ifndef CANSIGNALSENDER_P_H
#define CANSIGNALSENDER_P_H

#include "cansignalsender.h"
#include "gui/cansignalsenderguiimpl.h"
#include <QUuid>
#include <cantypes.hpp>
#include <log.h>
#include <memory>
#include <propertyfields.h>
#include <candbhandler.h>

class CanSignalSender;

class CanSignalSenderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalSender)

public:
    CanSignalSenderPrivate(
        CanSignalSender* q, CanSignalSenderCtx&& ctx = CanSignalSenderCtx(new CanSignalSenderGuiImpl));
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanSignalSenderCtx _ctx;
    CanSignalSenderGuiInt& _ui;
    bool _docked{ true };
    std::map<QString, QVariant> _props;
    CanDbHandler _db;

private:
    CanSignalSender* q_ptr;
    const QString _nameProperty = "name";
    const QString _dbProperty = "CAN database";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),

            std::make_tuple(_dbProperty, QVariant::String, true, cf([this] {
                auto *p = new PropertyFieldCombo();

                for (auto &item : _db._dbNames) {
                    p->addItem(item.second, item.first);
                }

                return p;
            }))
    };
    // clang-format on
};

#endif // CANSIGNALSENDER_P_H
