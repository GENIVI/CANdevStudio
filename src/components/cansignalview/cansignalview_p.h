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

    QStandardItemModel *tvModel { new QStandardItemModel(0,4) };
    QTableView *tv { new QTableView() };
    const int cMaxListSize { 1000 };

 private:
    void initTv()
    {
        tvModel->setHorizontalHeaderLabels({tr("time [s]"), tr("dir"),  tr("name"), tr("value")});
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 70);
        tv->setColumnWidth(1, 27);
        tv->setColumnWidth(2, 170);
        tv->setColumnWidth(3, 92);
    }

    QTime timer;
};

#endif // CANSIGNALVIEW_P_H

