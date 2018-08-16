#ifndef CANSIGNALDATA_P_H
#define CANSIGNALDATA_P_H

#include <QtCore/QObject>
#include <memory>
#include "gui/cansignaldataguiimpl.h"
#include "cansignaldata.h"
#include <QStandardItemModel>
#include "searchmodel.h"

class CanSignalData;

class CanSignalDataPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalData)

public:
    CanSignalDataPrivate(CanSignalData* q, CanSignalDataCtx&& ctx = CanSignalDataCtx(new CanSignalDataGuiImpl));
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void loadDbc(const std::string& filename);
    std::pair<CANmessage, std::vector<CANsignal>>* findInDb(uint32_t id);

private:
    void initProps();
    std::string loadFile(const std::string& filename);

public:
    bool _simStarted{ false };
    CanSignalDataCtx _ctx;
    CanSignalDataGuiInt& _ui;
    bool _docked{ true };
    bool _settings{ false };
    std::map<QString, QVariant> _props;
    QStringList _columnsOrder;
    QStringList _columnsSettings;
    QStandardItemModel _tvModel;
    SearchModel _tvModelFilter;
    QStandardItemModel _tvModelSettings;
    SearchModel _tvModelSettingsFilter;
    CANmessages_t _messages;

private:
    std::map<uint32_t, std::pair<QString, QString>> _msgSettings;
    CanSignalData* q_ptr;
    const QString _fileProperty = "file";
    const QString _nameProperty = "name";
    ComponentInterface::ComponentProperties _supportedProps = {
            {_nameProperty,   {QVariant::String, true}},
            {_fileProperty,   {QVariant::String, true}}
    };
};

#endif // CANSIGNALDATA_P_H
