#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include <memory>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>

class CanRawViewPrivate : public QObject
{
    Q_OBJECT

public:
    void setupUi(QWidget *w)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QTableView *tv = new QTableView();

        tvModel->setHorizontalHeaderLabels({tr("time"), tr("id"), tr("dir"), tr("dlc"), tr("data")});
        tv->setModel(tvModel);
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 36);
        tv->setColumnWidth(1, 92);
        tv->setColumnWidth(2, 27);
        tv->setColumnWidth(3, 25);
        tv->setColumnWidth(4, 178);

        layout->addWidget(tv);
        w->setLayout(layout);
    }
    
    QStandardItemModel *tvModel { new QStandardItemModel(0,5) };
};

#endif // CANRAWVIEW_P_H
