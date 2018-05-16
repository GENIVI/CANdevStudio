#ifndef CANRAWFILTERGUIIMPL_H
#define CANRAWFILTERGUIIMPL_H

#include "canrawfilterguiint.h"
#include "ui_canrawfilter.h"
#include <QComboBox>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QtGui/QStandardItemModel>
#include <log.h>

class PolicyCB : public QComboBox {
public:
    PolicyCB(QWidget* parent = nullptr)
        : QComboBox(parent)
    {
        setProperty("type", "nlmItem");

        addItem("ACCEPT");
        addItem("DROP");
    }
};

struct CanRawFilterGuiImpl : public CanRawFilterGuiInt {
    CanRawFilterGuiImpl()
        : _ui(new Ui::CanRawFilterPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);

        initTv();

        QObject::connect(_ui->pbAdd, &QPushButton::pressed, [this] {
            if (_ui->rxTv->hasFocus()) {
                addRow(_rxModel, "RX");
                rxListUpdated();
            } else if (_ui->txTv->hasFocus()) {
                addRow(_txModel, "TX");
                txListUpdated();
            } else {
                cds_info("Neither TX nor RX has focus");
            }
        });

        QObject::connect(_ui->pbRemove, &QPushButton::pressed, [this] {
            if (_ui->rxTv->hasFocus()) {
                _rxModel.removeRow(_ui->rxTv->currentIndex().row());
                rxListUpdated();
            } else if (_ui->txTv->hasFocus()) {
                _txModel.removeRow(_ui->txTv->currentIndex().row());
                txListUpdated();
            } else {
                cds_info("Neither TX nor RX has focus");
            }
        });
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

    virtual void setTxListCbk(const ListUpdated_t& cb)
    {
        _txListUpdatedCbk = cb;
    }

    virtual void setRxListCbk(const ListUpdated_t& cb)
    {
        _rxListUpdatedCbk = cb;
    }

private:
    AcceptList_t getAcceptList(const QStandardItemModel& model)
    {
        AcceptList_t list;

        for (int i = 0; i < model.rowCount(); ++i) {
            QString id = model.item(i, 0)->data(Qt::DisplayRole).toString();
            QString payload = model.item(i, 1)->data(Qt::DisplayRole).toString();
            bool policy = model.item(i, 3)->data(Qt::DisplayRole).toString() == "ACCEPT";

            list.push_back({ id, payload, policy });
        }

        return list;
    }

    void rxListUpdated()
    {
        AcceptList_t list = getAcceptList(_rxModel);

        if (_rxListUpdatedCbk) {
            _rxListUpdatedCbk(list);
        } else {
            cds_warn("_rxListUdpatedCbk not defined");
        }
    }

    void txListUpdated()
    {
        AcceptList_t list = getAcceptList(_txModel);

        if (_txListUpdatedCbk) {
            _txListUpdatedCbk(list);
        } else {
            cds_warn("_txListUdpatedCbk not defined");
        }
    }

    template <typename F>
    void setRxDelegate(QTableView* tv, int col, QStyledItemDelegate& del, const std::function<void()>& cb)
    {
        QItemEditorFactory* factory = new QItemEditorFactory;
        QItemEditorCreatorBase* editor = new QStandardItemEditorCreator<F>();
        factory->registerEditor(QVariant::String, editor);
        del.setItemEditorFactory(factory);
        tv->setItemDelegateForColumn(col, &del);
        QObject::connect(&del, &QAbstractItemDelegate::closeEditor, cb);
    }

    void addRow(QStandardItemModel& model, const QString& dir)
    {
        QList<QStandardItem*> list;
        list.append(new QStandardItem(".*"));
        list.append(new QStandardItem(".*"));
        QStandardItem* item = new QStandardItem(dir);
        item->setEditable(false);
        list.append(item);
        list.append(new QStandardItem("ACCEPT"));
        model.insertRow(0, list);
    }

    void initTv()
    {
        setRxDelegate<QLineEdit>(_ui->rxTv, 0, _rxIdDelegate, std::bind(&CanRawFilterGuiImpl::rxListUpdated, this));
        setRxDelegate<QLineEdit>(
            _ui->rxTv, 1, _rxPayloadDelegate, std::bind(&CanRawFilterGuiImpl::rxListUpdated, this));
        setRxDelegate<PolicyCB>(_ui->rxTv, 3, _rxPolicyDelegate, std::bind(&CanRawFilterGuiImpl::rxListUpdated, this));

        _ui->rxTv->setModel(&_rxModel);
        _rxModel.setHorizontalHeaderLabels(_tabList);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        addRow(_rxModel, "RX");

        setRxDelegate<QLineEdit>(_ui->txTv, 0, _txIdDelegate, std::bind(&CanRawFilterGuiImpl::txListUpdated, this));
        setRxDelegate<QLineEdit>(
            _ui->txTv, 1, _txPayloadDelegate, std::bind(&CanRawFilterGuiImpl::txListUpdated, this));
        setRxDelegate<PolicyCB>(_ui->txTv, 3, _txPolicyDelegate, std::bind(&CanRawFilterGuiImpl::txListUpdated, this));

        _ui->txTv->setModel(&_txModel);
        _txModel.setHorizontalHeaderLabels(_tabList);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        addRow(_txModel, "TX");
    }

private:
    QStandardItemModel _rxModel;
    QStandardItemModel _txModel;
    const QStringList _tabList = { "id", "payload", "dir", "policy" };
    Ui::CanRawFilterPrivate* _ui;
    QWidget* _widget;
    QStyledItemDelegate _rxIdDelegate;
    QStyledItemDelegate _rxPayloadDelegate;
    QStyledItemDelegate _rxPolicyDelegate;
    QStyledItemDelegate _txIdDelegate;
    QStyledItemDelegate _txPayloadDelegate;
    QStyledItemDelegate _txPolicyDelegate;
    ListUpdated_t _rxListUpdatedCbk;
    ListUpdated_t _txListUpdatedCbk;
};

#endif // CANRAWFILTERGUIIMPL_H
