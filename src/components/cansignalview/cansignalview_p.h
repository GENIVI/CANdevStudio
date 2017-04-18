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
    void setupUi()
    {
        tvModel->setHorizontalHeaderLabels({tr("time"), tr("name"), tr("value")});
        tv->setModel(tvModel.get());
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 36);
        tv->setColumnWidth(1, 92);
        tv->setColumnWidth(2, 92);

        toolbar->addWidget(pbClear.get());

        layout->addWidget(toolbar.get());
        layout->addWidget(tv.get());
    }

    std::unique_ptr<QVBoxLayout> layout { std::make_unique<QVBoxLayout>() };
    std::unique_ptr<QToolBar> toolbar { std::make_unique<QToolBar>() };
    std::unique_ptr<QTableView> tv { std::make_unique<QTableView>() };
    std::unique_ptr<QStandardItemModel> tvModel { std::make_unique<QStandardItemModel>(0, 3) };
    std::unique_ptr<QPushButton> pbClear { std::make_unique<QPushButton>("Clear") };
};

#endif // CANSIGNALVIEW_P_H

