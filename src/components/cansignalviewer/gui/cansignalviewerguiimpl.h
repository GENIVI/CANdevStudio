#ifndef CANSINGALVIEWERGUIIMPL_H
#define CANSINGALVIEWERGUIIMPL_H

//#include "crv_enums.h"
#include "cansignalviewerguiint.h"
#include "ui_cansignalviewer.h"
#include <QStandardItemModel>
#include <QWidget>
#include <log.h>
#include <memory>

struct CanSignalViewerGuiImpl : public CanSignalViewerGuiInt {

    CanSignalViewerGuiImpl()
        : _ui(new Ui::CanSignalViewerPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
        _widget->setMinimumSize(_ui->tv->minimumSize());
    }

    virtual void setClearCbk(const clear_t& cb) override
    {
        QObject::connect(_ui->pbClear, &QPushButton::pressed, cb);
    }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(_ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    virtual void setSectionClikedCbk(const sectionClicked_t& cb) override
    {
        QObject::connect(_ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, cb);
    }

    virtual void setFilterCbk(const filter_t& cb) override
    {
        QObject::connect(_ui->pbToggleFilter, &QPushButton::toggled, cb);
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void setModel(QAbstractItemModel* model) override
    {
        _ui->tv->setModel(model);
    }

    virtual void initTableView(QAbstractItemModel& tvModel) override
    {
        _ui->tv->setModel(&tvModel);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);
        _ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        _ui->tv->setColumnHidden(0, true);
        _ui->tv->setColumnWidth(3, 85);
        _ui->tv->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        _ui->tv->setColumnWidth(5, 200);
    }

    virtual bool isViewFrozen() override
    {
        return _ui->freezeBox->isChecked();
    }

    virtual void setViewFrozen(bool state) override
    {
        if (_ui->freezeBox->isChecked() != state) {
            _ui->freezeBox->setChecked(state);
        }
    }

    virtual void scrollToBottom() override
    {
        _ui->tv->scrollToBottom();
    }

    virtual Qt::SortOrder getSortOrder() override
    {
        return _ui->tv->horizontalHeader()->sortIndicatorOrder();
    }

    virtual QString getClickedColumn(int ndx) override
    {
        return _ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    virtual void setSorting(int sortNdx, Qt::SortOrder order) override
    {
        _ui->tv->sortByColumn(sortNdx, order);
        _ui->tv->horizontalHeader()->setSortIndicator(sortNdx, order);
    }

    virtual QString getWindowTitle() override
    {
        return _widget->windowTitle();
    }

    virtual bool isColumnHidden(int ndx) override
    {
        return _ui->tv->isColumnHidden(ndx);
    }

    virtual void getColumnProper(int ndx, int& vIdx) override
    {
        auto horHeader = _ui->tv->horizontalHeader();
        vIdx = horHeader->visualIndex(ndx);
    }

    virtual void setColumnProper(int vIdxFrom, int vIdxTo) override
    {
        auto horHeader = _ui->tv->horizontalHeader();
        horHeader->moveSection(vIdxFrom, vIdxTo);
    }

private:
    Ui::CanSignalViewerPrivate* _ui;
    QWidget* _widget;
};

#endif // CANSINGALVIEWERGUIIMPL_H
