#include "bcastmsgs.h"
#include <QJsonObject>
#include <QVariant>
#include <log.h>
#include <propertyfields.h>
#include "candbhandler.h"

CanDbHandler::CanDbHandler(ComponentInterface::PropertyContainer& props, const QString& dbProperty)
    : _props(props)
    , _dbProperty(dbProperty)
{
}

void CanDbHandler::processBcast(const QJsonObject& msg, const QVariant& param)
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
        QJsonObject msg;
        msg["msg"] = BcastMsg::RequestCanDb;

        emit sendCanDbRequest(msg);
    }
}

CANmessages_t& CanDbHandler::getDb()
{
    return _candb[_currentDb];
}

QString CanDbHandler::getName()
{
    if (_dbNames.size() == 0) {
        // Avoid creation of empty item in _dbNames
        return "";
    } else {
        return _dbNames[_currentDb];
    }
}

void CanDbHandler::updateCurrentDbFromProps()
{
    _currentDb = QUuid::fromString(_props[_dbProperty].toString());
}

QWidget* CanDbHandler::createPropertyWidget()
{
    auto* p = new PropertyFieldCombo();

    for (auto& item : _dbNames) {
        p->addItem(item.second, item.first);
    }

    p->setPropText(getName());

    return p;
}

void CanDbHandler::dbDeleted(const QUuid& id)
{
    if (id == _currentDb) {
        _props[_dbProperty] = "";
        for (auto& iter : _dbNames) {
            if (!iter.first.isNull()) {
                _props[_dbProperty] = iter.first.toString();
                _currentDb = iter.first;
            }
        }
    }
}
