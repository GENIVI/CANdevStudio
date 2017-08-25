#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QtCore/QString>
#include <QtCore/Qt> // SortOrder

#include <functional>  // function

class CanRawView;
class QAbstractItemModel;
class QWidget;

template<class Subject>
struct UIBackend;

template<>
struct UIBackend<CanRawView>
{
    virtual QString getClickedColumn(int ndx) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual int getSortOrder() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void setClearCbk(std::function<void ()> cb) = 0;
    virtual void setDockUndockCbk(std::function<void ()> cb) = 0;
    virtual void setSectionClikedCbk(std::function<void (int)> cb) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
    virtual void updateScroll() = 0;

    virtual ~UIBackend() = default;
};

#endif // UIBACKEND_H
