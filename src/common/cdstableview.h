#ifndef __CDSTABLEVIEW_H
#define __CDSTABLEVIEW_H

#include <QObject>
#include <QHeaderView>
#include <QModelIndex>
#include <QTableView>

class CDSTableView : public QTableView {
    Q_OBJECT

public:
    CDSTableView(QWidget* parent)
        : QTableView(parent)
    {
         connect(this, &CDSTableView::clicked, this, &CDSTableView::editWrapper);
    }

private slots:
    void editWrapper(const QModelIndex& index)
    {
        QTableView::edit(index, QAbstractItemView::EditKeyPressed, nullptr);
    }
};

#endif /* !__CDSTABLEVIEW_H */
