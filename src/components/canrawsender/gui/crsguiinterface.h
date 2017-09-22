#ifndef CRSGUIINTERFACE_H
#define CRSGUIINTERFACE_H

#include <QModelIndex>
#include <functional>
#include <memory>
class QWidget;
class QAbstractItemModel;
class CanRawSender;
class NewLineManager;
class SortModel;

struct CRSGuiInterface {
    virtual ~CRSGuiInterface()
    {
    }

    typedef std::function<void()> add_t;
    typedef std::function<void()> remove_t;
    typedef std::function<void()> dockUndock_t;
    typedef std::function<void(int)> sectionClicked_t;

    virtual void setAddCbk(const add_t& cb) = 0;
    virtual void setRemoveCbk(const remove_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void setSectionClikedCbk(const sectionClicked_t& cb) = 0;

    virtual QWidget* mainWidget() = 0;
    virtual void initTableView(QAbstractItemModel& _tvModel, CanRawSender* ptr) = 0;
    virtual QModelIndexList getSelectedRows() = 0;
    virtual void setIndexWidget(const QModelIndex& index, QWidget* widget) = 0;
    virtual void setWidgetPersistent(const QModelIndex& index) = 0;
    virtual void setModel(QAbstractItemModel* model) = 0;

    virtual Qt::SortOrder getSortOrder() = 0;
    virtual QString getClickedColumn(int ndx) = 0;
    virtual void setSorting(int sortNdx, Qt::SortOrder order) = 0;
};
#endif // CRSGUIINTERFACE_H
