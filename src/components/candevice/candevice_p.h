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
    CanDevicePrivate(CanDevice* q, CanDeviceCtx&& ctx = CanDeviceCtx(new CanDeviceQt()))
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

        for (const auto& p : _props) {
            json[p.first] = QJsonValue::fromVariant(p.second);
        }
    }

    bool restoreConfiguration(const QJsonObject& json)
    {
        for (const auto& p : _supportedProps) {
            QString propName = std::get<0>(p);
            if (json.contains(propName))
                _props[propName] = json[propName].toVariant();
        }
        return true;
    }

    using devConfigPair = std::pair<int, QVariant>;
    std::vector<devConfigPair> getDevConfig()
    {
        QString c = _props.at(_configProperty).toString().simplified().replace(" ", "");
        std::vector<devConfigPair> ret;

        auto&& propList = c.split(";");

        for (auto& item : propList) {
            devConfigPair pair;
            bool res = getConfigPair(item, pair);

            if (res) {
                ret.push_back(pair);
            }
        }

        return ret;
    }

    CanDeviceCtx _ctx;
    QVector<QCanBusFrame> _sendQueue;
    CanDeviceInterface& _canDevice;
    bool _initialized{ false };
    bool _simStarted{ false };

    const QString _nameProperty = "name";
    const QString _backendProperty = "backend";
    const QString _interfaceProperty = "interface";
    const QString _configProperty = "configuration";

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty,  QVariant::String, true),
            std::make_tuple(_backendProperty, QVariant::String, true),
            std::make_tuple(_interfaceProperty, QVariant::String, true),
            std::make_tuple(_configProperty, QVariant::String, true)
    };
    // clang-format on

    std::map<QString, QVariant> _props;

private:
    void initProps()
    {
        for (const auto& p : _supportedProps) {
            _props[std::get<0>(p)];
        }
    }

    bool getConfigPair(const QString& in, devConfigPair& out)
    {
        auto configStr = in.split("=");

        if ((configStr.length() != 2) || ((configStr.length() == 2) && (configStr[1].length() == 0))) {
            cds_error("Config parameter parse error");
            return false;
        }

        auto keyStr = configStr[0];
        auto valStr = configStr[1];

        QCanBusDevice::ConfigurationKey key;
        QVariant val;
        bool res = true;

        if (keyStr == "RawFilterKey") {
            cds_error("RawFilterKey not supported");
            res = false;
        } else if (keyStr == "ErrorFilterKey") {
            cds_error("ErrorFilterKey not supported");
            res = false;
        } else if (keyStr == "LoopbackKey") {
            key = QCanBusDevice::LoopbackKey;
            val = valStr.toUpper() == "TRUE";
        } else if (keyStr == "ReceiveOwnKey") {
            key = QCanBusDevice::ReceiveOwnKey;
            val = valStr.toUpper() == "TRUE";
        } else if (keyStr == "BitRateKey") {
            key = QCanBusDevice::BitRateKey;
            val = valStr.toUInt(&res);
        } else if (keyStr == "CanFdKey") {
            key = QCanBusDevice::CanFdKey;
            val = valStr.toUpper() == "TRUE";
#if QT_VERSION >= 0x050900
        } else if (keyStr == "DataBitRateKey") {
            key = QCanBusDevice::DataBitRateKey;
            val = valStr.toUInt(&res);
#endif
        } else if (keyStr == "UserKey") {
            key = QCanBusDevice::UserKey;
            val = valStr;
        } else {
            cds_error("Failed to convert '{}' to ConfigurationKey", keyStr.toStdString());
            res = false;
        }

        if (res) {
            cds_debug("Key: {}({}), Val: {}", keyStr.toStdString(), key, val.toString().toStdString());
            out = std::make_pair(key, val);
        }

        return res;
    }

    CanDevice* q_ptr;
};

#endif /* !__CANDEVICE_P_H */
