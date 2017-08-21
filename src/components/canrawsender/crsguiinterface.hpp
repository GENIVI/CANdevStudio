#ifndef CRSGUIINTERFACE_HPP
#define CRSGUIINTERFACE_HPP

#include <functional>
#include <QModelIndex>
class QWidget;
class QAbstractItemModel;

struct CRSGuiInterface {
    typedef std::function<void()> add_t;
    typedef std::function<void()> remove_t;
    typedef std::function<void()> dockUndock_t;

    virtual void setAddCbk(const add_t& cb) = 0;
    virtual void setRemoveCbk(const remove_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;


    virtual ~CRSGuiInterface() {}

    virtual QWidget* getMainWidget() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;

    virtual QModelIndexList getSelectedRows() = 0;
    virtual void setIndexWidget(const QModelIndex &index, QWidget *widget) = 0;
};

#endif // CRSGUIINTERFACE_HPP
