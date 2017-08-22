
#ifndef CANRAWVIEWBACKEND_H
#define CANRAWVIEWBACKEND_H

#include "ui_canrawview.h"
#include "canrawview.h"
#include "canrawview_p.h"
#include "uibackend.hpp"

#include <QHeaderView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>

#include <cassert> // assert
#include <functional> // function
#include <memory> // unique_ptr
#include <utility> // move

namespace Ui { class CanRawViewPrivate; }

class CanRawViewBackend : public UIBackend<CanRawView>
{

 public:

    CanRawViewBackend()
        : ui(new Ui::CanRawViewPrivate)
        , widget(new QWidget)
    {
        assert(nullptr != ui);

        ui->setupUi(widget);
    }

    virtual void setClearCbk(std::function<void ()> cb) override
    {
        assert(nullptr != ui);

        QObject::connect(ui->pbClear, &QPushButton::pressed, std::move(cb));
    }

    virtual void setDockUndockCbk(std::function<void ()> cb) override
    {
        assert(nullptr != ui);

        QObject::connect(ui->pbDockUndock, &QPushButton::pressed, std::move(cb));
    }

    virtual void setSectionClikedCbk(std::function<void (int)> cb) override
    {
        assert(nullptr != ui);
        assert(nullptr != ui->tv);
        assert(nullptr != ui->tv->horizontalHeader());

        QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, std::move(cb));
    }

    virtual QWidget* getMainWidget() override
    {
        return widget;
    }

    virtual void initTableView(QAbstractItemModel& tvModel) override
    {
        assert(nullptr != ui);
        assert(nullptr != ui->tv);
        assert(nullptr != ui->tv->horizontalHeader());

        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setColumnHidden(0, true);
        ui->tv->setColumnHidden(1, true);
        ui->tv->setColumnHidden(3, true);
    }

    virtual void updateScroll() override
    {
        assert(nullptr != ui);

        if (false == ui->freezeBox->isChecked())
        {
            assert(nullptr != ui->tv);

            ui->tv->scrollToBottom();
        }
        /* else: noop*/
    }

    virtual int getSortOrder() override
    {
        assert(nullptr != ui);
        assert(nullptr != ui->tv);

        return ui->tv->horizontalHeader()->sortIndicatorOrder();
    }

    virtual QString getClickedColumn(int ndx) override
    {
        assert(nullptr != ui);
        assert(nullptr != ui->tv);
        assert(nullptr != ui->tv->model());

        return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) override
    {
        assert(nullptr != ui);
        assert(nullptr != ui->tv);
        assert(nullptr != ui->tv->horizontalHeader());

        ui->tv->sortByColumn(sortNdx, order);
        ui->tv->horizontalHeader()->setSortIndicator(clickedNdx, order);
    }

 private:

    Ui::CanRawViewPrivate* ui;
    QWidget* widget;
};

#endif // CANRAWVIEWBACKEND_H

