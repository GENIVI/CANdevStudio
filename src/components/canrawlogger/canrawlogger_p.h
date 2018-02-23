#ifndef CANRAWLOGGER_P_H
#define CANRAWLOGGER_P_H

#include "canrawlogger.h"
#include <QtCore/QObject>
#include <memory>
#include <QFile>
#include <QElapsedTimer>

class CanRawLogger;

class CanRawLoggerPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawLogger)

public:
    CanRawLoggerPrivate(CanRawLogger* q, CanRawLoggerCtx&& ctx = CanRawLoggerCtx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void logFrame(const QCanBusFrame& frame, const QString& dir);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanRawLoggerCtx _ctx;
    std::map<QString, QVariant> _props;
    QString _filename;
    QFile _file;
    QElapsedTimer _timer;

private:
    CanRawLogger* q_ptr;
    const QString _nameProperty = "name";
    const QString _dirProperty = "directory";
    ComponentInterface::ComponentProperties _supportedProps = {
            {_dirProperty,   {QVariant::String, true}},
            {_nameProperty,   {QVariant::String, true}}
    };
};

#endif // CANRAWLOGGER_P_H
