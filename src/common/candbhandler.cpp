#include "candbhandler.h"
#include "bcastmsgs.h"
#include <QJsonObject>
#include <QVariant>
#include <log.h>
#include <propertyfields.h>

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
        id = QUuid(vId.toString());
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
        _dbColor[id] = msg["color"].toString();

        if (_currentDb.isNull()) {
            _currentDb = id;
        }

        if (_currentDb == id) {
            _props["color"] = _dbColor[id];
            emit requestRedraw();
            emit dbChanged();
        }
    } else if ((vMsg.isValid() && vMsg.toString() == BcastMsg::ConfigChanged) && (type == "CanSignalData")) {
        _dbNames[id] = name;

        if (msg["config"].isObject()) {
            _dbColor[id] = msg["config"].toObject()["color"].toString();
        }

        if (_currentDb == id) {
            _props["color"] = _dbColor[id];
            emit requestRedraw();
        }
    } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::NodeDeleted) {
        _dbNames.erase(id);
        _candb.erase(id);
        _dbColor.erase(id);

        dbDeleted(id);
    } else if (vMsg.isValid() && vMsg.toString() == BcastMsg::InitDone) {
        QJsonObject msg;
        msg["msg"] = BcastMsg::RequestCanDb;

        emit sendCanDbRequest(msg);
    }
}

const CANmessages_t& CanDbHandler::getDb() const
{
    if (_candb.count(_currentDb) > 0) {
        return _candb.at(_currentDb);
    }

    return _emptyDb;
}

const QString CanDbHandler::getName() const
{
    if (_dbNames.count(_currentDb) > 0) {
        return _dbNames.at(_currentDb);
    }

    return "";
}

void CanDbHandler::updateCurrentDbFromProps()
{
    _currentDb = QUuid(_props[_dbProperty].toString());
    _props["color"] = _dbColor[_currentDb];
    emit requestRedraw();
    emit dbChanged();
}

QWidget* CanDbHandler::createPropertyWidget()
{
    auto* p = new PropertyFieldCombo(false);

    for (auto& item : _dbNames) {
        p->addItem(item.second, item.first);
    }

    p->sort();
    p->setPropText(getName());

    return p;
}

void CanDbHandler::dbDeleted(const QUuid& id)
{
    if (id == _currentDb) {
        _props[_dbProperty] = "";
        _props["color"] = "";
        for (auto& iter : _dbNames) {
            if (!iter.first.isNull()) {
                _props[_dbProperty] = iter.first.toString();
            }
        }

        updateCurrentDbFromProps();
    }
}
