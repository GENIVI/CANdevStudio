#include "editdelegate.h"
#include "crs_sendbutton.h"
#include <QCheckBox>
#include <QItemDelegate>
#include <QRegExpValidator>
#include <QStyledItemDelegate>

EditDelegate::EditDelegate(QAbstractItemModel* model = 0, CanRawSender* q = 0, QWidget* parent = 0)
    : canRawSender(q)
    , QItemDelegate(parent)
{
    this->model = qobject_cast<QStandardItemModel*>(model);
}

QWidget* EditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 4) // checkbox
    {
        QCheckBox* checkBox = new QCheckBox(parent);
        return checkBox;
    } else if (index.column() == 5) { // send button
        CRS_SendButton* pushButton = new CRS_SendButton(index.row(), parent);
        pushButton->setText("Send");

        connect(pushButton, &CRS_SendButton::clicked,
            [=](int section) { prepareFrame(section); }); // << clean up this signals?

        return pushButton;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void EditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QItemDelegate::setEditorData(editor, index);
}

void EditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 4) // checkbox
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(editor);
        if (checkBox->isChecked() == true) {
            model->setData(index, QVariant::fromValue(1));
        } else {
            model->setData(index, QVariant::fromValue(0));
        }
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void EditDelegate::prepareFrame(int section) const
{

    QList<QStandardItem*> frameDataIndexList
        = model->findItems(QString::number(section), Qt::MatchExactly, 0); // match send button index with rowID index

    QStandardItem* item = frameDataIndexList.takeLast();
    int dataRow = item->row(); // real item index in model

    QModelIndex frameIndex = model->index(dataRow, 1);
    QString frameID = model->data(frameIndex).toString();

    QModelIndex dataIndex = model->index(dataRow, 2);
    QString data = model->data(dataIndex).toString();

    QCanBusFrame frame;

    frame.setFrameId(frameID.toUInt(nullptr, 16));
    frame.setPayload(QByteArray::fromHex(data.toUtf8()));

    emit canRawSender->sendFrame(frame);

    // if (mId->getTextLength() > 0) {
    // frame.setFrameId(mId->getText().toUInt(nullptr, 16));
    // frame.setPayload(QByteArray::fromHex(mData->getText().toUtf8()));
    //  emit canRawSender->sendFrame(frame);

    // if ((timer.isActive() == false) && (mCheckBox->getState() == true)) {
    // const auto delay = mInterval->getText().toUInt();
    // if (delay != 0) {
    //    timer.start(delay);
    // mId->setDisabled(true);
    // mData->setDisabled(true);
    // mInterval->setDisabled(true);
    //   }
    // }
    // }
}
