#ifndef CANSIGNALDATAGUIIMPL_H
#define CANSIGNALDATAGUIIMPL_H

#include <QWidget>
#include "cansignaldataguiint.h"
#include "ui_cansignaldata.h"
#include "searchmodel.h"

struct CanSignalDataGuiImpl : public CanSignalDataGuiInt {
    CanSignalDataGuiImpl()
        : _ui(new Ui::CanSignalDataPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
        _widget->setMinimumSize(_ui->tv->minimumSize());
    }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(_ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    virtual void setSettingsCbk(const settings_t& cb) override 
    {
        QObject::connect(_ui->pbSettings, &QPushButton::toggled, cb);
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void initSettings(QAbstractItemModel& tvModel) override
    {
        if(_settingsState.size() > 0) {
            _ui->tv->horizontalHeader()->restoreState(_settingsState);
        }
        
        _ui->tv->setModel(&tvModel);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);
        _ui->tv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        _ui->tv->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
        _ui->tv->setColumnHidden(0, false);

        _settingsState = _ui->tv->horizontalHeader()->saveState();
    }

    virtual void initTableView(QAbstractItemModel& tvModel) override
    {
        if(_tableState.size() > 0) {
            _ui->tv->horizontalHeader()->restoreState(_tableState);
        }

        _ui->tv->setModel(&tvModel);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);
        _ui->tv->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        _ui->tv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        _ui->tv->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        _ui->tv->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        _ui->tv->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        _ui->tv->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

        _tableState = _ui->tv->horizontalHeader()->saveState();

        //tvModel.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // rowID
        //tvModel.setHeaderData(1, Qt::Horizontal, QVariant::fromValue(CRV_ColType::double_type), Qt::UserRole); // time
        //tvModel.setHeaderData(2, Qt::Horizontal, QVariant::fromValue(CRV_ColType::hex_type), Qt::UserRole); // frame ID
        //tvModel.setHeaderData(3, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // direction
        //tvModel.setHeaderData(4, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // length
        //tvModel.setHeaderData(5, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // data
    }

    void initSearch(SearchModel& model) override
    {
        QObject::connect(_ui->searchLine, &QLineEdit::textChanged, &model, &SearchModel::updateFilter);
    }


private:
    Ui::CanSignalDataPrivate* _ui;
    QWidget* _widget;
    QByteArray _settingsState;
    QByteArray _tableState;
};

#endif // CANSIGNALDATAGUIIMPL_H
