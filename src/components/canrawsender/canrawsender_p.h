#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>
#include <QCanBusFrame>

class CanRawSenderPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender *q) :
        q_ptr(q)
    {
    }

    void setupUi(QWidget *w)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QToolBar *toolbar = new QToolBar();
        QTableView *tv = new QTableView();
        QStandardItemModel *tvModel = new QStandardItemModel(0,3);
        QPushButton *pbAdd  = new QPushButton("Add");

        tvModel->setHorizontalHeaderLabels({tr("id"), tr("data"), ""});
        tv->setModel(tvModel);
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 92);
        tv->setColumnWidth(1, 178);
        tv->setAutoScroll(true);

        toolbar->addWidget(pbAdd);

        layout->addWidget(toolbar);
        layout->addWidget(tv);

        connect(pbAdd, &QPushButton::pressed, [this, layout, tv, tvModel] () {
                    QStandardItem *id = new QStandardItem();
                    QStandardItem *value = new QStandardItem();
                    QList<QStandardItem*> list {id, value};
                    tvModel->appendRow(list);
                    tv->scrollToBottom();
                    QPushButton *pbSend = new QPushButton("Send");
                    tv->setIndexWidget(tvModel->index(tvModel->rowCount()-1,2), pbSend);

                    connect(pbSend, &QPushButton::pressed, [this, id, value] () {
                                Q_Q(CanRawSender);

                                if(id->text().size() && value->text().size()) {
                                    quint32 val;
                                    if(id->text().startsWith("0x"))
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

        w->setLayout(layout);
    }

private:
    CanRawSender *q_ptr;
};
#endif // CANRAWSENDER_P_H
