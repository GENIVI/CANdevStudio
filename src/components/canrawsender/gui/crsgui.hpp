#ifndef CRSGUI_HPP
#define CRSGUI_HPP

#include "../crsguiinterface.hpp"
#include "ui_canrawsender.h"
#include <memory>

namespace Ui {
class CanRawSenderPrivate;
}

struct CRSGui : public CRSGuiInterface {
    CRSGui()
        : ui(new Ui::CanRawSenderPrivate)
        , widget(new QWidget)
    {
        ui->setupUi(widget);
    }

    void setAddCbk(const add_t& cb) override
    {
        QObject::connect(ui->pbAdd, &QPushButton::pressed, cb);
    }

    void setRemoveCbk(const remove_t& cb) override
    {
        QObject::connect(ui->pbRemove, &QPushButton::pressed, cb);
    }

    void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    QWidget* getMainWidget() override
    {
        return widget;
    }

    void initTableView(QAbstractItemModel& tvModel) override
    {

        ui->tv->setModel(&tvModel);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    }

    QModelIndexList getSelectedRows() override
    {
        return ui->tv->selectionModel()->selectedRows();
    }

    void setIndexWidget(const QModelIndex& index, QWidget* widget) override
    {
        ui->tv->setIndexWidget(index, widget);
    }

    std::unique_ptr<NewLineManager> newLine(CanRawSender* q_ptr, bool simulationState) override
    {
        return std::make_unique<NewLineManager>(q_ptr, simulationState);
    }

private:
    Ui::CanRawSenderPrivate* ui;
    QWidget* widget;
};
#endif // CRSGUI_HPP
