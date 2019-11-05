#ifndef CANSIGNALENCODER_P_H
#define CANSIGNALENCODER_P_H

#include "cansignalencoder.h"
#include <memory>
#include <QUuid>
#include <candbhandler.h>
#include <propertyfields.h>

class CanSignalEncoder;

class CanSignalEncoderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalEncoder)

public:
    CanSignalEncoderPrivate(CanSignalEncoder* q, CanSignalEncoderCtx&& ctx = CanSignalEncoderCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanSignalEncoderCtx _ctx;
    std::map<QString, QVariant> _props;
    CanDbHandler _db{ _props, _dbProperty };

private:
    CanSignalEncoder* q_ptr;
    const QString _nameProperty = "name";
    const QString _dbProperty = "CAN database";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_dbProperty, QVariant::String, true, cf(std::bind(&CanDbHandler::createPropertyWidget, &_db)))
    };
    // clang-format on
};

#endif // CANSIGNALENCODER_P_H
