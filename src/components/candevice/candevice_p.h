#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candevice.h"
#include "candeviceqt.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QtCore/QVector>

#include <QJsonDocument>

class CanDevicePrivate {
    Q_DECLARE_PUBLIC(CanDevice)

public:
    CanDevicePrivate(CanDevice *q, CanDeviceCtx&& ctx = CanDeviceCtx(new CanDeviceQt()))
        : _ctx(std::move(ctx))
        , _canDevice(_ctx.get<CanDeviceInterface>())
        , q_ptr(q)
    {
        initProps();
        _canDevice.setParent(q);
    }

    void saveSettings(QJsonObject& json)
    {
        QJsonArray viewModelsArray;

        for (const auto& p: _props)
        {
            json[p.first] = QJsonValue::fromVariant(p.second);
        }
    }

    bool restoreConfiguration(const QJsonObject& json)
    {
        for (const auto& p: _supportedProps)
        {
            if (json.contains(p.first))
                _props[p.first] = json[p.first].toVariant();
        }
        return true;
    }

    CanDeviceCtx _ctx;
    QVector<QCanBusFrame> _sendQueue;
    CanDeviceInterface& _canDevice;
    bool _initialized{ false };
    bool _simStarted{ false };

    const QString _nameProperty = "name";
    const QString _backendProperty = "backend";
    const QString _interfaceProperty = "interface";

    ComponentInterface::ComponentProperties _supportedProps = {
            {_nameProperty,   {QVariant::String, true}},
            {_backendProperty,   {QVariant::String, true}},
            {_interfaceProperty, {QVariant::String, true}}
    };

    std::map<QString, QVariant> _props;

private:
    void initProps()
    {
        for (const auto& p: _supportedProps)
        {
            _props[p.first];
        }
    }

    CanDevice *q_ptr;

};

#endif /* !__CANDEVICE_P_H */
