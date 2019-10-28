#ifndef __CANDBHANDLER_H
#define __CANDBHANDLER_H

#include "bcastmsgs.h"
#include <QJsonObject>
#include <QObject>
#include <QUuid>
#include <QVariant>
#include <cantypes.hpp>
#include <log.h>
#include <map>

Q_DECLARE_METATYPE(CANmessages_t);

class CanDbHandler : public QObject {
    Q_OBJECT

public:
    void processBcast(const QJsonObject& msg, const QVariant& param)
    {
        QVariant vId = msg["id"];
        QUuid id;
        if (vId.isValid()) {
            id = QUuid::fromString(vId.toString());
        }

        QString name = msg["caption"].toString();
        QString type = msg["name"].toString();

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
            }
        } else if ((vMsg.isValid() && vMsg.toString() == BcastMsg::ConfigChanged) && (type == "CanSignalData")) {
            _dbNames[id] = name;
        } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::NodeDeleted) {
            _dbNames.erase(id);
            _candb.erase(id);

            emit dbDeleted(id);
        } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::InitDone) {
            emit sendCanDbRequest();
        }
    }

    CANmessages_t& getDb()
    {
        return _candb[_currentDb];
    }

    QString getName()
    {
        if (_currentDb.isNull()) {
            return "";
        } else {
            return _dbNames[_currentDb];
        }
    }

    void updateCurrentDb(const QVariant& id)
    {
        updateCurrentDb(id.toString());
    }

    void updateCurrentDb(const QString& id)
    {
        _currentDb = QUuid::fromString(id);

        cds_debug("Current DB {}", _currentDb.toString().toStdString());
    }

public:
    std::map<QUuid, QString> _dbNames;
    std::map<QUuid, CANmessages_t> _candb;
    QUuid _currentDb;

signals:
    void sendCanDbRequest();
    void dbDeleted(const QUuid& id);
};

#endif /* !__CANDBHANDLER_H */
