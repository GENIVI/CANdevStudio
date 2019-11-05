#ifndef CANSIGNALENCODER_P_H
#define CANSIGNALENCODER_P_H

#include "cansignalencoder.h"
#include <memory>

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

private:
    CanSignalEncoder* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};

#endif // CANSIGNALENCODER_P_H
