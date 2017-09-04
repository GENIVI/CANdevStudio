
#include "canrawview.h"
#include "canrawview_p.h"
#include "canrawviewbackend.hpp"
#include "ui_canrawview.h"
#include "uibackendiface.h"

#include <QHeaderView>
#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

#include <cassert> // assert
#include <functional> // function
#include <memory> // unique_ptr
#include <utility> // move



UIBackendDefault<CanRawView>::UIBackendDefault()
    : ui(new Ui::CanRawViewPrivate)
    , widget(new QWidget)
{
    assert(nullptr != ui);

    ui->setupUi(widget);
}

void UIBackendDefault<CanRawView>::setClearCbk(std::function<void ()> cb)
{
    assert(nullptr != ui);

    QObject::connect(ui->pbClear, &QPushButton::pressed, std::move(cb));
}

void UIBackendDefault<CanRawView>::setDockUndockCbk(std::function<void ()> cb)
{
    assert(nullptr != ui);

    QObject::connect(ui->pbDockUndock, &QPushButton::toggled, std::move(cb));
}

void UIBackendDefault<CanRawView>::setSectionClikedCbk(std::function<void (int)> cb)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, std::move(cb));
}

QWidget* UIBackendDefault<CanRawView>::getMainWidget()
{
    return widget;
}

void UIBackendDefault<CanRawView>::initTableView(QAbstractItemModel& tvModel)
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

void UIBackendDefault<CanRawView>::updateScroll()
{
    assert(nullptr != ui);

    if (false == ui->freezeBox->isChecked())
    {
        assert(nullptr != ui->tv);

        ui->tv->scrollToBottom();
    }
    /* else: noop*/
}

Qt::SortOrder UIBackendDefault<CanRawView>::getSortOrder()
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    return ui->tv->horizontalHeader()->sortIndicatorOrder();
}

QString UIBackendDefault<CanRawView>::getClickedColumn(int ndx)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->model());

    return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
}

void UIBackendDefault<CanRawView>::setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    ui->tv->sortByColumn(sortNdx, order);
    ui->tv->horizontalHeader()->setSortIndicator(clickedNdx, order);
}

bool UIBackendDefault<CanRawView>::isColumnHidden(int column) const
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    return ui->tv->isColumnHidden(column);
}

bool UIBackendDefault<CanRawView>::isViewFrozen() const
{
    assert(nullptr != ui);

    return ui->freezeBox->isChecked();
}

int UIBackendDefault<CanRawView>::getSortSection()
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    return ui->tv->horizontalHeader()->sortIndicatorSection();
}

void UIBackendDefault<CanRawView>::scrollToBottom()
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    ui->tv->scrollToBottom();
}

QString UIBackendDefault<CanRawView>::getWindowTitle()
{
    assert(nullptr != widget);

    return widget->windowTitle();
}

void UIBackendDefault<CanRawView>::setModel(QAbstractItemModel* model)
{
    assert(nullptr != model);
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    ui->tv->setModel(model);
}

void UIBackendDefault<CanRawView>::setFilterCbk(std::function<void()> cb)
{
    assert(nullptr != ui);

    QObject::connect(ui->pbToggleFilter, &QPushButton::toggled, std::move(cb));
}

