#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include <QCanBusFrame>
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <memory>

class CanRawSenderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender* q)
        : q_ptr(q)
    {
    }

    void setupUi(QWidget* w)
    {
        QVBoxLayout* layout = new QVBoxLayout();
        QToolBar* toolbar = new QToolBar();
        QTableView* tv = new QTableView();
        QStandardItemModel* tvModel = new QStandardItemModel(0, 3);
        QPushButton* pbAdd = new QPushButton("Add");

        tvModel->setHorizontalHeaderLabels({ tr("id"), tr("data"), "" });
        tv->setModel(tvModel);
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 92);
        tv->setColumnWidth(1, 178);

        toolbar->addWidget(pbAdd);

        layout->addWidget(toolbar);
        layout->addWidget(tv);

        connect(pbAdd, &QPushButton::pressed, [this, layout, tv, tvModel]() {
            QStandardItem* id = new QStandardItem();
            QStandardItem* value = new QStandardItem();
            QList<QStandardItem*> list{ id, value };
            tvModel->appendRow(list);
            tv->scrollToBottom();
            QPushButton* pbSend = new QPushButton("Send");
            tv->setIndexWidget(tvModel->index(tvModel->rowCount() - 1, 2), pbSend);

            connect(pbSend, &QPushButton::pressed, [this, id, value]() {
                Q_Q(CanRawSender);

                if (!started) {
                    qDebug() << "CanDevice not activated";
                    return;
                }

                if (id->text().size()) {
                    quint32 val;
                    if (id->text().startsWith("0x"))
                        val = id->text().toUInt(nullptr, 16);
                    else
                        val = id->text().toUInt(nullptr, 10);

                    QCanBusFrame frame;
                    frame.setFrameId(val);
                    frame.setPayload(QByteArray::fromHex(value->text().toUtf8()));
                    QVariant ctx = 0;
                    emit q->sendFrame(frame, ctx);
                }
            });
        });

        // Testing code start
        // TODO: remove
        pbAdd->click();
        pbAdd->click();
        pbAdd->click();
        tvModel->item(0, 0)->setText("0x111");
        tvModel->item(0, 1)->setText("23 02 03 03 0a 48 11 22");
        tvModel->item(1, 0)->setText("0x12345678");
        tvModel->item(1, 1)->setText("12345678");
        tvModel->item(2, 0)->setText("0x666");
        tvModel->item(2, 1)->setText("");
        // Testing code end

        w->setLayout(layout);
    }

    bool started{ false };

private:
    CanRawSender* q_ptr;
};
#endif // CANRAWSENDER_P_H
