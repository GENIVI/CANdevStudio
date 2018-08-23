#ifndef CANSIGNALDATAGUIIMPL_H
#define CANSIGNALDATAGUIIMPL_H

#include <QWidget>
#include "cansignaldataguiint.h"
#include "ui_cansignaldata.h"
#include "searchmodel.h"
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QStyledItemDelegate>
#include <log.h>

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

    virtual void setSettingsUpdatedCbk(const settingsUpdated_t& cb) override 
    {
        _settingsUpdatedCbk = cb;
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    template <typename F>
    void setDelegate(QTableView* tv, int col, QStyledItemDelegate& del, const std::function<void()>& cb)
    {
        QItemEditorFactory* factory = new QItemEditorFactory;
        QItemEditorCreatorBase* editor = new QStandardItemEditorCreator<F>();
        factory->registerEditor(QVariant::String, editor);
        del.setItemEditorFactory(factory);
        tv->setItemDelegateForColumn(col, &del);
        QObject::connect(&del, &QAbstractItemDelegate::closeEditor, cb);
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

        setDelegate<QLineEdit>(_ui->tv, 4, _cycleDelegate, std::bind(&CanSignalDataGuiImpl::settingsUpdated, this));
        setDelegate<QLineEdit>(_ui->tv, 5, _initValDelegate, std::bind(&CanSignalDataGuiImpl::settingsUpdated, this));
    }

    void settingsUpdated()
    {
        if(_settingsUpdatedCbk) {
            _settingsUpdatedCbk();
        }
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
    QStyledItemDelegate _cycleDelegate;
    QStyledItemDelegate _initValDelegate;
    settingsUpdated_t _settingsUpdatedCbk{ nullptr };
};

#endif // CANSIGNALDATAGUIIMPL_H
