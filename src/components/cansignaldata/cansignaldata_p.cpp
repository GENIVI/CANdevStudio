#include "cansignaldata_p.h"
#include <QJsonArray>
#include <atomic>
#include <bcastmsgs.h>
#include <dbcparser.h>
#include <fstream>
#include <log.h>

namespace {
std::atomic<uint32_t> g_cnt(0);

const std::vector<QColor> g_colorTab
    = { QColor("#DCDAA4"), QColor("#9CD59B"), QColor("#4ED1DF"), QColor("#FFCCBB"), QColor("#DAB7EB")};

QString getDefaultColor()
{
    uint32_t cnt = g_cnt.load();
    QString ret = g_colorTab[cnt % g_colorTab.size()].name(QColor::HexRgb).toUpper();
    g_cnt.fetch_add(1);

    return ret;
}
} // namespace

CanSignalDataPrivate::CanSignalDataPrivate(CanSignalData* q, CanSignalDataCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalDataGuiInt>())
    , _columnsSig({ "id", "name", "start", "length", "type", "order", "factor", "offset", "min", "max" })
    , _columnsMsg({ "id", "name", "dlc", "ecu", "cycle", "initial value" })
    , q_ptr(q)
{
    initProps();

    _tvModelSig.setHorizontalHeaderLabels(_columnsSig);
    _tvModelSigFilter.setSourceModel(&_tvModelSig);
    _ui.initSearch(_tvModelSigFilter);

    _tvModelMsg.setHorizontalHeaderLabels(_columnsMsg);
    _tvModelMsgFilter.setSourceModel(&_tvModelMsg);
    _ui.initSearch(_tvModelMsgFilter);

    _ui.setMsgView(_tvModelMsgFilter);

    _ui.setMsgViewCbk([this] {
        _msgView = !_msgView;

        if (_msgView) {
            _ui.setMsgView(_tvModelMsgFilter);
        } else {
            _ui.setSigView(_tvModelSigFilter);
        }
    });

    _ui.setDockUndockCbk([this] {
        _docked = !_docked;
        emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
    });

    _ui.setMsgSettingsUpdatedCbk([this] {
        setMsgSettings(getMsgSettings());
        sendCANdbUpdated();
    });
}

void CanSignalDataPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        QString propName = ComponentInterface::propertyName(p);
        _props[propName];
    }

    _props[_colorProperty] = getDefaultColor();
}

ComponentInterface::ComponentProperties CanSignalDataPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

CanSignalDataPrivate::msgSettings_t CanSignalDataPrivate::getMsgSettings()
{
    msgSettings_t msgSettings;

    for (int i = 0; i < _tvModelMsg.rowCount(); ++i) {
        uint32_t id = _tvModelMsg.item(i, 0)->data(Qt::DisplayRole).toString().toUInt(nullptr, 16);
        auto cyclePtr = _tvModelMsg.item(i, 4);
        auto initValPtr = _tvModelMsg.item(i, 5);
        QVariant cycle;
        QVariant initVal;

        if (cyclePtr) {
            cycle = cyclePtr->data(Qt::DisplayRole);
        }

        if (initValPtr) {
            initVal = initValPtr->data(Qt::DisplayRole);
        }

        if (cycle.toString().length() > 0 || initVal.toString().length() > 0) {
            msgSettings[id] = std::make_pair(cycle.toString(), initVal.toString());
        }
    }

    return msgSettings;
}

void CanSignalDataPrivate::setMsgSettings(const msgSettings_t& msgSettings)
{
    // Clear current settings
    for (std::pair<CANmessage, std::vector<CANsignal>>&& msg : _messages) {
        msg.first.updateCycle = 0;
        msg.first.initValue = "";
    }

    for (const auto& msg : msgSettings) {
        auto msgDb = findInDb(msg.first);
        if (msgDb) {
            msgDb->first.updateCycle = msg.second.first.toUInt();
            msgDb->first.initValue = msg.second.second.toStdString();
        }
    }

    _msgSettings = msgSettings;
}

QJsonObject CanSignalDataPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    QJsonArray array;

    for (const auto& msg : _msgSettings) {
        QJsonObject obj;
        obj["id"] = QString::number(msg.first, 16);
        obj["cycle"] = msg.second.first;
        obj["initVal"] = msg.second.second;

        array.append(obj);
    }

    json["msgSettings"] = array;

    return json;
}

void CanSignalDataPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        QString propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }

    _msgSettings.clear();
    if (json.contains("msgSettings")) {
        if (json["msgSettings"].type() == QJsonValue::Array) {
            auto rowArray = json["msgSettings"].toArray();

            for (int i = 0; i < rowArray.size(); ++i) {
                if (rowArray[i].type() == QJsonValue::Object) {
                    auto row = rowArray[i].toObject();
                    QString id, cycle, initVal;

                    if (row.contains("id")) {
                        id = row["id"].toString();
                    }

                    if (row.contains("cycle")) {
                        cycle = row["cycle"].toString();
                    }

                    if (row.contains("initVal")) {
                        initVal = row["initVal"].toString();
                    }

                    if (id.length() && (cycle.length() || initVal.length())) {
                        _msgSettings[id.toUInt(nullptr, 16)] = std::make_pair(cycle, initVal);
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

    setMsgSettings(_msgSettings);
}

std::string CanSignalDataPrivate::loadFile(const std::string& filename)
{
    const std::string path = filename;

    std::fstream file{ path.c_str() };

    if (!file.good()) {
        cds_error("File {} does not exists", filename);
    }

    std::string buff;
    std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(buff));

    file.close();
    return buff;
}

void CanSignalDataPrivate::loadDbc(const std::string& filename)
{
    if (_currentDbcFile == filename) {
        cds_info("DBC filename not changed. Loading aborted");
        return;
    }

    _currentDbcFile = filename;
    _messages.clear();

    CANdb::DBCParser parser;
    bool success = parser.parse(loadFile(filename));

    if (!success) {
        cds_error("Failed to load CAN DB from '{}' file", filename);
        // send empty messages to indicate problem
        sendCANdbUpdated();
        return;
    }

    _messages = parser.getDb().messages;

    cds_info("CAN DB load successful. {} records found", _messages.size());

    setMsgSettings(_msgSettings);

    _tvModelSig.removeRows(0, _tvModelSig.rowCount());
    _tvModelMsg.removeRows(0, _tvModelMsg.rowCount());

    for (auto& message : _messages) {
        QList<QStandardItem*> settingsList;
        uint32_t id = message.first.id;
        QString frameID = QString("0x" + QString::number(id, 16));

        settingsList.append(new QStandardItem(frameID));
        settingsList.append(new QStandardItem(message.first.name.c_str()));
        settingsList.append(new QStandardItem(QString::number(message.first.dlc)));
        settingsList.append(new QStandardItem(message.first.ecu.c_str()));

        for (auto& i : settingsList) {
            i->setEditable(false);
        }

        if (message.first.updateCycle > 0) {
            settingsList.append(new QStandardItem(QString::number(message.first.updateCycle)));
        } else {
            settingsList.append(new QStandardItem());
        }

        settingsList.append(new QStandardItem(message.first.initValue.c_str()));

        _tvModelMsg.appendRow(settingsList);

        for (auto& signal : message.second) {
            QList<QStandardItem*> list;

            list.append(new QStandardItem(frameID));
            list.append(new QStandardItem(signal.signal_name.c_str()));
            list.append(new QStandardItem(QString::number(signal.startBit)));
            list.append(new QStandardItem(QString::number(signal.signalSize)));
            list.append(new QStandardItem(signal.valueSigned ? "signed" : "unsigned"));
            list.append(new QStandardItem(QString::number(signal.byteOrder)));
            list.append(new QStandardItem(QString::number(signal.factor)));
            list.append(new QStandardItem(QString::number(signal.offset)));
            list.append(new QStandardItem(QString::number(signal.min)));
            list.append(new QStandardItem(QString::number(signal.max)));

            for (auto& i : list) {
                i->setEditable(false);
            }

            _tvModelSig.appendRow(list);
        }
    }

    sendCANdbUpdated();
}

std::pair<CANmessage, std::vector<CANsignal>>* CanSignalDataPrivate::findInDb(uint32_t id)
{
    CANmessage key(id);
    auto msg = _messages.find(key);

    if (msg != std::end(_messages)) {
        return (std::pair<CANmessage, std::vector<CANsignal>>*)&(*msg);
    }

    return nullptr;
}

void CanSignalDataPrivate::sendCANdbUpdated()
{
    QVariant param;
    param.setValue(_messages);
    QJsonObject msg;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["color"] = _props[_colorProperty].toString();

    emit q_ptr->simBcastSnd(msg, param);
}
