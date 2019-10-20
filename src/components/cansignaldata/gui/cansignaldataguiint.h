#ifndef CANSIGNALDATAGUIINT_H
#define CANSIGNALDATAGUIINT_H

#include <Qt>
#include <functional>

class QWidget;
class QAbstractItemModel;
class SearchModel;

struct CanSignalDataGuiInt {
    typedef std::function<void()> msgView_t;
    typedef std::function<void()> dockUndock_t;
    typedef std::function<void()> msgSettingsUpdated_t;

    virtual ~CanSignalDataGuiInt()
    {
    }

    virtual QWidget* mainWidget() = 0;
    virtual void setMsgView(QAbstractItemModel& tvModel) = 0;
    virtual void setSigView(QAbstractItemModel& tvModel) = 0;
    virtual void setMsgViewCbk(const msgView_t& cb) = 0;
    virtual void setMsgSettingsUpdatedCbk(const msgSettingsUpdated_t& cb) = 0;
    virtual void setDockUndockCbk(const dockUndock_t& cb) = 0;
    virtual void initSearch(SearchModel& model) = 0;
};

#endif // CANSIGNALDATAGUIINT_H
