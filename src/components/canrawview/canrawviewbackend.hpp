
#ifndef CANRAWVIEWBACKEND_H
#define CANRAWVIEWBACKEND_H

#include <Qt> // SortOrder
#include <QtCore/QString>

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
    virtual QString getWindowTitle() = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual Qt::SortOrder getSortOrder() = 0;
    virtual bool isColumnHidden(int column) const = 0;
    virtual bool isViewFrozen() const = 0;
    virtual int getSortSection() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void scrollToBottom() = 0;
    virtual void setClearCbk(std::function<void ()> cb) = 0;
    virtual void setDockUndockCbk(std::function<void ()> cb) = 0;
    virtual void setFilterCbk(std::function<void()> cb) = 0;
    virtual void setModel(QAbstractItemModel* model) = 0;
    virtual void setSectionClikedCbk(std::function<void (int)> cb) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
    virtual void updateScroll() = 0;

    virtual ~UIBackend() = default;
};


template<>
class UIBackendDefault<CanRawView> : public UIBackend<CanRawView>
{

 public:

    QString getClickedColumn(int ndx) override;
    QString getWindowTitle() override;
    QWidget* getMainWidget() override;
    Qt::SortOrder getSortOrder() override;
    bool isColumnHidden(int column) const override;
    bool isViewFrozen() const override;
    int getSortSection() override;
    void initTableView(QAbstractItemModel& tvModel) override;
    void scrollToBottom() override;
    void setClearCbk(std::function<void ()> cb) override;
    void setDockUndockCbk(std::function<void ()> cb) override;
    void setFilterCbk(std::function<void()> cb) override;
    void setModel(QAbstractItemModel* model) override;
    void setSectionClikedCbk(std::function<void (int)> cb) override;
    void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) override;
    void updateScroll() override;

    UIBackendDefault();
    virtual ~UIBackendDefault() = default;

 private:

    Ui::CanRawViewPrivate* ui     = nullptr;
    QWidget*               widget = nullptr;
};

using CanRawViewBackend = UIBackendDefault<CanRawView>;

#endif // CANRAWVIEWBACKEND_H

