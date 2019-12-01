#include "cansignaldecoder_p.h"
#include <QCanBusFrame>
#include <log.h>

CanSignalDecoderPrivate::CanSignalDecoderPrivate(CanSignalDecoder* q, CanSignalDecoderCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, q_ptr, &CanSignalDecoder::simBcastSnd);
    connect(&_db, &CanDbHandler::requestRedraw, q_ptr, &CanSignalDecoder::requestRedraw);
}

void CanSignalDecoderPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalDecoderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalDecoderPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalDecoderPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _db.updateCurrentDbFromProps();
}

void CanSignalDecoderPrivate::decodeFrame(const QCanBusFrame& frame, Direction const direction, bool)
{
    bool littleEndian;

    const CANmessages_t::value_type* msgDesc = nullptr;
    if (_db.getDb().count(frame.frameId()) > 0) {
        auto iter = _db.getDb().find(frame.frameId());

        if (iter != _db.getDb().end()) {
            msgDesc = &(*iter);
        }
    }

    if (msgDesc) {
        for (auto& sig : msgDesc->second) {
            if ((sig.startBit >= (frame.payload().size() * 8))
                || ((sig.startBit + sig.signalSize - 1) >= (frame.payload().size() * 8))) {

                cds_error("Invalid message size - startBit {}, sigSize {}, payload size {}", sig.startBit,
                    sig.signalSize, frame.payload().size());

                continue;
            }

            switch (sig.byteOrder) {
            case 0:
                // Little endian
                littleEndian = true;
                break;

            case 1:
                // Big endian
                littleEndian = false;
                break;

            default:
                cds_error("byte order {} not suppoerted", sig.byteOrder);
                continue;
            }

            int64_t value = rawToSignal((const uint8_t*)frame.payload().constData(), sig.startBit, sig.signalSize,
                littleEndian, sig.valueSigned);

            QVariant sigVal;


            if ((std::fmod(sig.factor, 1) == 0.0) && (std::fmod(sig.offset, 1) == 0.0)) {
                // resulting number will be integer
                value = value * sig.factor + sig.offset;
                sigVal.setValue(value);
            } else {
                double fValue = static_cast<double>(value) * sig.factor + sig.offset;
                sigVal.setValue(fValue);
            }

            QString sigName = fmt::format("0x{:03x}_{}", frame.frameId(), sig.signal_name).c_str();

            if ((_signalCache.find(sigName) == _signalCache.end())
                || ((_signalCache.find(sigName) != _signalCache.end()) && (_signalCache[sigName] != sigVal))) {

                emit q_ptr->sndSignal(sigName, sigVal, direction);

                cds_debug("Signal: {}, val: {}", sigName.toStdString(), sigVal.toDouble());
            }

            _signalCache[sigName] = sigVal;
        }
    } else {
        cds_debug("No signal description for frame {:x}", frame.frameId());
    }
}

int64_t CanSignalDecoderPrivate::rawToSignal(
    const uint8_t* data, int startBit, int sigSize, bool littleEndian, bool isSigned)
{
    int64_t result = 0;

    int bit = startBit;
    for (int bitpos = 0; bitpos < sigSize; bitpos++) {
        if (data[bit / 8] & (1 << (bit % 8))) {
            if (littleEndian) {
                result |= (1ULL << bitpos);
            } else {
                result |= (1ULL << (sigSize - bitpos - 1));
            }
        }

        bit++;
    }

    // if signal is signed and sign bit (MSB) is set make sure
    // that this is reflected in int64_t
    if (isSigned && (result & (1ULL << (sigSize - 1)))) {
        result |= ~((1ULL << sigSize) - 1);
    }

    return result;
}
