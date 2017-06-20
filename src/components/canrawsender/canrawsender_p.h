#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTableView>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>
#include <QtSerialBus/QCanBusFrame>

class CanRawSenderPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender *q) :
        tvModel (0,3), pbAdd("Add"), q_ptr(q)
    {
    }

    void setupUi()
    {
        tvModel.setHorizontalHeaderLabels({tr("id"), tr("data"), ""});
        tv.setModel(&tvModel);
        tv.verticalHeader()->hideSection(0);

        toolbar.addWidget(&pbAdd);

        layout.addWidget(&toolbar);
        layout.addWidget(&tv);

        connect(&pbAdd, &QPushButton::pressed, [this] () {
                    QStandardItem *id = new QStandardItem();
                    QStandardItem *value = new QStandardItem();
                    QList<QStandardItem*> list {id, value};
                    tvModel.appendRow(list);
                    QPushButton *pbSend = new QPushButton("Send");
                    tv.setIndexWidget(tvModel.index(tvModel.rowCount()-1,2), pbSend);

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
    }

    QVBoxLayout layout;
    QToolBar toolbar;
    QTableView tv;
    QStandardItemModel tvModel;
    QPushButton pbAdd;

private:
    CanRawSender *q_ptr;
};
#endif // CANRAWSENDER_P_H
