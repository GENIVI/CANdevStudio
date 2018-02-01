#ifndef CRVGUIINTERFACE_H
#define CRVGUIINTERFACE_H

#include <Qt>
#include <functional>

class QWidget;
class QAbstractItemModel;

struct CRVGuiInterface {
    typedef std::function<void()> clear_t;
    typedef std::function<void()> dockUndock_t;
    typedef std::function<void(int)> sectionClicked_t;
    typedef std::function<void(bool)> filter_t;

    virtual void setClearCbk(const clear_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void setSectionClikedCbk(const sectionClicked_t& cb) = 0;
    virtual void setFilterCbk(const filter_t& cb) = 0;

    virtual ~CRVGuiInterface()
    {
    }

    virtual QWidget* mainWidget() = 0;
    virtual void setModel(QAbstractItemModel* model) = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual bool isViewFrozen() = 0;
    virtual void setViewFrozen(bool state) = 0;
    virtual void scrollToBottom() = 0;
    virtual Qt::SortOrder getSortOrder() = 0;
    virtual QString getClickedColumn(int ndx) = 0;
    virtual void setSorting(int sortNdx, Qt::SortOrder order) = 0;
    virtual QString getWindowTitle() = 0;
    virtual bool isColumnHidden(int ndx) = 0;
    virtual void getColumnProper(int ndx, int& vIdx) = 0;
    virtual void setColumnProper(int vIdxFrom, int vIdxTo) = 0;
};

#endif // CRVGUIINTERFACE_H
