#include "canscripter.h"
#include "canscripter_p.h"
#include <QCanBusFrame>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

CanScripter::CanScripter(const QString& scriptFile, QObject* parent)
    : QObject(parent)
    , d_ptr(new CanScripterPrivate())
{
    Q_D(CanScripter);
    d->scriptName = scriptFile;
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
        if (!d->parseScriptObject(jf.object())) {
            return;
        }
    } else {
        qDebug() << "ERROR: Root object shall be JsonObject";
    }

    int cnt = 1;
    for (auto& vec : d->timersActions) {
        qDebug() << "\n***************** TIMER " << cnt++ << " *****************\n";

        for (auto& el : vec) {
            qDebug() << "name: " << el.name << ", val: " << el.value << ", pre-delay: " << el.preDelay
                     << ", post-delay: " << el.postDelay;
        }
    }
}
