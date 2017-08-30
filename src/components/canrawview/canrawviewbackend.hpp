
#ifndef CANRAWVIEWBACKEND_H
#define CANRAWVIEWBACKEND_H

#include <Qt> // SortOrder

#include "uibackend.h" // UIBackend, UIBackendDefault

#include <functional> // function


class QAbstractItemModel;
class CanRawView;
class CanRawViewPrivate;
class QWidget;

namespace Ui { class CanRawViewPrivate; }


template<>
class UIBackendDefault<CanRawView> : public UIBackend<CanRawView>
{

 public:

    UIBackendDefault();
    virtual ~UIBackendDefault() = default;

    QString getClickedColumn(int ndx) override;
    QWidget* getMainWidget() override;
    bool isColumnHidden(int column) override;
    bool isFrozen() override;
    int getSortIndicator() override;
    int getSortOrder() override;
    void initTableView(QAbstractItemModel& tvModel) override;
    void setClearCbk(std::function<void ()> cb) override;
    void setDockUndockCbk(std::function<void ()> cb) override;
    void setSectionClikedCbk(std::function<void (int)> cb) override;
    void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) override;
    void updateScroll() override;

 private:

    Ui::CanRawViewPrivate* ui     = nullptr;
    QWidget*               widget = nullptr;
};

using CanRawViewBackend = UIBackendDefault<CanRawView>;

#endif // CANRAWVIEWBACKEND_H

