#include "cansignalsender_p.h"

CanSignalSenderPrivate::CanSignalSenderPrivate(CanSignalSender* q, CanSignalSenderCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalSenderGuiInt>())
    , _tvColumns({ "Id", "Signal name", "Value", "" })
    , q_ptr(q)
{
    initProps();

    connect(&_db, &CanDbHandler::sendCanDbRequest, q_ptr, &CanSignalSender::simBcastSnd);
    connect(&_db, &CanDbHandler::requestRedraw, q_ptr, &CanSignalSender::requestRedraw);

    _tvModel.setHorizontalHeaderLabels(_tvColumns);
    _ui.initTv(_tvModel, &_db);

    connect(&_tvModel, &CanSignalSenderTableModel::droppedItems, this, &CanSignalSenderPrivate::setDroppedItems);

    _ui.setDockUndockCbk([this] {
        _docked = !_docked;
        emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
    });

    _ui.setAddCbk([this] { _ui.addRow(); });

    _ui.setRemoveCbk([this] {
        QModelIndexList IndexList = _ui.getSelectedRows();
        std::list<QModelIndex> tmp(IndexList.begin(), IndexList.end());

        tmp.sort(); // List must to be sorted and reversed because erasing started from last row
        tmp.reverse();

        for (QModelIndex n : tmp) {
            _tvModel.removeRow(n.row()); // Delete line from table view
        }
    });

    _ui.setSendCbk([this](const QString& id, const QString& name, const QVariant& val) {
        if (_simStarted) {
            uint32_t idNum = id.toUInt(nullptr, 16);
            std::string sigName;

            if (idNum > 0x7ff) {
                sigName = fmt::format("0x{:08x}_{}", idNum, name.toStdString());
            } else {
                sigName = fmt::format("0x{:03x}_{}", idNum, name.toStdString());
            }

            emit q_ptr->sendSignal(sigName.c_str(), val);
        }
    });
}

void CanSignalSenderPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

void CanSignalSenderPrivate::setDroppedItems(const QList<QList<QString>>& droppedItems)
{
    for (const auto& rowItem : droppedItems) {
        if (2 > rowItem.size()) {
            cds_warn("{} wrong row item.", Q_FUNC_INFO);
            continue;
        }

        QString strMsgId = rowItem[0];
        uint32_t msgId = strMsgId.toUInt(nullptr, 16);
        if (_db.getDb().end() == _db.getDb().find(msgId)) {
            cds_warn("{} cannot find message id({})", Q_FUNC_INFO, strMsgId.toStdString().c_str());
            continue;
        }

        QString sigName = rowItem[1];
        if ("" == sigName) {
            std::vector<CANsignal> canSignals = _db.getDb().at(msgId);
            for (const auto& signal : canSignals) {
                _ui.addRow(strMsgId, QString::fromStdString(signal.signal_name));
            }
        } else {
            _ui.addRow(strMsgId, sigName);
        }
    }
}

ComponentInterface::ComponentProperties CanSignalSenderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalSenderPrivate::getSettings()
{
    QJsonObject ret;

    for (const auto& p : _props) {
        ret[p.first] = QJsonValue::fromVariant(p.second);
    }

    ret["rows"] = _ui.getRows();

    return ret;
}

void CanSignalSenderPrivate::setSettings(const QJsonObject& json)
{
    if (json.contains("rows")) {
        if (json["rows"].type() == QJsonValue::Array) {
            auto rowArray = json["rows"].toArray();

            for (int i = 0; i < rowArray.size(); ++i) {
                if (rowArray[i].type() == QJsonValue::Object) {
                    auto row = rowArray[i].toObject();
                    QString id, sig, val;

                    if (row.contains("id")) {
                        id = row["id"].toString();

                        if (row.contains("sig")) {
                            sig = row["sig"].toString();
                        }

                        if (row.contains("val")) {
                            val = row["val"].toString();
                        }

                        _ui.addRow(id, sig, val);
                    }
                } else {
                    cds_warn("rows array element expected to be object!");
                }
            }
        } else {
            cds_warn("rows expected to be array!");
        }
    } else {
        cds_info("Rows to restore not found");
    }

    _db.updateCurrentDbFromProps();
}
