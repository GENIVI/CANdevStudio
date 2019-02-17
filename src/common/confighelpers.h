#ifndef SRC_COMMON_CONFIGHELPERS_H_
#define SRC_COMMON_CONFIGHELPERS_H_

#include "componentinterface.h"
#include <QWidget>
#include <QVariant>

#include <map>
#include <memory>

class configHelpers {

public:
    static std::shared_ptr<QWidget> getQConfig(
        const ComponentInterface::ComponentProperties& sp, const std::map<QString, QVariant>& properties)
    {
        std::shared_ptr<QWidget> q = std::make_shared<QWidget>();

        QStringList props;
        for (auto& p : sp) {
            if (ComponentInterface::propertyEditability(p)) {
                // property editable
                const QString& propName = ComponentInterface::propertyName(p);
                props.push_back(propName);
                q->setProperty(propName.toStdString().c_str(), properties.at(propName));
            }
        }

        q->setProperty("exposedProperties", props);

        return q;
    }

    static void setQConfig(const QWidget& qobject, const ComponentInterface::ComponentProperties& sp,
        std::map<QString, QVariant>& properties)
    {
        for (auto& p : sp) {
            const QString& propName = ComponentInterface::propertyName(p);
            QVariant v = qobject.property(propName.toStdString().c_str());
            if (v.isValid() && v.type() == ComponentInterface::propertyType(p)) {
                properties[propName] = v;
            }
        }
    }
};

#endif /* SRC_COMMON_CONFIGHELPERS_H_ */
