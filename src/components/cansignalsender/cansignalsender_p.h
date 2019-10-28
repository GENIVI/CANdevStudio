#ifndef CANSIGNALSENDER_P_H
#define CANSIGNALSENDER_P_H

#include "gui/cansignalsenderguiimpl.h"
#include "cansignalsender.h"
#include <memory>
#include <cantypes.hpp>
#include <QUuid>

class CanSignalSender;

class CanSignalSenderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalSender)

public:
    CanSignalSenderPrivate(CanSignalSender* q, CanSignalSenderCtx&& ctx = CanSignalSenderCtx(new CanSignalSenderGuiImpl));
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
    std::map<QUuid, QString> _dbNames;
    std::map<QUuid, CANmessages_t> _candb;

private:
    CanSignalSender* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};

#endif // CANSIGNALSENDER_P_H
