#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "ui_canrawsender.h"
#include <memory>
#include <QtSerialBus/QCanBusFrame>
#include <QtGui/QStandardItemModel>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QLineEdit>

namespace Ui {
class CanRawSenderPrivate;
}

class CanRawSenderPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender* q)
    : ui(std::make_unique<Ui::CanRawSenderPrivate>())
    , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({"ID", "Data", "Cyclic", "Loop", ""});
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);

        connect(ui->pbAdd, &QPushButton::pressed, this, &CanRawSenderPrivate::addNewItem);
        connect(ui->pbRemove, &QPushButton::pressed, this, &CanRawSenderPrivate::removeRowsSelectedByMouse);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawSenderPrivate::dockUndock);
    }

    ~CanRawSenderPrivate()
    {
    }

private:
    std::unique_ptr<Ui::CanRawSenderPrivate> ui;
    QStandardItemModel tvModel;
    CanRawSender* q_ptr;

private slots:
    /**
     * @brief removeRowsSelectedByMouse
     *
     *  This function is used to remove selected (selected by mouse) rows.
     *
     */
    void removeRowsSelectedByMouse()
    {
        QModelIndexList IndexList = ui->tv->selectionModel()->selectedRows();
        std::list<QModelIndex> tmp = IndexList.toStdList();
        tmp.sort(); // List must to be sorted and reversed because erasing started from last row
        tmp.reverse();
        for (QModelIndex n : tmp) {
            tvModel.removeRow(n.row());
        }
    }

    void addNewItem()
    {
        QStandardItem* id = new QStandardItem();
        QStandardItem* data = new QStandardItem();

        QList<QStandardItem*> list {id, data};
        tvModel.appendRow(list);

        QLineEdit* cyclic = new QLineEdit;
        cyclic->setFrame(false);
        cyclic->setAlignment(Qt::AlignHCenter);
        cyclic->setPlaceholderText("Time in ms");
        QRegExp rx("[1-9]\\d{0,6}");
        QValidator *v = new QRegExpValidator(rx, this);
        cyclic->setValidator(v);
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -3), cyclic);

        QWidget *loopWidget = new QWidget();
        QCheckBox *loopCheckBox = new QCheckBox();
        loopCheckBox->setCheckable(false);
        QHBoxLayout *loopLayout = new QHBoxLayout(loopWidget);
        loopLayout->addWidget(loopCheckBox);
        loopLayout->setAlignment(Qt::AlignCenter);
        loopLayout->setContentsMargins(0,0,0,0);
        loopWidget->setLayout(loopLayout);
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -2), loopWidget);

        connect(cyclic, &QLineEdit::textChanged, this, [this, cyclic, loopCheckBox]{
            if(cyclic->text().length() > 0)
                loopCheckBox->setCheckable(true);
            else
                loopCheckBox->setCheckable(false);
        });

        QPushButton* pbSend = new QPushButton("Send");
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -1), pbSend);

        connect(pbSend, &QPushButton::pressed, this, [this, id, data, cyclic] {
            Q_Q(CanRawSender);

            if (id->text().size() && data->text().size()) {
                quint32 val;
                if (id->text().startsWith("0x"))
                    val = id->text().toUInt(nullptr, 16);
                else
                    val = id->text().toUInt(nullptr, 10);

                QCanBusFrame frame;
                frame.setFrameId(val);
                frame.setPayload(QByteArray::fromHex(data->text().toUtf8()));
                emit q->sendFrame(frame);
            }
        });
    }

    void dockUndock()
    {
        Q_Q(CanRawSender);
        emit q->dockUndock();
    }
};

#endif // CANRAWSENDER_P_H
