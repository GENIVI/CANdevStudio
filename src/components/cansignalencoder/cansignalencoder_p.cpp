#include "cansignalencoder_p.h"
#include <QCanBusFrame>
#include <log.h>

CanSignalEncoderPrivate::CanSignalEncoderPrivate(CanSignalEncoder* q, CanSignalEncoderCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, q_ptr, &CanSignalEncoder::simBcastSnd);
    connect(&_db, &CanDbHandler::requestRedraw, q_ptr, &CanSignalEncoder::requestRedraw);
}

void CanSignalEncoderPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalEncoderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalEncoderPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalEncoderPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _db.updateCurrentDbFromProps();
}

void CanSignalEncoderPrivate::encodeSignal(const QString& name, const QVariant& val)
{
    auto nameSplit = name.split('_');

    if (nameSplit.size() < 2) {
        cds_error("Wrong signal name: {}", name.toStdString());
        return;
    }

    uint32_t id = nameSplit[0].toUInt(nullptr, 16);

    const CANmessages_t::value_type* msgDesc = nullptr;
    if (_db.getDb().count(id) > 0) {
        auto iter = _db.getDb().find(id);

        if (iter != _db.getDb().end()) {
            msgDesc = &(*iter);
        }
    }

    QString sigName = name.mid(name.indexOf("_") + 1);

    if (!msgDesc) {
        cds_warn("Msg '{:03x}' not found in DB", id);
        return;
    }

    for (auto& sig : msgDesc->second) {
        if (sig.signal_name == sigName.toStdString()) {
            if (_rawCache[id].size() < (int)msgDesc->first.dlc) {
                cds_info("Setting up new cache for {:03x} msg", id);
                // Set chache for the first time
                _rawCache[id].fill(0, msgDesc->first.dlc);
            }

            if (_rawCache[id].size() * 8 >= sig.startBit + sig.signalSize) {

                signalToRaw(id, sig, val, msgDesc->first.updateCycle);
                return;
            } else {
                cds_error("Payload size ('{}') for signal '{}' is too small. StartBit {}, signalSize {}",
                    _rawCache[id].size() * 8, sig.signal_name, sig.startBit, sig.signalSize);
            }
        }
    }

    cds_warn("Failed to find '{}' signal in '{:03x}' msg", sigName.toStdString(), id);

    return;
}

void CanSignalEncoderPrivate::signalToRaw(
    const uint32_t id, const CANsignal& sigDesc, const QVariant& sigVal, const uint32_t updateCycle)
{
    if (sigDesc.factor == 0) {
        cds_error("Factor for {} signal is 0! Singal encoding failed.", sigDesc.signal_name);
        return;
    }

    int64_t rawVal = static_cast<int64_t>((sigVal.toDouble() - sigDesc.offset) / sigDesc.factor);
    uint8_t* data = (uint8_t*)_rawCache[id].data();

    if (sigDesc.byteOrder == 0) {
        // little endian
        auto bit = sigDesc.startBit;
        for (int bitpos = 0; bitpos < sigDesc.signalSize; bitpos++) {
            // clear bit first
            data[bit / 8] &= ~(1U << (bit % 8));
            // set bit
            data[bit / 8] |= ((rawVal >> bitpos) & 1U) << (bit % 8);
            bit++;
        }
    } else {
        // motorola / big endian mode
        auto bit = sigDesc.startBit;
        for (int bitpos = 0; bitpos < sigDesc.signalSize; bitpos++) {
            // clear bit first
            data[bit / 8] &= ~(1U << (bit % 8));
            // set bit
            data[bit / 8] |= ((rawVal >> (sigDesc.signalSize - bitpos - 1)) & 1U) << (bit % 8);

            bit++;
        }
    }

    if (updateCycle == 0) {
        // Send raw frame if cycle for message is not defined
        QCanBusFrame frame(id, _rawCache[id]);

        emit q_ptr->sndFrame(frame);
    }
}

void CanSignalEncoderPrivate::initCacheAndTimers()
{
    _cycleTimers.clear();
    _rawCache.clear();

    for (const auto& msg : _db.getDb()) {
        if (msg.first.initValue.length()) {
            _rawCache[msg.first.id] = QByteArray::fromHex(msg.first.initValue.c_str());
        }

        if (msg.first.updateCycle) {
            QTimer* timer = new QTimer;
            timer->setInterval(msg.first.updateCycle);
            connect(timer, &QTimer::timeout, [this, msg] {
                QCanBusFrame frame;
                frame.setFrameId(msg.first.id);
                if (_rawCache[msg.first.id].size() < (int)msg.first.dlc) {
                    cds_info("Setting up new cache for {:03x} msg", msg.first.id);
                    // Set chache for the first time
                    _rawCache[msg.first.id].fill(0, msg.first.dlc);
                }

                frame.setPayload(_rawCache[msg.first.id]);
                emit q_ptr->sndFrame(frame);
            });

            _cycleTimers.push_back(std::make_unique<QTimer>());
            _cycleTimers.back().reset(timer);
        }

        if (msg.first.updateCycle || msg.first.initValue.length()) {
            cds_debug("Settings for msg '{:03x}', cycle: {}, initValue: {}", msg.first.id, msg.first.updateCycle,
                msg.first.initValue);
        }
    }

}
