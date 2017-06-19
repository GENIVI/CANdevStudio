#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTableView>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>

class CanRawViewPrivate : public QObject
{
    Q_OBJECT

public:
    void setupUi()
    {
        tvModel->setHorizontalHeaderLabels({tr("time"), tr("id"), tr("dir"), tr("dlc"), tr("data")});
        tv->setModel(tvModel.get());
        tv->verticalHeader()->hideSection(0);
        tv->setColumnWidth(0, 36);
        tv->setColumnWidth(1, 92);
        tv->setColumnWidth(2, 27);
        tv->setColumnWidth(3, 25);
        tv->setColumnWidth(4, 178);

        toolbar->addWidget(pbClear.get());

        layout->addWidget(toolbar.get());
        layout->addWidget(tv.get());
    }

    std::unique_ptr<QVBoxLayout> layout { std::make_unique<QVBoxLayout>() };
    std::unique_ptr<QToolBar> toolbar { std::make_unique<QToolBar>() };
    std::unique_ptr<QTableView> tv { std::make_unique<QTableView>() };
    std::unique_ptr<QStandardItemModel> tvModel { std::make_unique<QStandardItemModel>(0, 5) };
    std::unique_ptr<QPushButton> pbClear { std::make_unique<QPushButton>("Clear") };
};

#endif // CANRAWVIEW_P_H
