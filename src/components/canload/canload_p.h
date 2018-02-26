#ifndef CANLOAD_P_H
#define CANLOAD_P_H

#include "canload.h"
#include <QtCore/QObject>
#include <memory>
#include <QTimer>

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
    uint64_t _totalBits = 0;
    QTimer _timer;

private:
    CanLoad* q_ptr;
    ComponentInterface::ComponentProperties _supportedProps = {
            {_bitrateProperty,   {QVariant::String, true}},
            {_periodProperty,   {QVariant::String, true}},
            {_nameProperty,   {QVariant::String, true}}
    };
};

#endif // CANLOAD_P_H
