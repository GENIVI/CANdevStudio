#ifndef CRSGUIINTERFACE_H
#define CRSGUIINTERFACE_H

#include <QModelIndex>
#include <functional>
#include <memory>
class QWidget;
class QAbstractItemModel;
class CanRawSender;
class NewLineManager;

struct CRSGuiInterface {
    virtual ~CRSGuiInterface()
    {
    }

    typedef std::function<void()> add_t;
    typedef std::function<void()> remove_t;
    typedef std::function<void()> dockUndock_t;
    virtual void setAddCbk(const add_t& cb) = 0;
    virtual void setRemoveCbk(const remove_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;

    virtual QWidget* mainWidget() = 0;
    virtual void initTableView(QAbstractItemModel& _tvModel) = 0;
    virtual QModelIndexList getSelectedRows() = 0;
    virtual void setIndexWidget(const QModelIndex& index, QWidget* widget) = 0;
};
#endif // CRSGUIINTERFACE_H
