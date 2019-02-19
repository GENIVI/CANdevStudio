#ifndef CANLOAD_P_H
#define CANLOAD_P_H

#include "canload.h"
#include <QObject>
#include <QTimer>
#include <memory>
#include <propertyfields.h>

class CanLoad;

class CanLoadPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanLoad)

public:
    CanLoadPrivate(CanLoad* q, CanLoadCtx&& ctx = CanLoadCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanLoadCtx _ctx;
    std::map<QString, QVariant> _props;
    const QString _nameProperty = "name";
    const QString _periodProperty = "period [ms]";
    const QString _bitrateProperty = "bitrate [bps]";
    uint32_t _period = 0;
    uint32_t _bitrate = 0;
    uint32_t _div = 0;
    uint64_t _totalBits = 0;
    QTimer _timer;

private:
    CanLoad* q_ptr;

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_bitrateProperty, QVariant::String, true, cf([] { return new PropertyFieldText(true); } )),
            std::make_tuple(_periodProperty, QVariant::String, true, cf([] { return new PropertyFieldText(true); } ))
    };
    // clang-format on
};

#endif // CANLOAD_P_H
