#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include "candeviceqt.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QtCore/QVector>

#include <QJsonDocument>

class CanDevicePrivate {
public:
    CanDevicePrivate(CanDeviceCtx&& ctx = CanDeviceCtx(new CanDeviceQt))
        : _ctx(std::move(ctx))
        , _canDevice(_ctx.get<CanDeviceInterface>())
    {
        initProps();
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

    const QString _backendProperty = "backend";
    const QString _interfaceProperty = "interface";

    ComponentInterface::ComponentProperties _supportedProps = {
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

};

#endif /* !__CANDEVICE_P_H */
