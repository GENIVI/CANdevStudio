#ifndef CANSIGNALDECODER_P_H
#define CANSIGNALDECODER_P_H

#include "cansignaldecoder.h"
#include <memory>

class CanSignalDecoder;

class CanSignalDecoderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalDecoder)

public:
    CanSignalDecoderPrivate(CanSignalDecoder* q, CanSignalDecoderCtx&& ctx = CanSignalDecoderCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanSignalDecoderCtx _ctx;
    std::map<QString, QVariant> _props;

private:
    CanSignalDecoder* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};

#endif // CANSIGNALDECODER_P_H
