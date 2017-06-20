#ifndef CANSIGNALVIEW_P_H
#define CANSIGNALVIEW_P_H

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <memory>

class CanSignalViewPrivate : public QObject {
    Q_OBJECT

public:
    CanSignalViewPrivate()
        : tvModel(0, 3)
        , pbClear("Clear")
    {
    }
    void setupUi()
    {
        tvModel.setHorizontalHeaderLabels({ tr("time"), tr("name"), tr("value") });
        tv.setModel(&tvModel);
        tv.verticalHeader()->hideSection(0);
        tv.setColumnWidth(0, 36);
        tv.setColumnWidth(1, 170);
        tv.setColumnWidth(2, 92);

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

#endif // CANSIGNALVIEW_P_H
