#ifndef __CANDBHANDLER_H
#define __CANDBHANDLER_H

#include <QJsonObject>
#include <QUuid>
#include <map>
#include "bcastmsgs.h"
#include <QObject>
#include <cantypes.hpp>
#include <QVariant>

Q_DECLARE_METATYPE(CANmessages_t);

class CanDbHandler : public QObject {
    Q_OBJECT

public:
    void processBcast(const QJsonObject& msg, const QVariant& param)
    {
        QVariant vId = msg["id"];
        QUuid id;
        if (vId.isValid()) {
            id = qvariant_cast<QUuid>(vId);
        }

        QString name = msg["caption"].toString();

        QVariant vMsg = msg["msg"];
        if (vMsg.isValid() && vMsg.toString() == BcastMsg::CanDbUpdated) {
            CANmessages_t paramMsg;
            if (param.isValid()) {
                paramMsg = qvariant_cast<CANmessages_t>(param);
            }

            _candb[id] = paramMsg;
            _dbNames[id] = name;

            if (_currentDb.isNull()) {
                _currentDb = id;

                emit currentDbNameChanged(name);
            }
        } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::ConfigChanged) {
            _dbNames[id] = name;

            if (id == _currentDb) {
                emit currentDbNameChanged(name);
            }
        } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::InitDone) {
            emit sendCanDbRequest();
        }
    }

public:
    std::map<QUuid, QString> _dbNames;
    std::map<QUuid, CANmessages_t> _candb;
    QUuid _currentDb;

signals:
    void currentDbNameChanged(const QString& name);
    void sendCanDbRequest();

};

#endif /* !__CANDBHANDLER_H */
