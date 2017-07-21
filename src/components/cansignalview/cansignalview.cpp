#include "cansignalview.h"
#include "cansignalview_p.h"
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtGui/QStandardItem>

CanSignalView::CanSignalView(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanSignalViewPrivate(this))
{
    Q_D(CanSignalView);

    setLayout(d->ui->layout);
}

CanSignalView::~CanSignalView()
{
}

void CanSignalView::signalReceived(const QString& name, const QByteArray& val)
{
    Q_D(CanSignalView);

    QList<QStandardItem*> list;
    list.append(new QStandardItem("0"));
    list.append(new QStandardItem(name));
    list.append(new QStandardItem(QString::number(val.toUInt())));

    d->tvModel.appendRow(list);
}
