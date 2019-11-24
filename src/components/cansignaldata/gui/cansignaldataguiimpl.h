#ifndef CANSIGNALDATAGUIIMPL_H
#define CANSIGNALDATAGUIIMPL_H

#include "cansignaldataguiint.h"
#include "searchmodel.h"
#include "ui_cansignaldata.h"
#include <QButtonGroup>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QStyledItemDelegate>
#include <QWidget>
#include <log.h>

class IntervalLE : public QLineEdit {
public:
    IntervalLE(uint32_t, QWidget* parent)
        : QLineEdit(parent)
    {
        QRegExp qRegExp("[0-9]*");
        auto v = new QRegExpValidator(qRegExp, this);
        setValidator(v);
    }
};

class ValueLE : public QLineEdit {
public:
    ValueLE(uint32_t len, QWidget* parent)
        : QLineEdit(parent)
    {
        QRegExp qRegExp("[0-9A-Fa-f]{" + QString::number(len * 2) + "}");
        auto v = new QRegExpValidator(qRegExp, this);
        setValidator(v);
        setPlaceholderText(QString(" %1 hex digits expected").arg(len*2));
    }
};

template <typename T> class DbFieldDelegate : public QStyledItemDelegate {
public:
    DbFieldDelegate(QAbstractItemModel* model, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        uint32_t dlc = _model->data(_model->index(index.row(), 2)).toUInt();

        return new T(dlc, parent);
    }

private:
    QAbstractItemModel* _model;
};

struct CanSignalDataGuiImpl : public CanSignalDataGuiInt {
    CanSignalDataGuiImpl()
        : _ui(new Ui::CanSignalDataPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
        _widget->setMinimumSize(_ui->tv->minimumSize());

        QButtonGroup* bg = new QButtonGroup(_widget);
        bg->addButton(_ui->pbMsg);
        bg->addButton(_ui->pbSig);

        _ui->tv->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(_ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    virtual void setMsgViewCbk(const msgView_t& cb) override
    {
        QObject::connect(_ui->pbMsg, &QPushButton::toggled, cb);
    }

    virtual void setMsgSettingsUpdatedCbk(const msgSettingsUpdated_t& cb) override
    {
        _msgSettingsUpdatedCbk = cb;
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void setMsgView(QAbstractItemModel& tvModel) override
    {
        if (_settingsState.size() > 0) {
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

        _cycleDelegate = new DbFieldDelegate<IntervalLE>(&tvModel, _ui->tv);
        _ui->tv->setItemDelegateForColumn(4, _cycleDelegate);
        QObject::connect(_cycleDelegate, &QAbstractItemDelegate::closeEditor,
            std::bind(&CanSignalDataGuiImpl::msgSettingUpdated, this));

        _initValDelegate = new DbFieldDelegate<ValueLE>(&tvModel, _ui->tv);
        _ui->tv->setItemDelegateForColumn(5, _initValDelegate);
        QObject::connect(_initValDelegate, &QAbstractItemDelegate::closeEditor,
            std::bind(&CanSignalDataGuiImpl::msgSettingUpdated, this));
    }

    void msgSettingUpdated()
    {
        if (_msgSettingsUpdatedCbk) {
            _msgSettingsUpdatedCbk();
        }
    }

    virtual void setSigView(QAbstractItemModel& tvModel) override
    {
        if (_tableState.size() > 0) {
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
    DbFieldDelegate<IntervalLE>* _cycleDelegate;
    DbFieldDelegate<ValueLE>* _initValDelegate;
    msgSettingsUpdated_t _msgSettingsUpdatedCbk{ nullptr };
};

#endif // CANSIGNALDATAGUIIMPL_H
