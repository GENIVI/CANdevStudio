
#ifndef CANRAWVIEWBACKEND_H
#define CANRAWVIEWBACKEND_H

#include <Qt> // SortOrder

#include "uibackendiface.h" // UIBackend, UIBackendDefault

#include <functional> // function


class QAbstractItemModel;
class CanRawView;
class CanRawViewPrivate;
class QWidget;


namespace Ui { class CanRawViewPrivate; }


template<>
struct UIBackend<CanRawView>  // polymorphic as an example, but it's optional, see the note on top
{
    virtual QString getClickedColumn(int ndx) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual bool isColumnHidden(int column) const = 0;
    virtual bool isFrozen() const = 0;
    virtual int getSortIndicator() = 0;
    virtual Qt::SortOrder getSortOrder() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void setClearCbk(std::function<void ()> cb) = 0;
    virtual void setDockUndockCbk(std::function<void ()> cb) = 0;
    virtual void setSectionClikedCbk(std::function<void (int)> cb) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
    virtual void updateScroll() = 0;

    virtual ~UIBackend() = default;
};


template<>
class UIBackendDefault<CanRawView> : public UIBackend<CanRawView>
{

 public:

    UIBackendDefault();
    virtual ~UIBackendDefault() = default;

    QString getClickedColumn(int ndx) override;
    QWidget* getMainWidget() override;
    bool isColumnHidden(int column) const override;
    bool isFrozen() const override;
    int getSortIndicator() override;
    Qt::SortOrder getSortOrder() override;
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

