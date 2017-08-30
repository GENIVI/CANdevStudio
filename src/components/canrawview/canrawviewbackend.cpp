
#include "canrawview.h"
#include "canrawview_p.h"
#include "canrawviewbackend.hpp"
#include "ui_canrawview.h"
#include "uibackend.hpp"

#include <QHeaderView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>

#include <cassert> // assert
#include <functional> // function
#include <memory> // unique_ptr
#include <utility> // move


CanRawViewBackend::CanRawViewBackend()
    : ui(new Ui::CanRawViewPrivate)
    , widget(new QWidget)
{
    assert(nullptr != ui);

    ui->setupUi(widget);
}

void CanRawViewBackend::setClearCbk(std::function<void ()> cb)
{
    assert(nullptr != ui);

    QObject::connect(ui->pbClear, &QPushButton::pressed, std::move(cb));
}

void CanRawViewBackend::setDockUndockCbk(std::function<void ()> cb)
{
    assert(nullptr != ui);

    QObject::connect(ui->pbDockUndock, &QPushButton::pressed, std::move(cb));
}

void CanRawViewBackend::setSectionClikedCbk(std::function<void (int)> cb)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, std::move(cb));
}

QWidget* CanRawViewBackend::getMainWidget()
{
    return widget;
}

void CanRawViewBackend::initTableView(QAbstractItemModel& tvModel)
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

void CanRawViewBackend::updateScroll()
{
    assert(nullptr != ui);

    if (false == ui->freezeBox->isChecked())
    {
        assert(nullptr != ui->tv);

        ui->tv->scrollToBottom();
    }
    /* else: noop*/
}

int CanRawViewBackend::getSortOrder()
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    return ui->tv->horizontalHeader()->sortIndicatorOrder();
}

QString CanRawViewBackend::getClickedColumn(int ndx)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->model());

    return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
}

void CanRawViewBackend::setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    ui->tv->sortByColumn(sortNdx, order);
    ui->tv->horizontalHeader()->setSortIndicator(clickedNdx, order);
}

bool CanRawViewBackend::isColumnHidden(int column)
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);

    return ui->tv->isColumnHidden(column);
}

bool CanRawViewBackend::isFrozen()
{
    assert(nullptr != ui);

    return ui->freezeBox->isChecked();
}

int CanRawViewBackend::getSortIndicator()
{
    assert(nullptr != ui);
    assert(nullptr != ui->tv);
    assert(nullptr != ui->tv->horizontalHeader());

    return ui->tv->horizontalHeader()->sortIndicatorSection();
}

