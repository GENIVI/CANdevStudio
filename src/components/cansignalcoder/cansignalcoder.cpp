#include "cansignalcoder.h"
#include "cansignalcoder_p.h"
#include <QCanBusFrame>
#include <QVariant>
#include <syslog.h>

extern CanSignal geniviDemoSignals[];
extern uint32_t geniviDemoSignals_cnt;

static uint32_t bitMask[] = {
    0x00000001,
    0x00000003,
    0x00000007,
    0x0000000f,
    0x0000001f,
    0x0000003f,
    0x0000007f,
    0x000000ff,
    0x000001ff,
    0x000003ff,
    0x000007ff,
    0x00000fff,
    0x00001fff,
    0x00003fff,
    0x00007fff,
    0x0000ffff,
};

CanSignalCoder::CanSignalCoder(QObject* parent)
    : QObject(parent)
    , d_ptr(new CanSignalCoderPrivate())
{
    Q_D(CanSignalCoder);

    d->addSignalDescriptors(geniviDemoSignals, geniviDemoSignals_cnt);
}

CanSignalCoder::~CanSignalCoder()
{
}

void CanSignalCoder::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanSignalCoder);

    const auto& id = frame.frameId();

    if (d->raw2Sig.find(id) != d->raw2Sig.end()) {
        for (auto& s : d->raw2Sig[id]) {
            if(frame.payload().size() * 8 > s->end) {
                uint8_t byteNum = s->start / 8;
                uint8_t valShift = s->start % 8;
                uint32_t mask = bitMask[s->end - s->start];

                // for now uint8 will be enough
                uint8_t val = frame.payload().at(byteNum);
                val >>= valShift;
                val = val & mask;

                emit sendSignal(s->sigName, QByteArray::number(val));
            } else {
                //TODO: error
            }

        }
    } else {
        //TODO: error
    }

}

void CanSignalCoder::signalReceived(const QString& name, const QByteArray& value)
{
    Q_D(CanSignalCoder);

    if(d->sig2Raw.find(name) != d->sig2Raw.end()) {
        const CanSignal *s = d->sig2Raw[name];
        uint8_t val = value.toUShort();

        if((val >= s->min) && (val <= s->max)) {
            uint8_t byteNum = s->start / 8;
            uint8_t valShift = s->start % 8;
            uint32_t mask = bitMask[s->end - s->start];

            // for now uint8 will be enough
            uint8_t tmp = d->rawValue[s->canId].at(byteNum);
            tmp &= ~(mask << valShift);
            tmp |= val << valShift;
            d->rawValue[s->canId].replace(byteNum, 1, (const char *)&tmp, 1);

            QCanBusFrame frame;
            frame.setPayload(d->rawValue[s->canId]);
            frame.setFrameId(s->canId);

            QVariant ctx = 0;
            emit sendFrame(frame, ctx);
        } else {
            //TODO: error
        }
    } else {
        //TODO: error
    }
}
