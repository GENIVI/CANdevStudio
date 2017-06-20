#include <QtCore/QList>
#include <QtGui/QStandardItem>
#include <QtCore/QString>
#include "cansignalview.h"
#include "cansignalview_p.h"

CanSignalView::CanSignalView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanSignalViewPrivate())
{
    Q_D(CanSignalView);

    d->setupUi();
    setLayout(&d->layout);
}

CanSignalView::~CanSignalView()
{
}

void CanSignalView::signalReceived(const QString &name, const QByteArray &val)
{
    Q_D(CanSignalView);

    QList<QStandardItem*> list;
    list.append(new QStandardItem("0"));
    list.append(new QStandardItem(name));
    list.append(new QStandardItem(QString::number(val.toUInt())));
    
    d->tvModel.appendRow(list);
}

