#include "canrawfilter_p.h"
#include <log.h>

CanRawFilterPrivate::CanRawFilterPrivate(CanRawFilter* q, CanRawFilterCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanRawFilterGuiInt>())
    , q_ptr(q)
{
    initProps();

    _ui.setRxListCbk([this](const CanRawFilterGuiInt::AcceptList_t& list) {
        _rxAcceptList = list;

        for (auto& item : _rxAcceptList) {
            cds_debug("id: {}, payload: {}. accept: {}", std::get<0>(item).toStdString(),
                std::get<1>(item).toStdString(), std::get<2>(item));
        }
    });
    _ui.setTxListCbk([this](const CanRawFilterGuiInt::AcceptList_t& list) { _txAcceptList = list; });
}

void CanRawFilterPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[p.first];
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

    return json;
}

void CanRawFilterPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        if (json.contains(p.first))
            _props[p.first] = json[p.first].toVariant();
    }
}

bool CanRawFilterPrivate::acceptRxFrame(const QCanBusFrame& frame)
{
    // TODO
    return true;
}

bool CanRawFilterPrivate::acceptTxFrame(const QCanBusFrame& frame)
{
    // TODO
    return true;
}
