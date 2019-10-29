#ifndef __CANDBHANDLER_H
#define __CANDBHANDLER_H

#include "componentinterface.h"
#include <QObject>
#include <QUuid>
#include <cantypes.hpp>

class QString;
class QJsonObject;

Q_DECLARE_METATYPE(CANmessages_t);

class CanDbHandler : public QObject {
    Q_OBJECT

public:
    CanDbHandler(ComponentInterface::PropertyContainer& props, const QString& dbProperty);
    void processBcast(const QJsonObject& msg, const QVariant& param);
    CANmessages_t& getDb();
    QString getName();
    void updateCurrentDbFromProps();
    QWidget* createPropertyWidget();

signals:
    void sendCanDbRequest(const QJsonObject& msg, const QVariant = QVariant());
    void requestRedraw();

private:
    void dbDeleted(const QUuid& id);

private:
    ComponentInterface::PropertyContainer& _props;
    const QString& _dbProperty;
    std::map<QUuid, QString> _dbNames;
    std::map<QUuid, CANmessages_t> _candb;
    std::map<QUuid, QString> _dbColor;
    QUuid _currentDb;
};

#endif /* !__CANDBHANDLER_H */
