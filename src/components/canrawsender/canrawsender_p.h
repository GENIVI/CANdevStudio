#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "ui_canrawsender.h"
#include <memory>
#include <QtSerialBus/QCanBusFrame>
#include <QtGui/QStandardItemModel>
#include <QCheckBox>
#include <QLineEdit>
#include <QTimer>


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
    void setSendButtonState(QLineEdit* id, QLineEdit* data, QPushButton* button) {
        if ((id == nullptr) || (data == nullptr) || (button == nullptr))
            return;
        if ((id->text().length() > 0) && (data->text().length() > 0)) {
            if (button->isEnabled() == false)
                button->setDisabled(false);
        }
        else if (button->isEnabled() == true)
            button->setDisabled(true);
    }

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
        QRegExp reDec("[1-9]\\d{0,6}");
        QRegExp reIdHex("[1]?[0-9A-Fa-f]{7}");
        QRegExp reDataHex("[0-9A-Fa-f]{16}");
        QValidator *vDec = new QRegExpValidator(reDec, this);
        QValidator *vIdHex = new QRegExpValidator(reIdHex, this);
        QValidator *vDataHex = new QRegExpValidator(reDataHex, this);

        QList<QStandardItem*> list {};
        tvModel.appendRow(list);

        QTimer *timer = new QTimer(this);
        bool *autoSenderEnable = new bool(false);

        QLineEdit* id = new QLineEdit;
        id->setFrame(false);
        id->setAlignment(Qt::AlignHCenter);
        id->setPlaceholderText("Id in hex");
        id->setValidator(vIdHex);
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -5), id);

        QLineEdit* data = new QLineEdit;
        data->setFrame(false);
        data->setAlignment(Qt::AlignHCenter);
        data->setPlaceholderText("Data in hex");
        data->setValidator(vDataHex);
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -4), data);

        QLineEdit* cyclic = new QLineEdit;
        cyclic->setFrame(false);
        cyclic->setAlignment(Qt::AlignHCenter);
        cyclic->setPlaceholderText("Time in ms");
        cyclic->setValidator(vDec);
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
        connect(loopCheckBox, &QCheckBox::released, this, [autoSenderEnable, loopCheckBox, timer, id, data, cyclic] {
            if ((loopCheckBox->isChecked() == false) && (*autoSenderEnable == true)) {
                *autoSenderEnable = false;
                timer->stop();
                id->setDisabled(false);
                data->setDisabled(false);
                cyclic->setDisabled(false);
            }
        });
        QPushButton* pbSend = new QPushButton("Send");
        pbSend->setDisabled(true);
        ui->tv->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, tvModel.columnCount() -1), pbSend);

        connect(id, &QLineEdit::textChanged, this, [this, id, data, pbSend]{
            setSendButtonState(id, data, pbSend);
        });
        connect(data, &QLineEdit::textChanged, this, [this, id, data, pbSend]{
            setSendButtonState(id, data, pbSend);
        });

        connect(pbSend, &QPushButton::pressed, this, [this, id, data, cyclic, autoSenderEnable, loopCheckBox, timer] {
            Q_Q(CanRawSender);

            if (id->text().length() && data->text().length()) {
                auto val = id->text().toUInt(nullptr, 16);
                QCanBusFrame frame;
                frame.setFrameId(val);
                frame.setPayload(QByteArray::fromHex(data->text().toUtf8()));
                emit q->sendFrame(frame);

                if ((*autoSenderEnable == false) && (loopCheckBox->isChecked() == true))
                {
                    *autoSenderEnable = true;
                    timer->disconnect();
                    connect(timer, &QTimer::timeout, this, [this, frame, ctx]{
                        Q_Q(CanRawSender);
                        emit q->sendFrame(frame, ctx);
                    });
                    timer->start(cyclic->text().toUInt());
                    id->setDisabled(true);
                    data->setDisabled(true);
                    cyclic->setDisabled(true);
                }
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
