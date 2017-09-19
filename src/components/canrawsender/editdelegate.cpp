//#include "newlinemanager.h"
#include "editdelegate.h"
#include <QRegExpValidator>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QCheckBox>

QWidget* EditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 4) // checkbox 
    {
        QCheckBox* checkBox = new QCheckBox(parent);
       return checkBox;
    }
    else if (index.column() == 5){ //send button
        QPushButton* pushButton = new QPushButton(parent);
        pushButton->setText("Send");
        return pushButton;
    }
    else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void EditDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (index.column() == 4) // checkbox 
    {
        QItemDelegate::setEditorData(editor, index);
    }
    else if (index.column() == 5){ //send button
        QItemDelegate::setEditorData(editor, index);
    }
    else {
        QItemDelegate::setEditorData(editor, index);
    }
}


void EditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    if (index.column() == 4) // checkbox 
    {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
        if (checkBox->isChecked() == true)
        {
            model->setData(index, QVariant::fromValue(true));
        }
        else
        {
           model->setData(index, QVariant::fromValue(false));
        }
    }
    else if (index.column() == 5){ //send button

    }
    else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void EditDelegate::paint( QPainter *painter,const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
   /*  if (index.column() == 4) //checkbox
     { 
            bool isChecked = index.data().toBool();
  // Draw a check box for the status column
        QStyleOptionViewItem opt = option;
        QRect crect = opt.rect;
        //crect.moveLeft(15);
		drawCheck(painter, opt, crect, isChecked ? Qt::Checked : Qt::Unchecked);

        } else {
            
        }        
*/
    QItemDelegate::paint(painter, option, index);
}

void EditDelegate::commitAndCloseEditor()
{

    QCheckBox *editor = qobject_cast<QCheckBox *>(sender());  

    emit commitData(editor);
    emit closeEditor(editor);
}
