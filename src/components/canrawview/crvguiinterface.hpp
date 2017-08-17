#ifndef CRVGUIINTERFACE_H
#define CRVGUIINTERFACE_H

#include <functional>

class QWidget;
class QAbstractItemModel;

struct CRVGuiInterface {
    typedef std::function<void()> clear_t;
    typedef std::function<void()> dockUndock_t;
    typedef std::function<void(int)> sectionClicked_t;

    virtual void setClearCbk(const clear_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void setSectionClikedCbk(const sectionClicked_t& cb) = 0;

    virtual ~CRVGuiInterface() {}

    virtual QWidget* getMainWidget() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void updateScroll() = 0;
    virtual int getSortOrder() = 0;
    virtual QString getClickedColumn(int ndx) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
};

#endif // CRVGUIINTERFACE_H
