#ifndef CANSIGNALCODERPRIVATE_P_H
#define CANSIGNALCODERPRIVATE_P_H

#include <QString>
#include <can-signals/can_signals.h>
#include <map>
#include <tuple>
#include <vector>
#include <QTimer>
#include <memory>

class CanSignalCoderPrivate {
public:
    void addSignalDescriptors(const CanSignal* sig, uint32_t cnt)
    {
        for (uint32_t i = 0; i < cnt; ++i) {
            raw2Sig[sig[i].canId].push_back(&sig[i]);
            sig2Raw[sig[i].sigName] = &sig[i];
            rawValue[sig[i].canId].fill(0, sig[i].end / 8 + 1);
            valueUpdated[sig[i].canId] = true;
        }

    }

    void clearFrameCache()
    {
        for (auto& ba : rawValue) {
            ba.second.fill(0, ba.second.size());
        }
    }

    std::map<quint32, std::vector<const CanSignal*> > raw2Sig;
    std::map<QString, const CanSignal*> sig2Raw;
    std::map<quint32, QByteArray> rawValue;
    std::map<quint32, bool> valueUpdated;
    std::vector<std::unique_ptr<QTimer>> signalTimer;
    const uint32_t frameIntervalMs {500};
    QTimer frameTimer;
};

#endif // CANSIGNALCODERPRIVATE_P_H
