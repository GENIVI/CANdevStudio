#ifndef CANSIGNALCODERPRIVATE_P_H
#define CANSIGNALCODERPRIVATE_P_H

#include <QString>
#include <map>
#include <tuple>
#include <vector>

class CanSignalCoderPrivate {
public:
    typedef std::tuple<QString, quint64, quint64> SignalDesc;

    void addMessage(quint32 id, quint8 dlc, const std::vector<SignalDesc> sigVec)
    {
        for (auto& i : sigVec) {
            raw2SigMap[id].push_back(i);
            auto& name = std::get<SIGNAL_NAME>(i);
            sig2RawMap.insert({ name, { id, i } });
            rawValue[id].fill(0, dlc);
        }
    }

    quint64 ba2val(const QByteArray& ba)
    {
        quint64 ret = 0;

        for (int i = 0; i < ba.size(); ++i) {
            ret |= ba.data()[i] << (8 * i);
        }

        return ret;
    }

    void val2ba(const quint64 number, QByteArray& ba)
    {
        for (int i = 0; i < ba.size(); ++i) {
            ba[i] = (number >> 8 * i) & 0xff;
        }
    }

    enum TupleId {
        SIGNAL_NAME = 0,
        SIGNAL_MASK,
        SIGNAL_SHIFT
    };

    std::map<quint32, std::vector<SignalDesc> > raw2SigMap;
    std::map<QString, std::pair<quint32, SignalDesc> > sig2RawMap;
    std::map<quint32, QByteArray> rawValue;
};

#endif // CANSIGNALCODERPRIVATE_P_H
