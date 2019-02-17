#include "canrawlogger_p.h"
#include <QCanBusFrame>
#include <log.h>

CanRawLoggerPrivate::CanRawLoggerPrivate(CanRawLogger* q, CanRawLoggerCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{
    initProps();
}

void CanRawLoggerPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }

    _props[_dirProperty] = ".";
}

ComponentInterface::ComponentProperties CanRawLoggerPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanRawLoggerPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanRawLoggerPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString &propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}

void CanRawLoggerPrivate::logFrame(const QCanBusFrame& frame, const QString& dir)
{
    using namespace fmt::literals;

    qint64 nsec = _timer.nsecsElapsed();

    if (_file.exists()) {
        auto payHex = frame.payload().toHex().toUpper();
        // insert space between bytes, skip the end
        for (int ii = payHex.size() - 2; ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }

        std::string formatString = " ({sec:03}.{msec:06})  {iface}  {id:03X}   [{dlc}]  {data}\n";
        if (frame.hasExtendedFrameFormat()) {
            formatString = " ({sec:03}.{msec:06})  {iface}  {id:08X}   [{dlc}]  {data}\n";
        }

        std::string line = fmt::format(formatString, "sec"_a = nsec / 1000000000, "msec"_a = (nsec % 1000000000) / 1000,
            "iface"_a = dir.toStdString(), "id"_a = frame.frameId(), "dlc"_a = frame.payload().size(),
            "data"_a = payHex.toStdString());

        _file.write(line.c_str());
    } else {
        cds_warn("Frame received, but file does not exist!");
    }
}
