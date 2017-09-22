#ifndef EDITDELEGATE_H
#define EDITDELEGATE_H

#include "canrawsender.h"
#include <QCanBusFrame>
#include <QItemDelegate>
#include <QStandardItemModel>

class EditDelegate : public QItemDelegate {
    Q_OBJECT

public:
    EditDelegate(QAbstractItemModel* model, CanRawSender* q, QWidget* parent);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& Windex) const override;

signals:

private slots:
    void prepareFrame(const int section) const;

private:
    CanRawSender* canRawSender;
    QStandardItemModel* model;
};

#endif // EDITDELEGATE_H
