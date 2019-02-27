#include "canrawfilter_p.h"
#include <QCanBusFrame>
#include <QJsonArray>
#include <QRegularExpression>
#include <log.h>

CanRawFilterPrivate::CanRawFilterPrivate(CanRawFilter* q, CanRawFilterCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanRawFilterGuiInt>())
    , q_ptr(q)
{
    initProps();

    _ui.setRxListCbk([this](const CanRawFilterGuiInt::AcceptList_t& list) {
        _rxAcceptList = list;

        cds_debug("RX list updated:");
        for (auto& item : _rxAcceptList) {
            cds_debug("id: {}, payload: {}. accept: {}", std::get<0>(item).toStdString(),
                std::get<1>(item).toStdString(), std::get<2>(item));
        }
    });

    _ui.setTxListCbk([this](const CanRawFilterGuiInt::AcceptList_t& list) {
        _txAcceptList = list;

        cds_debug("TX list updated:");
        for (auto& item : _txAcceptList) {
            cds_debug("id: {}, payload: {}. accept: {}", std::get<0>(item).toStdString(),
                std::get<1>(item).toStdString(), std::get<2>(item));
        }
    });
}

void CanRawFilterPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanRawFilterPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanRawFilterPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    QJsonArray rxList;
    for (const auto& lineItem : _rxAcceptList) {
        QJsonObject jsonLine;
        jsonLine["id"] = std::get<0>(lineItem);
        jsonLine["payload"] = std::get<1>(lineItem);
        jsonLine["policy"] = std::get<2>(lineItem);
        rxList.push_back(jsonLine);
    }

    QJsonArray txList;
    for (const auto& lineItem : _txAcceptList) {
        QJsonObject jsonLine;
        jsonLine["id"] = std::get<0>(lineItem);
        jsonLine["payload"] = std::get<1>(lineItem);
        jsonLine["policy"] = std::get<2>(lineItem);
        txList.push_back(jsonLine);
    }

    json["rxList"] = rxList;
    json["txList"] = txList;

    return json;
}

CanRawFilterGuiInt::AcceptList_t CanRawFilterPrivate::getAcceptList(const QJsonObject& json, const QString& listName)
{
    CanRawFilterGuiInt::AcceptList_t ret;

    auto listIter = json.find(listName);
    if (listIter != json.end()) {
        if (listIter.value().type() == QJsonValue::Array) {
            auto listArray = json[listName].toArray();

            for (const auto& item : listArray) {
                if (item.type() == QJsonValue::Object) {
                    auto itemObj = item.toObject();
                    QString id = itemObj["id"].toString();
                    QString payload = itemObj["payload"].toString();
                    bool policy = itemObj["policy"].toBool();

                    ret.push_back(CanRawFilterGuiInt::AcceptListItem_t(id, payload, policy));
                } else {
                    cds_warn("{}: list item is not an object", listName.toStdString());
                }
            }
        } else {
            cds_warn("{} expected to be an array", listName.toStdString());
        }
    } else {
        cds_warn("{} not found", listName.toStdString());
    }

    return ret;
}

void CanRawFilterPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        QString propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    auto&& rxList = getAcceptList(json, "rxList");
    auto&& txList = getAcceptList(json, "txList");

    _ui.setListRx(rxList);
    _ui.setListTx(txList);
}

bool CanRawFilterPrivate::acceptFrame(const CanRawFilterGuiInt::AcceptList_t& list, const QCanBusFrame& frame)
{
    const QString id = QString::number(frame.frameId(), 16).toLower();
    const QString payload = frame.payload().toHex().toLower();

    for (auto& listItem : list) {
        QRegularExpression reId(std::get<0>(listItem).toLower());
        QRegularExpression rePayload(std::get<1>(listItem).toLower());
        bool accept = std::get<2>(listItem);

        auto matchId = reId.match(id);
        auto matchPayload = rePayload.match(payload);

        if (matchId.hasMatch() && matchPayload.hasMatch()) {
            return accept;
        }
    }

    cds_error("No match or accept list is empty. Frame dropped");
    return false;
}

bool CanRawFilterPrivate::acceptRxFrame(const QCanBusFrame& frame)
{
    return acceptFrame(_rxAcceptList, frame);
}

bool CanRawFilterPrivate::acceptTxFrame(const QCanBusFrame& frame)
{
    return acceptFrame(_txAcceptList, frame);
}
