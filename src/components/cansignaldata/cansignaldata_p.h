#ifndef CANSIGNALDATA_P_H
#define CANSIGNALDATA_P_H

#include "cansignaldata.h"
#include "gui/cansignaldataguiimpl.h"
#include "searchmodel.h"
#include <QStandardItemModel>
#include <QtCore/QObject>
#include <memory>
#include <propertyfields.h>

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
    void sendCANdbUpdated();

private:
    void initProps();
    std::string loadFile(const std::string& filename);
    using msgSettings_t = std::map<uint32_t, std::pair<QString, QString>>;
    msgSettings_t getMsgSettings();
    void setMsgSettings(const msgSettings_t& msgSettings);

public:
    bool _simStarted{ false };
    CanSignalDataCtx _ctx;
    CanSignalDataGuiInt& _ui;
    bool _docked{ true };
    bool _msgView{ true };
    std::map<QString, QVariant> _props;
    QStringList _columnsSig;
    QStringList _columnsMsg;
    QStandardItemModel _tvModelSig;
    SearchModel _tvModelSigFilter;
    QStandardItemModel _tvModelMsg;
    SearchModel _tvModelMsgFilter;
    CANmessages_t _messages;

private:
    CanSignalData* q_ptr;
    const QString _fileProperty = "file";
    const QString _nameProperty = "name";
    const QString _colorProperty = "color";
    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty,  QVariant::String, true, cf(nullptr)),
            std::make_tuple(_fileProperty, QVariant::String, true, cf([] { return new PropertyFieldPath; } )),
            std::make_tuple(_colorProperty, QVariant::String, true, cf([] { return new PropertyFieldColor; } ))
    };
    // clang-format on

    std::string _currentDbcFile;
    msgSettings_t _msgSettings;
};

#endif // CANSIGNALDATA_P_H
