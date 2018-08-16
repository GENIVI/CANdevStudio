#ifndef CANSIGNALDATAGUIINT_H
#define CANSIGNALDATAGUIINT_H

#include <Qt>
#include <functional>

class QWidget;
class QAbstractItemModel;
class SearchModel;

struct CanSignalDataGuiInt {
    typedef std::function<void()> settings_t;
    typedef std::function<void()> dockUndock_t;

    virtual ~CanSignalDataGuiInt()
    {
    }

    virtual QWidget* mainWidget() = 0;
    virtual void initSettings(QAbstractItemModel& tvModel) = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void setSettingsCbk(const settings_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void initSearch(SearchModel& model) = 0;
};

#endif // CANSIGNALDATAGUIINT_H
