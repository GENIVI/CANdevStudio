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

        for (const auto& msg : _sigNames) {
            if (msg.first > 0x7ff) {
                cb->addItem(fmt::format("0x{:08x}", msg.first).c_str());
            } else {
                cb->addItem(fmt::format("0x{:03x}", msg.first).c_str());
            }
        }

        cb->setCurrentText(_model->data(index).toString());

        connect(cb, QOverload<const QString&>::of(&QComboBox::currentTextChanged), [index, this](const QString& text) {
            uint32_t id = text.toUInt(nullptr, 16);

            if (_sigNames.count(id)) {
                auto ndx = _model->index(index.row(), 1);
                _model->setData(ndx, _sigNames[id].first(), Qt::EditRole);
            } else {
                cds_error("No signals for selected id 0x{:03x}", id);
            }
        });

        return cb;
    }

    void updateEditorGeometry(
        QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);

        QComboBox* cb = static_cast<QComboBox*>(editor);
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
        uint32_t id = _model->data(_model->index(index.row(), 0)).toString().toUInt(nullptr, 16);

        if (_sigNames.count(id)) {
            cb->addItems(_sigNames.at(id));
        } else {
            cds_error("No signals for selected id 0x{:03x}", id);
        }

        cb->setCurrentText(_model->data(index).toString());

        return cb;
    }

    void updateEditorGeometry(
        QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);

        QComboBox* cb = static_cast<QComboBox*>(editor);
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

        _ui->tv->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }

    virtual QWidget* mainWidget() override
    {
        return _widget;
    }

    virtual void initTv(QStandardItemModel& tvModel, std::map<uint32_t, QStringList>& sigNames) override
    {
        _model = &tvModel;
        _ui->tv->setModel((QAbstractItemModel*)&tvModel);
        _ui->tv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        _ui->tv->horizontalHeader()->setSectionsMovable(true);

        _sigNames = &sigNames;

        _idDelegate = new SigIdDelegate(&tvModel, *_sigNames, _ui->tv);
        _ui->tv->setItemDelegateForColumn(0, _idDelegate);

        _nameDelegate = new SigNameDelegate(&tvModel, *_sigNames, _ui->tv);
        _ui->tv->setItemDelegateForColumn(1, _nameDelegate);
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

        // if (id.length() > 0 && sig.length() > 0) {
        //    // Hack to allow set ComboBox before _sigNames gets populated
        //    uint32_t numId = id.toUInt(nullptr, 16);
        //    if (!(*_sigNames)[numId].contains(sig)) {
        //        (*_sigNames)[numId].append(sig);
        //    }
        //}

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

        QList<QStandardItem*> list{ idItem, sigItem, valItem };

        _model->appendRow(list);

        // QPushButton* bt = new QPushButton("Send");
        // bt->setProperty("type", "nlmItem");
        // bt->setFlat(true);

        // QComboBox* sigCmb = new QComboBox();
        // sigCmb->setProperty("type", "nlmItem");

        // QComboBox* idCmb = new QComboBox();
        // idCmb->setProperty("type", "nlmItem");

        // QObject::connect(idCmb, &QComboBox::currentTextChanged, [this, sigCmb](const QString& text) {
        //    uint32_t id = text.toUInt(nullptr, 16);
        //    sigCmb->clear();

        //    if (_sigNames->count(id)) {
        //        sigCmb->addItems(_sigNames->at(id));
        //    } else {
        //        cds_error("No signals for selected id 0x{:03x}", id);
        //    }
        //});

        // for (const auto& msg : *_sigNames) {
        //    idCmb->addItem(fmt::format("0x{:03x}", msg.first).c_str());
        //}

        // QLineEdit* le = new QLineEdit();
        // le->setProperty("type", "nlmItem");

        // QObject::connect(bt, &QPushButton::pressed, [idCmb, sigCmb, le, this] {
        //    if (_sendCbk) {
        //        if (idCmb->currentText().length() > 0 && sigCmb->currentText().length() > 0
        //            && le->text().length() > 0) {

        //            _sendCbk(idCmb->currentText(), sigCmb->currentText(), QVariant(le->text()));
        //        }
        //    }
        //});

        // auto idNdx = _model->index(_model->rowCount() - 1, 0);
        //_ui->tv->setIndexWidget(idNdx, idCmb);

        // auto sigNdx = _model->index(_model->rowCount() - 1, 1);
        //_ui->tv->setIndexWidget(sigNdx, sigCmb);

        // auto leNdx = _model->index(_model->rowCount() - 1, 2);
        //_ui->tv->setIndexWidget(leNdx, le);

        // auto pbNdx = _model->index(_model->rowCount() - 1, _model->columnCount() - 1);
        //_ui->tv->setIndexWidget(pbNdx, bt);

        // if (id.length() > 0 && sig.length() > 0) {
        //    idCmb->setCurrentText(id);
        //    sigCmb->setCurrentText(sig);
        //}

        // le->setText(val);
    }

    QJsonArray getRows() override
    {
        QJsonArray ret;
        QJsonObject item;

        for (int i = 0; i < _model->rowCount(); ++i) {
            auto id = reinterpret_cast<QComboBox*>(_ui->tv->indexWidget(_model->index(i, 0)));
            auto sig = reinterpret_cast<QComboBox*>(_ui->tv->indexWidget(_model->index(i, 1)));
            auto val = reinterpret_cast<QLineEdit*>(_ui->tv->indexWidget(_model->index(i, 2)));

            item["id"] = id->currentText();
            item["sig"] = sig->currentText();
            item["val"] = val->text();

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
