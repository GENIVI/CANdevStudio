#ifndef CANSIGNALVIEW_P_H
#define CANSIGNALVIEW_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>

class CanSignalViewPrivate : public QObject
{
    Q_OBJECT

public:
    void setupUi(QWidget *w)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QTableView *tv = new QTableView();

        tvModel->setHorizontalHeaderLabels({tr("time"), tr("name"), tr("value")});
        tv->setModel(tvModel);
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 36);
        tv->setColumnWidth(1, 170);
        tv->setColumnWidth(2, 92);

        layout->addWidget(tv);
        w->setLayout(layout);
    }

    QStandardItemModel *tvModel { new QStandardItemModel(0,3) };
};

#endif // CANSIGNALVIEW_P_H

