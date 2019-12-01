#ifndef CANSIGNALDECODER_P_H
#define CANSIGNALDECODER_P_H

#include "cansignaldecoder.h"
#include <candbhandler.h>
#include <memory>
#include <propertyfields.h>

class CanSignalDecoder;

class CanSignalDecoderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalDecoder)

public:
    CanSignalDecoderPrivate(CanSignalDecoder* q, CanSignalDecoderCtx&& ctx = CanSignalDecoderCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void decodeFrame(const QCanBusFrame& frame, Direction const direction, bool status);

private:
    void initProps();
    int64_t rawToSignal(const uint8_t* data, int startBit, int sigSize, bool littleEndian, bool isSigned);

        public : bool _simStarted{ false };
    CanSignalDecoderCtx _ctx;
    std::map<QString, QVariant> _props;
    CanDbHandler _db{ _props, _dbProperty };

private:
    CanSignalDecoder* q_ptr;
    const QString _nameProperty = "name";
    const QString _dbProperty = "CAN database";
    std::map<QString, QVariant> _signalCache;

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_dbProperty, QVariant::String, true, cf(std::bind(&CanDbHandler::createPropertyWidget, &_db)))
    };
    // clang-format on
};

#endif // CANSIGNALDECODER_P_H
