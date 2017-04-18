#include "cansignalcoder.h"
#include "cansignalcoder_p.h"
#include <QCanBusFrame>
#include <QVariant>

CanSignalCoder::CanSignalCoder(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanSignalCoderPrivate())
{
    Q_D(CanSignalCoder);

    // Hardcode signals for now. Later they should come from DBC
    d->addMessage(0x123, 8, { { "VehicleSpeed", 0x1ff, 0 },
                                { "SteeringWheelAngle", 0x7fe00, 9 } });
}

CanSignalCoder::~CanSignalCoder()
{
}

void CanSignalCoder::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanSignalCoder);

    const auto& id = frame.frameId();

    if (d->raw2SigMap.find(id) != d->raw2SigMap.end()) {
        const auto& payload = frame.payload();

        if (d->rawValue[id].size() == payload.size()) {
            d->rawValue[id] = payload;

            for (auto& s : d->raw2SigMap[id]) {
                auto& name = std::get<CanSignalCoderPrivate::TupleId::SIGNAL_NAME>(s);
                auto& mask = std::get<CanSignalCoderPrivate::TupleId::SIGNAL_MASK>(s);
                auto& shift = std::get<CanSignalCoderPrivate::TupleId::SIGNAL_SHIFT>(s);

                quint64 val = d->ba2val(payload);

                emit sendSignal(name, QByteArray::number((val & mask) >> shift));
            }
        } else {
            //TODO: ERROR
        }
    } else {
        //TODO: No such message in DB
    }
}

void CanSignalCoder::signalReceived(const QString& name, const QByteArray& value)
{
    Q_D(CanSignalCoder);

    auto iter = d->sig2RawMap.find(name);
    if (iter != d->sig2RawMap.end()) {
        auto& id = iter->second.first;
        auto& s = iter->second.second;
        auto& mask = std::get<CanSignalCoderPrivate::TupleId::SIGNAL_MASK>(s);
        auto& shift = std::get<CanSignalCoderPrivate::TupleId::SIGNAL_SHIFT>(s);

        auto canVal = d->ba2val(d->rawValue[id]);
        auto sigVal = value.toUInt();

        canVal &= ~mask;
        canVal |= (sigVal << shift) & mask;
        d->val2ba(canVal, d->rawValue[id]);

        QCanBusFrame frame;
        frame.setPayload(d->rawValue[id]);
        frame.setFrameId(id);

        QVariant ctx = 0;
        emit sendFrame(frame, ctx);
    } else {
        //TODO: ERROR
    }
}
