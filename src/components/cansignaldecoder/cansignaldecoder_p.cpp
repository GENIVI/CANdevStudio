#include "cansignaldecoder_p.h"
#include <QCanBusFrame>
#include <array>
#include <log.h>

// for len in range(1,65):
//    print('%3d, ' %  ((int((len - 1)/ 8) * 2 + 1)*8 - len), end='')
//    if len % 8 == 0:
//        print('')
// clang-format off
const std::array beTransTable = {
      7,  6,  5,  4,  3,  2,  1,  0,
     15, 14, 13, 12, 11, 10,  9,  8,
     23, 22, 21, 20, 19, 18, 17, 16,
     31, 30, 29, 28, 27, 26, 25, 24,
     39, 38, 37, 36, 35, 34, 33, 32,
     47, 46, 45, 44, 43, 42, 41, 40,
     55, 54, 53, 52, 51, 50, 49, 48,
     63, 62, 61, 60, 59, 58, 57, 56 
};
// clang-format on

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

            switch (sig.byteOrder) {
            case 0:
                // Big endian
                littleEndian = false;
                break;
            case 1:
                // Little endian
                littleEndian = true;
                break;

            default:
                cds_error("byte order {} not suppoerted", sig.byteOrder);
                continue;
            }

            // Calculate how many bits is used already before this signal. Calculations are different for
            // little and big endian. Good overview on how big endian signals are alinged can be found
            // here: https://github.com/eerimoq/cantools#the-dump-subcommand
            uint8_t bitsBefore = 0;

            if (littleEndian) {
                bitsBefore = sig.startBit;
            } else {
                bitsBefore = beTransTable[sig.startBit];
            }

            if (bitsBefore + sig.signalSize > (frame.payload().size() * 8)) {
                cds_error(
                    "Invalid message size - startBit {}, sigSize {}, bitsBefore {}, payload size {}, littleEndian: {}",
                    sig.startBit, sig.signalSize, bitsBefore, frame.payload().size(), littleEndian);
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

    if (littleEndian) {
        // Little endian signal example with start bit 2 and length 9 (0=LSB, 8=MSB):
        // Byte:       0        1        2        3
        //       +--------+--------+--------+--- - -
        //       |543210| |    |876|        |
        //       +--------+--------+--------+--- - -
        // Bit:   7      0 15     8 23    16 31
        //
        // Source: https://github.com/eerimoq/cantools/blob/master/cantools/database/can/signal.py

        uint8_t bit = startBit;
        for (uint8_t bitpos = 0; bitpos < sigSize; bitpos++) {

            if (data[bit / 8] & (1 << (bit % 8))) {
                result |= (1ULL << bitpos);
            }

            ++bit;
        }
    } else {

        // Big endian signal example with start bit 2 and length 5 (0=LSB, 4=MSB):
        // Byte:       0        1        2        3
        //        +--------+--------+--------+--- - -
        //        |    |432|10|     |        |
        //        +--------+--------+--------+--- - -
        // Bit:    7      0 15     8 23    16 31
        //
        // Source: https://github.com/eerimoq/cantools/blob/master/cantools/database/can/signal.py

        uint8_t bitpos = 0;
        for (int i = sigSize - 1; i >= 0; --i) {
            // Fist beTransTable returns number of bytes used before startBit
            // Then we are adding length of the signal and getting 'offset' where the LSB is
            // Second 'translation' of 'offset' with beTransTable gives us actual bit position
            int bit = beTransTable[beTransTable[startBit] + i];

            if (data[bit / 8] & (1 << (bit % 8))) {
                result |= (1ULL << bitpos);
            }

            ++bitpos;
        }
    }

    // if signal is signed and sign bit (MSB) is set make sure
    // that this is reflected in int64_t
    if (isSigned && (result & (1ULL << (sigSize - 1)))) {
        result |= ~((1ULL << sigSize) - 1);
    }

    return result;
}
