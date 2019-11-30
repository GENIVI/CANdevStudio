#ifndef CANSIGNALSENDERGUIIMPL_H
#define CANSIGNALSENDERGUIIMPL_H

#include "cansignalsenderguiint.h"
#include "ui_cansignalsender.h"
#include <QComboBox>
#include <QJsonObject>
#include <QLineEdit>
#include <QMouseEvent>
#include <QRegExpValidator>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QWidget>
#include <candbhandler.h>
#include <log.h>

class SigIdDelegate : public QStyledItemDelegate {
public:
    SigIdDelegate(QAbstractItemModel* model, const CanDbHandler& db, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
        , _db(db)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        QComboBox* cb = new QComboBox(parent);

        cb->setProperty("type", "listItem");

        for (const auto& msg : _db.getDb()) {
            if (msg.first.id > 0x7ff) {
                cb->addItem(fmt::format("0x{:08x}", msg.first.id).c_str());
            } else {
                cb->addItem(fmt::format("0x{:03x}", msg.first.id).c_str());
            }
        }

        cb->setCurrentText(_model->data(index).toString());

        connect(cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated),
            [cb](const QString&) { cb->clearFocus(); });

        // QOverload is not supported by MSVC 2015
        connect(cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged),
            [index, this](const QString& text) {
                uint32_t id = text.toUInt(nullptr, 16);

                if (_db.getDb().count(id)) {
                    auto ndx = _model->index(index.row(), 1);
                    _model->setData(ndx, _db.getDb().at(id).front().signal_name.c_str(), Qt::EditRole);
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
    const CanDbHandler& _db;
};

class SigNameDelegate : public QStyledItemDelegate {
public:
    SigNameDelegate(QAbstractItemModel* model, const CanDbHandler& db, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
        , _db(db)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        QComboBox* cb = new QComboBox(parent);
        QString idStr = _model->data(_model->index(index.row(), 0)).toString();
        uint32_t id = idStr.toUInt(nullptr, 16);

        cb->setProperty("type", "listItem");

        if (_db.getDb().count(id)) {
            for (auto&& sig : _db.getDb().at(id)) {
                cb->addItem(sig.signal_name.c_str());
            }
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
    const CanDbHandler& _db;
};

class SigValDelegate : public QStyledItemDelegate {
public:
    SigValDelegate(QAbstractItemModel* model, const CanDbHandler& db, QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
        , _model(model)
        , _db(db)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        QString idStr = _model->data(_model->index(index.row(), 0)).toString();
        uint32_t id = idStr.toUInt(nullptr, 16);
        QString sigName = _model->data(_model->index(index.row(), 1)).toString();

        QLineEdit* le = new QLineEdit(parent);
        le->setProperty("type", "nlmItem");
        QRegExp qRegExp("-?([0-9]+[.])?[0-9]+");
        auto v = new QRegExpValidator(qRegExp, le);
        float min = 0, max = 0;
        le->setValidator(v);

        if (_db.getDb().count(id)) {
            for (auto&& sig : _db.getDb().at(id)) {
                if (QString(sig.signal_name.c_str()) == sigName) {
                    min = sig.min;
                    max = sig.max;
                }
            }
        } else if (idStr.length() > 0) {
            cds_error("No signals for selected id 0x{:03x}", id);
        }

        le->setPlaceholderText(QString("min: %1, max: %2").arg(min).arg(max));

        return le;
    }

private:
    QAbstractItemModel* _model;
    const CanDbHandler& _db;
};

struct CanSignalSenderGuiImpl : public CanSignalSenderGuiInt {
    CanSignalSenderGuiImpl()
        : _ui(new Ui::CanSignalSenderPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
        _ui->tv->setSelectionMode(QAbstractItemView::ExtendedSelection);
        _ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);

        // QOverload is not supported by MSVC 2015
        QObject::connect(_ui->tv, &QAbstractItemView::clicked, _ui->tv,
            static_cast<void (QAbstractItemView::*)(const QModelIndex&)>(&QAbstractItemView::edit));
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void initTv(QStandardItemModel& tvModel, const CanDbHandler* db) override
    {
        _model = &tvModel;
        _ui->tv->setModel((QAbstractItemModel*)&tvModel);
        _ui->tv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);
        _ui->tv->horizontalHeader()->setHighlightSections(false);

        _db = db;

        _idDelegate = new SigIdDelegate(&tvModel, *_db, _ui->tv);
        _ui->tv->setItemDelegateForColumn(0, _idDelegate);

        _nameDelegate = new SigNameDelegate(&tvModel, *_db, _ui->tv);
        _ui->tv->setItemDelegateForColumn(1, _nameDelegate);

        _valDelegate = new SigValDelegate(&tvModel, *_db, _ui->tv);
        _ui->tv->setItemDelegateForColumn(2, _valDelegate);
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
        } else if (_db->getDb().size() > 0) {
            uint32_t tmpId = _db->getDb().begin()->first.id;

            if (tmpId > 0x7ff) {
                idItem = new QStandardItem(fmt::format("0x{:08x}", tmpId).c_str());
            } else {
                idItem = new QStandardItem(fmt::format("0x{:03x}", tmpId).c_str());
            }

            if (_db->getDb().at(tmpId).size() > 0) {
                sigItem = new QStandardItem(_db->getDb().at(tmpId).front().signal_name.c_str());
            } else {
                cds_warn("Signal list for 0x{:x} is empty", tmpId);

                sigItem = new QStandardItem();
            }
        } else {
            idItem = new QStandardItem();
            sigItem = new QStandardItem();
            valItem = new QStandardItem();
        }

        QList<QStandardItem*> list{ idItem, sigItem, valItem };

        _model->appendRow(list);

        QPushButton* bt = new QPushButton("Send");
        bt->setProperty("type", "nlmItem");
        bt->setFlat(true);

        uint32_t row = _model->rowCount() - 1;
        QObject::connect(bt, &QPushButton::pressed, [this, row] {
            if (_sendCbk) {
                auto id = _model->data(_model->index(row, 0)).toString();
                auto sig = _model->data(_model->index(row, 1)).toString();
                auto val = _model->data(_model->index(row, 2)).toString();

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
            auto id = _model->data(_model->index(i, 0));
            auto sig = _model->data(_model->index(i, 1));
            auto val = _model->data(_model->index(i, 2));

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
    const CanDbHandler* _db;
    Ui::CanSignalSenderPrivate* _ui;
    QWidget* _widget;
    SigIdDelegate* _idDelegate;
    SigNameDelegate* _nameDelegate;
    SigValDelegate* _valDelegate;
};

#endif // CANSIGNALSENDERGUIIMPL_H
