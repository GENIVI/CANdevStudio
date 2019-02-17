#include "canrawplayer_p.h"
#include <QCanBusFrame>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>
#include <log.h>

CanRawPlayerPrivate::CanRawPlayerPrivate(CanRawPlayer* q, CanRawPlayerCtx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
    , _timer(this)
{
    initProps();

    setParent(q);

    _timer.setTimerType(Qt::PreciseTimer);
    connect(&_timer, &QTimer::timeout, this, &CanRawPlayerPrivate::timeout);
}

void CanRawPlayerPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
    
    _props[_tickProperty] = _tick;
}

ComponentInterface::ComponentProperties CanRawPlayerPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanRawPlayerPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanRawPlayerPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        QString propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}

void CanRawPlayerPrivate::loadTraceFile(const QString& filename)
{
    QFileInfo fileNfo(filename);
    QFile traceFile(filename);

    _frames.clear();

    if (!fileNfo.exists() || !fileNfo.isFile()) {
        cds_error("File: '{}' does not exist", filename.toStdString());
        return;
    }

    if (!traceFile.open(QIODevice::ReadOnly)) {
        cds_error("Failed to open file '{}' for reading", filename.toStdString());
        return;
    }

    QTextStream in(&traceFile);

    QRegularExpression re(R"(\((\d+\.\d{6})\)\s*\w*\s*([0-9,A-F]{1,8})\s*\[(\d)\]\s*((\s*[0-9,A-F]{2}){0,8}))");
    while (!in.atEnd()) {
        QString line = in.readLine();

        const auto& match = re.match(line);

        if (match.hasMatch()) {
            unsigned int time = static_cast<unsigned int>(std::stof(match.captured(1).toStdString()) * 1000 + 0.5);
            auto id = std::stoul(match.captured(2).toStdString(), 0, 16);
            auto payload = QByteArray::fromHex(match.captured(4).replace(" ", "").toLatin1());

            QCanBusFrame frame(id, payload);
            _frames.push_back({ time, std::move(frame) });
        }
    }

    cds_info("Number of frames to play: {}", _frames.size());

    traceFile.close();
}

void CanRawPlayerPrivate::stopPlayback()
{
    _timer.stop();
}

void CanRawPlayerPrivate::startPlayback()
{
    _frameNdx = 0;
    _ticks = 0;
    _timer.setInterval(_tick);
    _timer.start();
}

void CanRawPlayerPrivate::timeout()
{
    _ticks += _tick;

    while (_frameNdx < _frames.size() && _frames[_frameNdx].first <= _ticks) {
        emit q_ptr->sendFrame(_frames[_frameNdx].second);
        ++_frameNdx;
    }
}
