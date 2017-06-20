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
    CanRawViewPrivate(): tvModel(0,5), pbClear("Clear")
    {
    }

    void setupUi()
    {
        tvModel.setHorizontalHeaderLabels({tr("time"), tr("id"), tr("dir"), tr("dlc"), tr("data")});
        tv.setModel(&tvModel);
        tv.verticalHeader()->hideSection(0);
        tv.setColumnWidth(0, 36);
        tv.setColumnWidth(1, 92);
        tv.setColumnWidth(2, 27);
        tv.setColumnWidth(3, 25);
        tv.setColumnWidth(4, 178);

        toolbar.addWidget(&pbClear);

        layout.addWidget(&toolbar);
        layout.addWidget(&tv);
    }

    QVBoxLayout layout;
    QToolBar toolbar;
    QTableView tv;
    QStandardItemModel tvModel;
    QPushButton pbClear;
};

#endif // CANRAWVIEW_P_H
