#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include <QHeaderView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QTime>
#include <QToolBar>
#include <QVBoxLayout>
#include <memory>

class CanRawViewPrivate : public QObject {
    Q_OBJECT

public:
    void setupUi(QWidget* w)
    {
        QVBoxLayout* layout = new QVBoxLayout();

        tv->setModel(tvModel);
        initTv();

        layout->addWidget(tv);
        w->setLayout(layout);
    }

    void startTimer()
    {
        timer.restart();
    }

    QString elapsedTime()
    {
        return QString::number((float)timer.elapsed() / 1000, 'f', 2);
    }

    void clear()
    {
        tvModel->clear();
        initTv();
    }

    void instertRow(const QString& dir, uint32_t id, uint8_t dlc, const QByteArray& data)
    {
        if (tvModel->rowCount() >= cMaxListSize) {
            clear();
        }

        auto payHex = data;
        for (int i = payHex.size(); i >= 2; i -= 2) {
            payHex.insert(i, ' ');
        }

        QList<QStandardItem*> list;
        QStandardItem* item;

        item = new QStandardItem(elapsedTime());
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        list.append(item);

        item = new QStandardItem(dir);
        item->setTextAlignment(Qt::AlignCenter);
        list.append(item);

        item = new QStandardItem("0x" + QString::number(id, 16));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        list.append(item);

        item = new QStandardItem(QString::number(dlc));
        item->setTextAlignment(Qt::AlignCenter);
        list.append(item);

        list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

        tvModel->appendRow(list);
        tv->scrollToBottom();
    }

private:
    void initTv()
    {
        tvModel->setHorizontalHeaderLabels({ tr("time [s]"), tr("dir"), tr("id"), tr("dlc"), tr("data") });
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 70);
        tv->setColumnWidth(1, 40);
        tv->setColumnWidth(2, 92);
        tv->setColumnWidth(3, 25);
        tv->setColumnWidth(4, 178);
        tv->setShowGrid(false);
        tv->verticalHeader()->setVisible(false);
        tv->horizontalHeader()->setStretchLastSection(true);
    }

    QTime timer;
    QStandardItemModel* tvModel{ new QStandardItemModel(0, 5) };
    QTableView* tv{ new QTableView() };
    const int cMaxListSize{ 1000 };
};

#endif // CANRAWVIEW_P_H
