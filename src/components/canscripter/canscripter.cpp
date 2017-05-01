#include "canscripter.h"
#include "canscripter_p.h"
#include <QCanBusFrame>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

CanScripter::CanScripter(QObject* parent)
    : QObject(parent)
    , d_ptr(new CanScripterPrivate(this))
{
}

CanScripter::~CanScripter()
{
}

void CanScripter::start()
{
    Q_D(CanScripter);

    QFile file(d->scriptName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file '" << file.fileName() << "'";
        return;
    }

    QString script = file.readAll();
    QJsonDocument jf = QJsonDocument::fromJson(script.toUtf8());
    file.close();

    if (jf.isObject()) {
        if (d->parseScriptObject(jf.object())) {
            d->start();
        } else {
            qDebug() << "ERROR: Failed to start script";
            return;
        }
    } else {
        qDebug() << "ERROR: Root object shall be JsonObject";
    }

}

void CanScripter::stop()
{
    Q_D(CanScripter);

    d->stop();
}

void CanScripter::setScriptFilename(const QString& scriptFile)
{
    Q_D(CanScripter);

    d->scriptName = scriptFile;
}

void CanScripter::setScriptRepeat(bool repeat)
{
    Q_D(CanScripter);

    d->repeat = repeat;
}

