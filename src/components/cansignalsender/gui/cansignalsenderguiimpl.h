#ifndef CANSIGNALSENDERGUIIMPL_H
#define CANSIGNALSENDERGUIIMPL_H

#include "cansignalsenderguiint.h"
#include "ui_cansignalsender.h"
#include <QComboBox>
#include <QJsonObject>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QWidget>
#include <cdstableview.h>
#include <log.h>

class SigIdDelegate : public QStyledItemDelegate {
public:
    SigIdDelegate(QAbstractItemModel* model, std::map<uint32_t, QStringList>& sigNames, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
        , _sigNames(sigNames)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        QComboBox* cb = new QComboBox(parent);

        cb->setProperty("type", "listItem");

        for (const auto& msg : _sigNames) {
            if (msg.first > 0x7ff) {
                cb->addItem(fmt::format("0x{:08x}", msg.first).c_str());
            } else {
                cb->addItem(fmt::format("0x{:03x}", msg.first).c_str());
            }
        }

        cb->setCurrentText(_model->data(index).toString());

        connect(cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated),
            [cb](const QString&) { cb->clearFocus(); });

        // QOverload is not supported by MSVC 2015
        connect(cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged),
            [index, this](const QString& text) {
                uint32_t id = text.toUInt(nullptr, 16);

                if (_sigNames.count(id)) {
                    auto ndx = _model->index(index.row(), 2);
                    _model->setData(ndx, _sigNames[id].first(), Qt::EditRole);
                } else {
                    cds_error("No signals for selected id 0x{:03x}", id);
                }
            });

        return cb;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        Q_UNUSED(index);
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        cb->showPopup();
    }

private:
    QAbstractItemModel* _model;
    std::map<uint32_t, QStringList>& _sigNames;
};

class SigNameDelegate : public QStyledItemDelegate {
public:
    SigNameDelegate(QAbstractItemModel* model, std::map<uint32_t, QStringList>& sigNames, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
        , _sigNames(sigNames)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        QComboBox* cb = new QComboBox(parent);
        QString idStr = _model->data(_model->index(index.row(), 1)).toString();
        uint32_t id = idStr.toUInt(nullptr, 16);

        cb->setProperty("type", "listItem");

        if (_sigNames.count(id)) {
            cb->addItems(_sigNames.at(id));
        } else if (idStr.length() > 0) {
            cds_error("No signals for selected id 0x{:03x}", id);
        }

        cb->setCurrentText(_model->data(index).toString());

        connect(cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated),
            [cb](const QString&) { cb->clearFocus(); });

        return cb;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        Q_UNUSED(index);
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        cb->showPopup();
    }

private:
    QAbstractItemModel* _model;
    std::map<uint32_t, QStringList>& _sigNames;
};

struct CanSignalSenderGuiImpl : public CanSignalSenderGuiInt {
    CanSignalSenderGuiImpl()
        : _ui(new Ui::CanSignalSenderPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void initTv(QStandardItemModel& tvModel, std::map<uint32_t, QStringList>& sigNames) override
    {
        _model = &tvModel;
        _ui->tv->setModel((QAbstractItemModel*)&tvModel);
        _ui->tv->hideColumn(0);
        _ui->tv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);

        _sigNames = &sigNames;

        _idDelegate = new SigIdDelegate(&tvModel, *_sigNames, _ui->tv);
        _ui->tv->setItemDelegateForColumn(1, _idDelegate);

        _nameDelegate = new SigNameDelegate(&tvModel, *_sigNames, _ui->tv);
        _ui->tv->setItemDelegateForColumn(2, _nameDelegate);
    }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(_ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    virtual void setAddCbk(const add_t& cb) override
    {
        QObject::connect(_ui->pbAdd, &QPushButton::pressed, cb);
    }

    virtual void setSendCbk(const send_t& cbk) override
    {
        _sendCbk = cbk;
    }

    virtual void setRemoveCbk(const remove_t& cb) override
    {
        QObject::connect(_ui->pbRemove, &QPushButton::pressed, cb);
    }

    virtual void addRow(const QString& id = "", const QString& sig = "", const QString& val = "") override
    {
        if (!_model) {
            cds_error("_model is NULL");
            return;
        }

        QStandardItem* idItem = nullptr;
        QStandardItem* sigItem = nullptr;
        QStandardItem* valItem = nullptr;

        if (id.length() > 0 && sig.length() > 0) {
            idItem = new QStandardItem(id);
            sigItem = new QStandardItem(sig);
            valItem = new QStandardItem(val);
        } else if (_sigNames->size() > 0) {
            uint32_t tmpId = _sigNames->begin()->first;

            if (tmpId > 0x7ff) {
                idItem = new QStandardItem(fmt::format("0x{:08x}", tmpId).c_str());
            } else {
                idItem = new QStandardItem(fmt::format("0x{:03x}", tmpId).c_str());
            }

            if ((*_sigNames)[tmpId].size() > 0) {
                sigItem = new QStandardItem((*_sigNames)[tmpId].constFirst());
            } else {
                cds_warn("Signal list for 0x{:x} is empty", tmpId);

                sigItem = new QStandardItem();
            }
        } else {
            idItem = new QStandardItem();
            sigItem = new QStandardItem();
            valItem = new QStandardItem();
        }

        QList<QStandardItem*> list{ {}, idItem, sigItem, valItem };

        _model->appendRow(list);

        QPushButton* bt = new QPushButton("Send");
        bt->setProperty("type", "nlmItem");
        bt->setFlat(true);

        uint32_t row = _model->rowCount() - 1;
        QObject::connect(bt, &QPushButton::pressed, [this, row] {
            if (_sendCbk) {
                auto id = _model->data(_model->index(row, 1)).toString();
                auto sig = _model->data(_model->index(row, 2)).toString();
                auto val = _model->data(_model->index(row, 3)).toString();

                if (id.length() > 0 && sig.length() > 0 && val.length() > 0) {
                    _sendCbk(id, sig, QVariant(val));
                }
            }
        });

        auto pbNdx = _model->index(_model->rowCount() - 1, _model->columnCount() - 1);
        _ui->tv->setIndexWidget(pbNdx, bt);
    }

    QJsonArray getRows() override
    {
        QJsonArray ret;
        QJsonObject item;

        for (int i = 0; i < _model->rowCount(); ++i) {
            auto id = _model->data(_model->index(i, 1));
            auto sig = _model->data(_model->index(i, 2));
            auto val = _model->data(_model->index(i, 3));

            item["id"] = id.toString();
            item["sig"] = sig.toString();
            item["val"] = val.toString();

            ret.append(item);
        }

        return ret;
    }

    QModelIndexList getSelectedRows() override
    {
        return _ui->tv->selectionModel()->selectedRows();
    }

private:
    send_t _sendCbk{ nullptr };
    QStandardItemModel* _model;
    std::map<uint32_t, QStringList>* _sigNames;
    Ui::CanSignalSenderPrivate* _ui;
    QWidget* _widget;
    SigIdDelegate* _idDelegate;
    SigNameDelegate* _nameDelegate;
};

#endif // CANSIGNALSENDERGUIIMPL_H
