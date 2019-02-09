#ifndef SRC_COMMON_CONFIGHELPERS_H_
#define SRC_COMMON_CONFIGHELPERS_H_

#include "componentinterface.h"
#include <QObject>
#include <QVariant>

#include <map>
#include <memory>

class configHelpers {

public:
    static std::shared_ptr<QObject> getQConfig(
        const ComponentInterface::ComponentProperties& sp, const std::map<QString, QVariant>& properties)
    {
        std::shared_ptr<QObject> q = std::make_shared<QObject>();

        QStringList props;
        for (auto& p : sp) {
            if (std::get<2>(p)) {
                // property editable
                QString propName = std::get<0>(p);
                props.push_back(propName);
                q->setProperty(propName.toStdString().c_str(), properties.at(propName));
            }
        }

        q->setProperty("exposedProperties", props);

        return q;
    }

    static void setQConfig(const QObject& qobject, const ComponentInterface::ComponentProperties& sp,
        std::map<QString, QVariant>& properties)
    {
        for (auto& p : sp) {
            QString propName = std::get<0>(p);
            QVariant v = qobject.property(propName.toStdString().c_str());
            if (v.isValid() && v.type() == std::get<1>(p)) {
                properties[propName] = v;
            }
        }
    }
};

#endif /* SRC_COMMON_CONFIGHELPERS_H_ */
