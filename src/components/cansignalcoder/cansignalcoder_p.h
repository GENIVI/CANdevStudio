#ifndef CANSIGNALCODERPRIVATE_P_H
#define CANSIGNALCODERPRIVATE_P_H

#include <tuple>
#include <map>

class CanSignalCoderPrivate
{
public:
    enum TupleId {
        SIGNAL_NAME = 0,
        SIGNAL_MASK,
        SIGNAL_SHIFT,
        VALUE
    };

    typedef std::tuple<QString, quint64, quint64, quint64> SignalDesc;
    std::multimap<quint32, SignalDesc> signalMap;
    std::map<QString, quint32> sig2canid;
};

#endif // CANSIGNALCODERPRIVATE_P_H
