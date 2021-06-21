#include "cansignalencoder_p.h"
#include <QCanBusFrame>
#include <log.h>

// Python script used to generate below:
//
// for rows in range (1,9):
//    bit = rows*8 - 1
//    for cols in range(0,8):
//        val = int(bit - cols)
//        print('%3d,' % val, end='')
//        if val % 8 == 0:
//            print('')
//
// clang-format off
namespace {
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
}
// clang-format on

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

            // Calculate how many bits are used already before this signal. Calculations are different for
            // little and big endian. Good overview on how big endian signals are aligned can be found
            // here: https://github.com/eerimoq/cantools#the-dump-subcommand
            uint8_t bitsBefore = 0;
            bool littleEndian = sig.byteOrder == 1;
            uint8_t msgSize = _rawCache[id].size();

            if (littleEndian) {
                bitsBefore = sig.startBit;
            } else {
                bitsBefore = beTransTable[sig.startBit];
            }

            if (bitsBefore + sig.signalSize > (msgSize * 8)) {
                cds_error("Invalid signal or message size - startBit {}, sigSize {}, bitsBefore {}, payload size {}, "
                          "littleEndian: {}",
                    sig.startBit, sig.signalSize, bitsBefore, msgSize, littleEndian);
                continue;
            }

            signalToRaw(id, sig, val, msgDesc->first.updateCycle);
            return;
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

    int64_t rawVal = static_cast<int64_t>(std::llround((sigVal.toDouble() - sigDesc.offset) / sigDesc.factor));
    uint8_t* data = (uint8_t*)_rawCache[id].data();

    if (sigDesc.byteOrder == 0) {

        // Motorola / Big endian signal example with start bit 2 and length 5 (0=LSB, 4=MSB):
        // Byte:       0        1        2        3
        //        +--------+--------+--------+--- - -
        //        |    |432|10|     |        |
        //        +--------+--------+--------+--- - -
        // Bit:    7      0 15     8 23    16 31
        //
        // Source: https://github.com/eerimoq/cantools/blob/master/cantools/database/can/signal.py

        uint8_t bitpos = 0;
        for (int i = sigDesc.signalSize - 1; i >= 0; --i) {
            // First beTransTable returns number of bytes used before startBit
            // Then we are adding length of the signal and getting 'offset' where the LSB is
            // Second 'translation' of 'offset' with beTransTable gives us actual bit position
            int bit = beTransTable[beTransTable[sigDesc.startBit] + i];

            // clear bit first
            data[bit / 8] &= ~(1U << (bit % 8));
            // set bit
            data[bit / 8] |= ((rawVal >> bitpos) & 1U) << (bit % 8);

            ++bitpos;
        }
    } else {

        // Little endian signal example with start bit 2 and length 9 (0=LSB, 8=MSB):
        // Byte:       0        1        2        3
        //       +--------+--------+--------+--- - -
        //       |543210| |    |876|        |
        //       +--------+--------+--------+--- - -
        // Bit:   7      0 15     8 23    16 31
        //
        // Source: https://github.com/eerimoq/cantools/blob/master/cantools/database/can/signal.py

        auto bit = sigDesc.startBit;
        for (int bitpos = 0; bitpos < sigDesc.signalSize; bitpos++) {
            // clear bit first
            data[bit / 8] &= ~(1U << (bit % 8));
            // set bit
            data[bit / 8] |= ((rawVal >> bitpos) & 1U) << (bit % 8);
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
