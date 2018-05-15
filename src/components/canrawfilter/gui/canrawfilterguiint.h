#ifndef CANRAWFILTERGUIINT_H
#define CANRAWFILTERGUIINT_H

#include <Qt>
#include <functional>
#include <vector>
#include <tuple>

class QWidget;

struct CanRawFilterGuiInt {
    typedef std::vector<std::tuple<QString, QString, bool>> acceptList_t;
    typedef std::function<void(acceptList_t &list)> listUpdated_t;

    virtual ~CanRawFilterGuiInt()
    {
    }

    virtual QWidget* mainWidget() = 0;
    virtual void setTxListCbk(const listUpdated_t& cb) = 0;
    virtual void setRxListCbk(const listUpdated_t& cb) = 0;
};

#endif // CANRAWFILTERGUIINT_H
