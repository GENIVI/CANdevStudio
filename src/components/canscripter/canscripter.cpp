#include "canscripter.h"
#include "canscripter_p.h"
#include <QCanBusFrame>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

CanScripter::CanScripter(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanScripterPrivate())
{
    Q_D(CanScripter);

    QFile file("/home/remol/Projects/CanDevStudio/genivi-script2.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file '" << file.fileName() << "'";
    }

    QString script = file.readAll();
    QJsonDocument jf = QJsonDocument::fromJson(script.toUtf8());

    if(jf.isObject()) {
        d->parseScriptObject(jf.object());
    } else {
        qDebug() << "ERROR: Root object shall be JsonObject";
    }

}

CanScripter::~CanScripter()
{
}
