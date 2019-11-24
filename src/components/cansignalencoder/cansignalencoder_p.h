#ifndef CANSIGNALENCODER_P_H
#define CANSIGNALENCODER_P_H

#include "cansignalencoder.h"
#include <QUuid>
#include <candbhandler.h>
#include <memory>
#include <propertyfields.h>
#include <QTimer>

class CanSignalEncoder;

class CanSignalEncoderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalEncoder)

public:
    CanSignalEncoderPrivate(CanSignalEncoder* q, CanSignalEncoderCtx&& ctx = CanSignalEncoderCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void encodeSignal(const QString& name, const QVariant& val);

private:
    void initProps();
    void signalToRaw(const uint32_t id, const CANsignal& sigDesc, const QVariant& sigVal, const uint32_t updateCycle);

public:
    bool _simStarted{ false };
    CanSignalEncoderCtx _ctx;
    std::map<QString, QVariant> _props;
    CanDbHandler _db{ _props, _dbProperty };
    std::vector<std::unique_ptr<QTimer>> _cycleTimers;

public slots:
    void initCacheAndTimers();

private:
    CanSignalEncoder* q_ptr;
    const QString _nameProperty = "name";
    const QString _dbProperty = "CAN database";
    std::map<uint32_t, QByteArray> _rawCache;

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
