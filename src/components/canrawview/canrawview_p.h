#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>
#include <QTime>

class CanRawViewPrivate : public QObject
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

    QStandardItemModel *tvModel { new QStandardItemModel(0,5) };
    QTableView *tv { new QTableView() };
    const int cMaxListSize { 1000 };

 private:
    void initTv()
    {
        tvModel->setHorizontalHeaderLabels({tr("time [s]"), tr("dir"), tr("id"), tr("dlc"), tr("data")});
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 70);
        tv->setColumnWidth(1, 27);
        tv->setColumnWidth(2, 92);
        tv->setColumnWidth(3, 25);
        tv->setColumnWidth(4, 178);
    }

    QTime timer;
};

#endif // CANRAWVIEW_P_H
