#ifndef CANRAWPLAYER_P_H
#define CANRAWPLAYER_P_H

#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>
#include <QtCore/QTimer>
#include <memory>
#include "canrawplayer.h"
#include <propertyfields.h>

class CanRawPlayer;

class CanRawPlayerPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawPlayer)

public:
    CanRawPlayerPrivate(CanRawPlayer* q, CanRawPlayerCtx&& ctx = CanRawPlayerCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void loadTraceFile(const QString& filename);
    void startPlayback();
    void stopPlayback();

private:
    void initProps();


private slots:
    void timeout();

public:
    bool _simStarted{ false };
    CanRawPlayerCtx _ctx;
    std::map<QString, QVariant> _props;
    uint32_t _tick{ 10 };
    const QString _nameProperty = "name";
    const QString _fileProperty = "file";
    const QString _tickProperty = "timer tick [ms]";

private:
    CanRawPlayer* q_ptr;
    std::vector<std::pair<unsigned int, QCanBusFrame>> _frames;
    uint32_t _frameNdx;
    uint32_t _ticks;
    QTimer _timer;

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_fileProperty, QVariant::String, true, cf([] { return new PropertyFieldPath; } )),
            std::make_tuple(_tickProperty, QVariant::String, true, cf([] { return new PropertyFieldText(true); } ))
    };
    // clang-format on
};

#endif // CANRAWPLAYER_P_H
