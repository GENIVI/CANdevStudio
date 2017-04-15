#include <QMdiArea>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include "mainwindow.h"
#include "candevice/candevice.h"
#include "canrawview/canrawview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mdi(std::make_unique<QMdiArea>()),
    canDevice(std::make_unique<CanDevice>()),
    canRawView(std::make_unique<CanRawView>())
{
    mdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(mdi.get());

    mdi->addSubWindow(canRawView.get());
    mdi->tileSubWindows();
}

MainWindow::~MainWindow()
{
}
