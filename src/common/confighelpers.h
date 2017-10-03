#ifndef SRC_COMMON_CONFIGHELPERS_H_
#define SRC_COMMON_CONFIGHELPERS_H_

#include "componentinterface.h"
#include <QObject>
#include <QVariant>

#include <map>
#include <memory>

class configHelpers
{

public:
static std::shared_ptr<QObject> getQConfig(const ComponentInterface::ComponentProperties& sp,
        const std::map<QString, QVariant>& properties)
{
    std::shared_ptr<QObject> q = std::make_shared<QObject>();

    QStringList props;
    for (auto& p: sp)
    {
        if (p.second.second) // property editable
        {
            props.push_back(p.first);
            q->setProperty(p.first.toStdString().c_str(), properties.at(p.first));
        }
    }

    q->setProperty("exposedProperties", props);

    return q;
}

static void setQConfig(const QObject& qobject,
        const ComponentInterface::ComponentProperties& sp,
        std::map<QString, QVariant>& properties)
{
    for (auto& p: sp)
    {
        QVariant v = qobject.property(p.first.toStdString().c_str());
        if (v.isValid() && v.type() == p.second.first)
            properties[p.first] = v;
    }
}

};

#endif /* SRC_COMMON_CONFIGHELPERS_H_ */
