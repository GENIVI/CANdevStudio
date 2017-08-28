#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "log.hpp"
#include "ui_canrawview.h"
#include "uniquefiltermodel.h"
#include <QDebug>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>

namespace Ui {
class CanRawViewPrivate;
}

class QElapsedTimer;

class CanRawViewPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
        : ui(std::make_unique<Ui::CanRawViewPrivate>())
        , timer(std::make_unique<QElapsedTimer>())
        , simStarted(false)
        , q_ptr(q)
        , columnsOrder({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" })
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels(columnsOrder);
        ui->tv->setModel(&tvModel);
        uniqueModel.setSourceModel(&tvModel);
        ui->tv->setModel(&uniqueModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setColumnHidden(0, true);
        ui->tv->setColumnHidden(1, true);
        ui->tv->setColumnHidden(3, true);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);
        connect(ui->pbToggleFilter, &QPushButton::pressed, this, &CanRawViewPrivate::setFilter);

        connect(
            ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, [=](int logicalIndex) { sort(logicalIndex); });
    }

    ~CanRawViewPrivate() {}

    void saveSettings(QJsonObject& json)
    {
        QJsonObject jObjects;
        QJsonObject jSortingObject;
        QJsonArray viewModelsArray;
        /*
         * Temporary below code comments use for test during debug and do possibility to write settings to file
         *
        QString fileName("viewSave.cds");
        QFile saveFile(fileName);

        if (saveFile.open(QIODevice::WriteOnly) == false) {
            cds_debug("Problem with open the file {0} to write confiruration", fileName.toStdString());
            return;
        }
        */
        assert(ui != nullptr);
        assert(ui->freezeBox != nullptr);
        assert(q_ptr != nullptr);
        assert(q_ptr->windowTitle().toStdString().length() != 0);

        writeColumnsOrder(jObjects);
        writeSortingRules(jSortingObject);
        jObjects["Sorting"] = std::move(jSortingObject);
        jObjects["Scrolling"] = (ui->freezeBox->isChecked() == true) ? 1 : 0;
        writeViewModel(viewModelsArray);
        jObjects["Models"] = std::move(viewModelsArray);
        json[q_ptr->windowTitle().toStdString().c_str()] = std::move(jObjects);
        /*
        QJsonDocument saveDoc(json);
        saveFile.write(saveDoc.toJson());
        */
    }

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        if (!simStarted) {
            cds_debug("send/received frame while simulation stopped");
            return;
        }

        auto payHex = frame.payload().toHex();
        // insert space between bytes, skip the end
        for (int ii = payHex.size() - 2; ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }

        QList<QVariant> qvList;
        QList<QStandardItem*> list;

        int frameID = frame.frameId();
        double time = timer->elapsed() / 1000.0;

        qvList.append(rowID++);
        qvList.append(std::move(time));
        qvList.append(QString::number(time, 'f', 2));
        qvList.append(std::move(frameID));
        qvList.append(QString("0x" + QString::number(frameID, 16)));
        qvList.append(direction);
        qvList.append(QString::number(frame.payload().size()).toInt());
        qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

        for (QVariant qvitem : qvList) {
            QStandardItem* item = new QStandardItem();
            item->setData(qvitem, Qt::DisplayRole);
            list.append(item);
        }

        tvModel.appendRow(list);

        // Sort after reception of each frame and appending it to tvModel
        currentSortOrder = ui->tv->horizontalHeader()->sortIndicatorOrder();
        auto currentSortIndicator = ui->tv->horizontalHeader()->sortIndicatorSection();
        ui->tv->sortByColumn(sortIndex, currentSortOrder);
        ui->tv->horizontalHeader()->setSortIndicator(currentSortIndicator, currentSortOrder);

        uniqueModel.updateFilter(frameID, time, direction);

        if (ui->freezeBox->isChecked() == false) {
            ui->tv->scrollToBottom();
        }
    }

    std::unique_ptr<Ui::CanRawViewPrivate> ui;
    std::unique_ptr<QElapsedTimer> timer;
    QStandardItemModel tvModel;
    UniqueFilterModel uniqueModel;
    bool simStarted;

private:
    CanRawView* q_ptr;
    int rowID = 0;
    int prevIndex = 0;
    int sortIndex = 0;
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;
    QStringList columnsOrder;

    void writeSortingRules(QJsonObject& json) const
    {
        json["prevIndex"] = prevIndex;
        json["sortIndex"] = sortIndex;
        json["currentSortOrder"] = currentSortOrder;
    }

    void writeColumnsOrder(QJsonObject& json) const
    {
        assert(ui != nullptr);
        assert(ui->tv != nullptr);

        int ii = 0;
        QJsonArray columnList;
        for (const auto& column : columnsOrder) {
            if (ui->tv->isColumnHidden(ii) == false) {
                columnList.append(column);
            }
            ++ii;
        }
        json["Columns"] = std::move(columnList);
    }

    void writeViewModel(QJsonArray& jsonArray) const
    {
        assert(ui != nullptr);
        assert(ui->tv != nullptr);

        for (auto row = 0; row < tvModel.rowCount(); ++row) {
            QJsonArray lineIter;
            for (auto column = 0; column < tvModel.columnCount(); ++column) {
                if (ui->tv->isColumnHidden(column) == false) {
                    auto pp = tvModel.data(tvModel.index(row, column));
                    lineIter.append(std::move(pp.toString()));
                }
            }
            jsonArray.append(std::move(lineIter));
        }
    }

private slots:
    /**
     * @brief clear
     *
     * This function is used to clear data and filter models
     */
    void clear()
    {
        tvModel.removeRows(0, tvModel.rowCount());
        uniqueModel.clearFilter();
    }

    void dockUndock()
    {
        Q_Q(CanRawView);
        emit q->dockUndock();
    }

    void sort(const int clickedIndex)
    {
        currentSortOrder = ui->tv->horizontalHeader()->sortIndicatorOrder();
        sortIndex = clickedIndex;

        if (prevIndex == clickedIndex) {
            if (currentSortOrder == Qt::DescendingOrder) {
                ui->tv->sortByColumn(sortIndex, Qt::DescendingOrder);
                ui->tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::DescendingOrder);
            } else {
                ui->tv->sortByColumn(0, Qt::AscendingOrder);
                ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
                prevIndex = 0;
                sortIndex = 0;
            }
        } else {
            ui->tv->sortByColumn(sortIndex, Qt::AscendingOrder);
            ui->tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::AscendingOrder);
            prevIndex = clickedIndex;
        }
    }

    void setFilter()
    {
        uniqueModel.toggleFilter();

        if (uniqueModel.isFilterActive() == true) {
            ui->pbToggleFilter->setText("Combined view");
        } else {
            ui->pbToggleFilter->setText("Free view");
        }
    }
};
#endif // CANRAWVIEW_P_H
