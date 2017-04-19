#ifndef CANSIGNALSENDER_P_H
#define CANSIGNALSENDER_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>
#include "cansignalsender.h"

class CanSignalSenderPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalSender)

public:
    CanSignalSenderPrivate(CanSignalSender *q) :
        q_ptr(q)
    {
    }

    void setupUi()
    {
        tvModel->setHorizontalHeaderLabels({tr("name"), tr("value"), ""});
        tv->setModel(tvModel.get());
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 180);
        tv->setColumnWidth(1, 50);

        toolbar->addWidget(pbAdd.get());

        layout->addWidget(toolbar.get());
        layout->addWidget(tv.get());

        connect(pbAdd.get(), &QPushButton::pressed, [this] () {
                    QStandardItem *name = new QStandardItem();
                    QStandardItem *value = new QStandardItem();
                    QList<QStandardItem*> list {name, value};
                    tvModel->appendRow(list);
                    QPushButton *pbSend = new QPushButton("Send");
                    tv->setIndexWidget(tvModel->index(tvModel->rowCount()-1,2), pbSend);

                    connect(pbSend, &QPushButton::pressed, [this, name, value] () {
                                Q_Q(CanSignalSender);

                                if(name->text().size() && value->text().size()) {
                                    quint32 val;
                                    if(value->text().startsWith("0x"))
                                        val = value->text().toUInt(nullptr, 16);
                                    else
                                        val = value->text().toUInt(nullptr, 10);

                                    QByteArray ba = QByteArray::number(val);
                                    emit q->sendSignal(name->text(), ba);
                                }
                            });
                });

        // Testing code start
        // TODO: remove
        pbAdd->click();
        pbAdd->click();
        tvModel->item(0, 0)->setText("VehicleSpeed");
        tvModel->item(1, 0)->setText("SteeringWheelAngle");
        // Testing code end
    }

    std::unique_ptr<QVBoxLayout> layout { std::make_unique<QVBoxLayout>() };
    std::unique_ptr<QToolBar> toolbar { std::make_unique<QToolBar>() };
    std::unique_ptr<QTableView> tv { std::make_unique<QTableView>() };
    std::unique_ptr<QStandardItemModel> tvModel { std::make_unique<QStandardItemModel>(0, 3) };
    std::unique_ptr<QPushButton> pbAdd { std::make_unique<QPushButton>("Add") };

private:
    CanSignalSender *q_ptr;
};

#endif // CANSIGNALSENDER_P_H
