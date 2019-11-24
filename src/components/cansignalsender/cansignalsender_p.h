#ifndef CANSIGNALSENDER_P_H
#define CANSIGNALSENDER_P_H

#include <QtCore/QObject>
#include <memory>
#include "gui/cansignalsenderguiimpl.h"
#include "cansignalsender.h"
#include <QStandardItemModel>
#include <cantypes.hpp>
#include <QUuid>
#include <candbhandler.h>
#include <propertyfields.h>

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
    CanDbHandler _db{ _props, _dbProperty };

private:
    QStandardItemModel _tvModel;
    QStringList _tvColumns;
    CanSignalSender* q_ptr;
    const QString _nameProperty = "name";
    const QString _dbProperty = "CAN database";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_dbProperty, QVariant::String, true, cf(std::bind(&CanDbHandler::createPropertyWidget, &_db)))
    };
};

#endif // CANSIGNALSENDER_P_H
