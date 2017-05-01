#ifndef CANSIGNALVIEW_P_H
#define CANSIGNALVIEW_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>
#include <QTime>

class CanSignalViewPrivate : public QObject
{
    Q_OBJECT

public:
    void setupUi(QWidget *w)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        
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
        return QString::number((float)timer.elapsed()/1000, 'f', 2);
    }

    void clear()
    {
        tvModel->clear();
        initTv();
    }

    void insertRow(const QString &dir, const QString &name, const QString &val)
    {
        if(tvModel->rowCount() >= cMaxListSize) {
            clear();
        }

        QList<QStandardItem*> list;
        QStandardItem *item;
        
        item = new QStandardItem(elapsedTime());
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        list.append(item);

        item = new QStandardItem(dir);
        item->setTextAlignment(Qt::AlignCenter);
        list.append(item);
        
        item = new QStandardItem(name);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        list.append(item);
        list.append(new QStandardItem(val));
        
        tvModel->appendRow(list);
        tv->scrollToBottom();
    }

 private:
    void initTv()
    {
        tvModel->setHorizontalHeaderLabels({tr("time [s]"), tr("dir"),  tr("name"), tr("value")});
        tv->setColumnWidth(0, 70);
        tv->setColumnWidth(1, 40);
        tv->setColumnWidth(2, 190);
        tv->setColumnWidth(3, 92);
        tv->setShowGrid(false);
        tv->verticalHeader()->setVisible(false);
        tv->horizontalHeader()->setStretchLastSection(true);
    }

    QStandardItemModel *tvModel { new QStandardItemModel(0,4) };
    QTableView *tv { new QTableView() };
    const int cMaxListSize { 1000 };
    QTime timer;
};

#endif // CANSIGNALVIEW_P_H

