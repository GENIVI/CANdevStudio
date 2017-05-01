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

    void setupUi(QWidget *w)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QToolBar *toolbar = new QToolBar();
        QTableView *tv = new QTableView();
        QStandardItemModel *tvModel = new QStandardItemModel(0,3);
        QPushButton *pbAdd  = new QPushButton("Add");

        tvModel->setHorizontalHeaderLabels({tr("name"), tr("value"), ""});
        tv->setModel(tvModel);
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 180);
        tv->setColumnWidth(1, 50);
        tv->setAutoScroll(true);

        toolbar->addWidget(pbAdd);

        layout->addWidget(toolbar);
        layout->addWidget(tv);

        connect(pbAdd, &QPushButton::pressed, [this, tv, tvModel] () {
                    QStandardItem *name = new QStandardItem();
                    QStandardItem *value = new QStandardItem();
                    QList<QStandardItem*> list {name, value};
                    tvModel->appendRow(list);
                    tv->scrollToBottom();
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
    
        w->setLayout(layout);    
    }

private:
    CanSignalSender *q_ptr;
};

#endif // CANSIGNALSENDER_P_H
