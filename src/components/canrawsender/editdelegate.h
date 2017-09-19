#ifndef EDITDELEGATE_H
#define EDITDELEGATE_H

#include "newlinemanager.h"
#include "canrawsender.h"
#include <QRegExpValidator>
#include <QStyledItemDelegate>
#include <QItemDelegate>

class EditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    EditDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,     const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint( QPainter *painter,const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
private slots:
    void commitAndCloseEditor();
};


#endif // EDITDELEGATE_H
