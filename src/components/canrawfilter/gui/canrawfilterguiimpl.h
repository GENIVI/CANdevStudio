#ifndef CANRAWFILTERGUIIMPL_H
#define CANRAWFILTERGUIIMPL_H

#include "canrawfilterguiint.h"
#include "ui_canrawfilter.h"
#include <QWidget>
#include <QtGui/QStandardItemModel>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <log.h>
#include <QComboBox>


class PolicyCB : public QComboBox
{
public:
    PolicyCB(QWidget *parent = nullptr) :
        QComboBox(parent)
    {
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
                cds_info("RX focus");
            } else if (_ui->txTv->hasFocus()) {
                cds_info("TX focus");
            } else {
                cds_info("Nie ma focusa");
            }
        });
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

    virtual void setTxListCbk(const listUpdated_t& cb)
    {
    }

    virtual void setRxListCbk(const listUpdated_t& cb)
    {
    }

private:
    void initTv()
    {
        QItemEditorFactory *factory = new QItemEditorFactory;

        QItemEditorCreatorBase *editor = new QStandardItemEditorCreator<PolicyCB>();
        factory->registerEditor(QVariant::String, editor);
        _delegate.setItemEditorFactory(factory);
        _ui->rxTv->setItemDelegateForColumn(3, &_delegate);
        QObject::connect(&_delegate, &QItemDelegate::closeEditor, [] {
                    cds_info("edit completed");
                });

        _ui->rxTv->setModel(&_rxModel);

        _rxModel.setHorizontalHeaderLabels(_tabList);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->rxTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

        QList<QStandardItem*> list;
        QStandardItem *item;

        item = new QStandardItem(".*");
        list.append(item);
        item = new QStandardItem(".*");
        list.append(item);
        item = new QStandardItem("RX");
        list.append(item);
        item = new QStandardItem("ACCEPT");
        list.append(item);
        _rxModel.appendRow(list);

        _ui->txTv->setModel(&_txModel);
        _txModel.setHorizontalHeaderLabels(_tabList);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->txTv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

        list.clear();
        list.append(new QStandardItem(".*"));
        list.append(new QStandardItem(".*"));
        list.append(new QStandardItem("TX"));
        list.append(new QStandardItem("ACCEPT"));
        _txModel.appendRow(list);
    }

private:
    QStandardItemModel _rxModel;
    QStandardItemModel _txModel;
    const QStringList _tabList = { "id", "payload", "dir", "policy" };
    Ui::CanRawFilterPrivate* _ui;
    QWidget* _widget;
    QItemDelegate _delegate;
};

#endif // CANRAWFILTERGUIIMPL_H
