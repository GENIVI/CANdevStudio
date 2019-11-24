#ifndef CANSIGNALSENDERGUIINT_H
#define CANSIGNALSENDERGUIINT_H

#include <QJsonArray>
#include <QModelIndexList>
#include <Qt>
#include <functional>

class QWidget;
class QStandardItemModel;
class CanDbHandler;

struct CanSignalSenderGuiInt {
    using dockUndock_t = std::function<void()>;
    using remove_t = std::function<void()>;
    using add_t = std::function<void()>;
    using send_t = std::function<void(const QString&, const QString&, const QVariant&)>;

    virtual ~CanSignalSenderGuiInt() {}

    virtual QWidget* mainWidget() = 0;
    virtual void initTv(QStandardItemModel& tvModel, const CanDbHandler* db) = 0;
    virtual void setRemoveCbk(const remove_t& cb) = 0;
    virtual void setAddCbk(const add_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void setSendCbk(const send_t& cbk) = 0;
    virtual void addRow(const QString& id = "", const QString& sig = "", const QString& val = "") = 0;
    virtual QJsonArray getRows() = 0;
    virtual QModelIndexList getSelectedRows() = 0;
};

#endif // CANSIGNALSENDERGUIINT_H
