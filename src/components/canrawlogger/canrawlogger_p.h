#ifndef CANRAWLOGGER_P_H
#define CANRAWLOGGER_P_H

#include "canrawlogger.h"
#include <QElapsedTimer>
#include <QFile>
#include <QtCore/QObject>
#include <memory>
#include <propertyfields.h>

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

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty,  QVariant::String, true, cf(nullptr)),
            std::make_tuple(_dirProperty,  QVariant::String, true, cf([] { return new PropertyFieldPath(true); } ))
    };
    // clang-format on
};

#endif // CANRAWLOGGER_P_H
