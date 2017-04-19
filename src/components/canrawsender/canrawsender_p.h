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

    void setupUi()
    {
        tvModel->setHorizontalHeaderLabels({tr("id"), tr("data"), ""});
        tv->setModel(tvModel.get());
        tv->verticalHeader()->hideSection(0);

        toolbar->addWidget(pbAdd.get());

        layout->addWidget(toolbar.get());
        layout->addWidget(tv.get());

        connect(pbAdd.get(), &QPushButton::pressed, [this] () {
                    QStandardItem *id = new QStandardItem();
                    QStandardItem *value = new QStandardItem();
                    QList<QStandardItem*> list {id, value};
                    tvModel->appendRow(list);
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
    }

    std::unique_ptr<QVBoxLayout> layout { std::make_unique<QVBoxLayout>() };
    std::unique_ptr<QToolBar> toolbar { std::make_unique<QToolBar>() };
    std::unique_ptr<QTableView> tv { std::make_unique<QTableView>() };
    std::unique_ptr<QStandardItemModel> tvModel { std::make_unique<QStandardItemModel>(0, 3) };
    std::unique_ptr<QPushButton> pbAdd { std::make_unique<QPushButton>("Add") };

private:
    CanRawSender *q_ptr;
};
#endif // CANRAWSENDER_P_H
