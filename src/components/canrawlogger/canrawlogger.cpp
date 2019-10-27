#include "canrawlogger.h"
#include "canrawlogger_p.h"
#include <QDateTime>
#include <QDir>
#include <confighelpers.h>
#include <log.h>

CanRawLogger::CanRawLogger()
    : d_ptr(new CanRawLoggerPrivate(this))
{
}

CanRawLogger::CanRawLogger(CanRawLoggerCtx&& ctx)
    : d_ptr(new CanRawLoggerPrivate(this, std::move(ctx)))
{
}

CanRawLogger::~CanRawLogger() {}

QWidget* CanRawLogger::mainWidget()
{
    // Component does not have main widget
    return nullptr;
}

void CanRawLogger::setConfig(const QJsonObject& json)
{
    Q_D(CanRawLogger);

    d->setSettings(json);
}

void CanRawLogger::setConfig(const QWidget& qobject)
{
    Q_D(CanRawLogger);

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}

QJsonObject CanRawLogger::getConfig() const
{
    return d_ptr->getSettings();
}

std::shared_ptr<QWidget> CanRawLogger::getQConfig() const
{
    const Q_D(CanRawLogger);

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}

void CanRawLogger::configChanged() {}

bool CanRawLogger::mainWidgetDocked() const
{
    // Widget does not exist. Return always true
    return true;
}

ComponentInterface::ComponentProperties CanRawLogger::getSupportedProperties() const
{
    return d_ptr->getSupportedProperties();
}

void CanRawLogger::stopSimulation()
{
    Q_D(CanRawLogger);

    d->_simStarted = false;

    d->_file.close();
    d->_file.setFileName("");
}

void CanRawLogger::startSimulation()
{
    Q_D(CanRawLogger);
    QDateTime date = QDateTime::currentDateTime();
    QString dirName = d->_props[d->_dirProperty].toString();
    QDir dir;

    if (!dir.exists(dirName)) {
        cds_info("Dir {} does not exist", dirName.toStdString());

        if (dir.mkdir(dirName)) {
            cds_info("Directory '{}' created", dirName.toStdString());
        } else {
            cds_error("Failed to create '{}' directory", dirName.toStdString());
        }
    }

    QString name = d->_props[d->_nameProperty].toString();
    d->_filename = dirName + "/" + name.replace(" ", "_") + "_" + QString::number(date.date().year())
        + QString("%1").arg(date.date().month(), 2, 10, QChar('0'))
        + QString("%1").arg(date.date().day(), 2, 10, QChar('0')) + "_"
        + QString("%1").arg(date.time().hour(), 2, 10, QChar('0'))
        + QString("%1").arg(date.time().minute(), 2, 10, QChar('0'))
        + QString("%1").arg(date.time().second(), 2, 10, QChar('0')) + ".log";

    cds_debug("Log filename '{}'", d->_filename.toStdString());

    int num = 1;
    QString tmpName = d->_filename;
    while (QFile::exists(tmpName)) {
        cds_warn("Log file '{}' already exists!", tmpName.toStdString());
        tmpName = d->_filename.left(d->_filename.lastIndexOf(".")) + "(" + QString::number(num) + ").log";
        ++num;
    }

    if (tmpName != d->_filename) {
        d->_filename = tmpName;
        cds_warn("New name for log file '{}'", tmpName.toStdString());
    }

    d->_file.setFileName(d->_filename);
    d->_file.open(QIODevice::WriteOnly);

    d->_simStarted = true;
    d->_timer.start();
}

void CanRawLogger::frameReceived(const QCanBusFrame& frame)
{
    if (d_ptr->_simStarted) {
        d_ptr->logFrame(frame, "RX");
    }
}

void CanRawLogger::frameSent(bool status, const QCanBusFrame& frame)
{
    if (status && d_ptr->_simStarted) {
        d_ptr->logFrame(frame, "TX");
    }
}

void CanRawLogger::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}
