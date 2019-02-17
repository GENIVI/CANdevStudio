#ifndef CANRAWFILTER_P_H
#define CANRAWFILTER_P_H

#include "canrawfilter.h"
#include "gui/canrawfilterguiimpl.h"
#include <QtCore/QObject>
#include <memory>

class CanRawFilter;

class CanRawFilterPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawFilter)

public:
    CanRawFilterPrivate(CanRawFilter* q, CanRawFilterCtx&& ctx = CanRawFilterCtx(new CanRawFilterGuiImpl));
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    bool acceptRxFrame(const QCanBusFrame& frame);
    bool acceptTxFrame(const QCanBusFrame& frame);

private:
    void initProps();
    bool acceptFrame(const CanRawFilterGuiInt::AcceptList_t& list, const QCanBusFrame& frame);
    CanRawFilterGuiInt::AcceptList_t getAcceptList(const QJsonObject& json, const QString& listName);

public:
    bool _simStarted{ false };
    CanRawFilterCtx _ctx;
    CanRawFilterGuiInt& _ui;
    bool _docked{ true };
    std::map<QString, QVariant> _props;

private:
    CanRawFilterGuiInt::AcceptList_t _rxAcceptList;
    CanRawFilterGuiInt::AcceptList_t _txAcceptList;
    CanRawFilter* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};

#endif // CANRAWFILTER_P_H
