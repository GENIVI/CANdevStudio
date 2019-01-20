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
        using namespace std::placeholders;

        _ui->setupUi(_widget);
        initTv();

        QObject::connect(_ui->pbAdd, &QPushButton::pressed,
            [this] { handleListOperation(std::bind(&CanRawFilterGuiImpl::handleListAdd, this, _1)); });

        QObject::connect(_ui->pbRemove, &QPushButton::pressed,
            [this] { handleListOperation(std::bind(&CanRawFilterGuiImpl::handleListRemove, this, _1)); });

        QObject::connect(_ui->pbUp, &QPushButton::pressed,
            [this] { handleListOperation(std::bind(&CanRawFilterGuiImpl::handleListUp, this, _1)); });

        QObject::connect(_ui->pbDown, &QPushButton::pressed,
            [this] { handleListOperation(std::bind(&CanRawFilterGuiImpl::handleListDown, this, _1)); });

        QObject::connect(_ui->rxPolicy, &QComboBox::currentTextChanged, [this] { listUpdatedRx(); });

        QObject::connect(_ui->txPolicy, &QComboBox::currentTextChanged, [this] { listUpdatedTx(); });
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

    virtual void setTxListCbk(const ListUpdated_t& cb)
    {
        _txListUpdatedCbk = cb;

        listUpdatedTx();
    }

    virtual void setRxListCbk(const ListUpdated_t& cb)
    {
        _rxListUpdatedCbk = cb;

        listUpdatedRx();
    }

    virtual void setListRx(const AcceptList_t& list)
    {
        setList(list, _rxModel, "RX");
        listUpdatedRx();
    }

    virtual void setListTx(const AcceptList_t& list)
    {
        setList(list, _txModel, "TX");
        listUpdatedTx();
    }

private:
    void setList(const AcceptList_t& list, QStandardItemModel& model, const QString& dir)
    {
        model.removeRows(0, model.rowCount());

        if (list.size() >= 1) {
            // Last element used as default policy
            for (AcceptList_t::size_type i = 0; i < list.size() - 1; ++i) {
                const auto& item = list[i];
                model.appendRow(prepareRow(std::get<0>(item), std::get<1>(item), dir, std::get<2>(item)));
            }

            if (dir == "RX") {
                const auto& item = list[list.size() - 1];
                _ui->rxPolicy->setCurrentText(std::get<2>(item) ? "ACCEPT" : "DROP");
            } else if (dir == "TX") {
                const auto& item = list[list.size() - 1];
                _ui->txPolicy->setCurrentText(std::get<2>(item) ? "ACCEPT" : "DROP");
            } else {
                cds_warn("Wrong direction ({})", dir.toStdString());
            }

        } else {
            cds_warn("{} list expected to have at least 1 element", dir.toStdString());
        }
    }

    void handleListOperation(std::function<void(QTableView* tv)> func)
    {
        if (_ui->rxTv->hasFocus()) {
            func(_ui->rxTv);
            listUpdatedRx();
        } else if (_ui->txTv->hasFocus()) {
            func(_ui->txTv);
            listUpdatedTx();
        } else {
            cds_info("Neither TX nor RX has focus");
        }
    }

    QStandardItemModel& getItemModel(QTableView* tv)
    {
        if (tv == _ui->txTv) {
            return _txModel;
        } else {
            return _rxModel;
        }
    }

    void handleListDown(QTableView* tv)
    {
        auto& model = getItemModel(tv);
        int currRow = tv->currentIndex().row();
        int currCol = tv->currentIndex().column();

        // Do not move the last row
        if (currRow < model.rowCount() - 2) {
            const auto& row = model.takeRow(currRow);
            model.insertRow(currRow + 1, row);
            tv->setCurrentIndex(model.index(currRow + 1, currCol));
        }
    }

    void handleListUp(QTableView* tv)
    {
        auto& model = getItemModel(tv);
        int currRow = tv->currentIndex().row();
        int currCol = tv->currentIndex().column();

        // Do not move the last row
        if ((currRow > 0) && (currRow < model.rowCount() - 1)) {
            const auto& row = model.takeRow(currRow);
            model.insertRow(currRow - 1, row);
            tv->setCurrentIndex(model.index(currRow - 1, currCol));
        }
    }

    void handleListRemove(QTableView* tv)
    {
        auto& model = getItemModel(tv);
        model.removeRow(tv->currentIndex().row());
    }

    void handleListAdd(QTableView* tv)
    {
        auto& model = getItemModel(tv);

        QString dir = "RX";
        if (tv == _ui->txTv) {
            dir = "TX";
        }

        auto&& list = prepareRow(".*", ".*", dir, true);
        model.insertRow(0, list);
    }

    QList<QStandardItem*> prepareRow(const QString& id, const QString& payload, const QString& dir, bool policy)
    {
        QList<QStandardItem*> list;
        list.append(new QStandardItem(id));
        list.append(new QStandardItem(payload));
        QStandardItem* item = new QStandardItem(dir);
        item->setEditable(false);
        list.append(item);
        list.append(new QStandardItem(policy ? "ACCEPT" : "DROP"));

        return list;
    }

    AcceptList_t getAcceptList(const QStandardItemModel& model, bool policy)
    {
        AcceptList_t list;

        for (int i = 0; i < model.rowCount(); ++i) {
            QString id = model.item(i, 0)->data(Qt::DisplayRole).toString();
            QString payload = model.item(i, 1)->data(Qt::DisplayRole).toString();
            bool policy = model.item(i, 3)->data(Qt::DisplayRole).toString() == "ACCEPT";

            list.push_back(CanRawFilterGuiInt::AcceptListItem_t(id, payload, policy));
        }

        list.push_back(CanRawFilterGuiInt::AcceptListItem_t(".*", ".*", policy));

        return list;
    }

    void listUpdated(const QStandardItemModel& model, const ListUpdated_t& cb, bool policy)
    {
        AcceptList_t list = getAcceptList(model, policy);

        if (cb) {
            cb(list);
        } else {
            cds_warn("List callback not defined");
        }
    }

    void listUpdatedRx()
    {
        listUpdated(_rxModel, _rxListUpdatedCbk, _ui->rxPolicy->currentText() == "ACCEPT");
    }

    void listUpdatedTx()
    {
        listUpdated(_txModel, _txListUpdatedCbk, _ui->txPolicy->currentText() == "ACCEPT");
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

    void initTv()
    {
        setRxDelegate<QLineEdit>(_ui->rxTv, 0, _rxIdDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedRx, this));
        setRxDelegate<QLineEdit>(
            _ui->rxTv, 1, _rxPayloadDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedRx, this));
        setRxDelegate<PolicyCB>(_ui->rxTv, 3, _rxPolicyDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedRx, this));

        _ui->rxTv->setModel(&_rxModel);
        _rxModel.setHorizontalHeaderLabels(_tabList);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

        setRxDelegate<QLineEdit>(_ui->txTv, 0, _txIdDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedTx, this));
        setRxDelegate<QLineEdit>(
            _ui->txTv, 1, _txPayloadDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedTx, this));
        setRxDelegate<PolicyCB>(_ui->txTv, 3, _txPolicyDelegate, std::bind(&CanRawFilterGuiImpl::listUpdatedTx, this));

        _ui->txTv->setModel(&_txModel);
        _txModel.setHorizontalHeaderLabels(_tabList);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
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
