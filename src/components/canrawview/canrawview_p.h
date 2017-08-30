#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "crvfactory.hpp"
#include "crvgui.hpp"
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
#include <memory>

namespace Ui {
class CanRawViewPrivate;
}

class QElapsedTimer;

class CanRawViewPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
        : CanRawViewPrivate(q, mDefFactory)
    {
    }

    CanRawViewPrivate(CanRawView* q, CRVFactoryInterface& factory)
        : timer(std::make_unique<QElapsedTimer>())
        , simStarted(false)
        , mFactory(factory)
        , q_ptr(q)
        , columnsOrder({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" })
    {
        tvModel.setHorizontalHeaderLabels(columnsOrder);

        mUi.reset(mFactory.createGui());
        mUi->initTableView(tvModel);
        uniqueModel.setSourceModel(&tvModel);
        mUi->setModel(&uniqueModel);

        mUi->setClearCbk(std::bind(&CanRawViewPrivate::clear, this));
        mUi->setDockUndockCbk(std::bind(&CanRawViewPrivate::dockUndock, this));
        mUi->setSectionClikedCbk(std::bind(&CanRawViewPrivate::sort, this, std::placeholders::_1));
        mUi->setFilterCbk(std::bind(&CanRawViewPrivate::setFilter, this));
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
        assert(mUi->getWindowTitle().toStdString().length() != 0);

        writeColumnsOrder(jObjects);
        writeSortingRules(jSortingObject);
        jObjects["Sorting"] = std::move(jSortingObject);
        jObjects["Scrolling"] = mUi->isViewFrozen();
        writeViewModel(viewModelsArray);
        jObjects["Models"] = std::move(viewModelsArray);
        json[mUi->getWindowTitle().toStdString().c_str()] = std::move(jObjects);
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
        currentSortOrder = mUi->getSortOrder();
        int currentSortIndicator = mUi->getSortSection();
        mUi->setSorting(sortIndex, currentSortIndicator, currentSortOrder);

        uniqueModel.updateFilter(frameID, time, direction);

        if (!mUi->isViewFrozen()) {
            mUi->scrollToBottom();
        }
    }

    std::unique_ptr<QElapsedTimer> timer;
    QStandardItemModel tvModel;
    UniqueFilterModel uniqueModel;
    bool simStarted;
    CRVFactoryInterface& mFactory;
    std::unique_ptr<CRVGuiInterface> mUi;

private:
    CanRawView* q_ptr;
    int rowID = 0;
    int prevIndex = 0;
    int sortIndex = 0;
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;
    QStringList columnsOrder;
    CRVFactory mDefFactory;

    void writeSortingRules(QJsonObject& json) const
    {
        json["prevIndex"] = prevIndex;
        json["sortIndex"] = sortIndex;
        json["currentSortOrder"] = currentSortOrder;
    }

    void writeColumnsOrder(QJsonObject& json) const
    {
        int ii = 0;
        QJsonArray columnList;
        for (const auto& column : columnsOrder) {
            if (mUi->isColumnHidden(ii) == false) {
                columnList.append(column);
            }
            ++ii;
        }
        json["Columns"] = std::move(columnList);
    }

    void writeViewModel(QJsonArray& jsonArray) const
    {
        for (auto row = 0; row < tvModel.rowCount(); ++row) {
            QJsonArray lineIter;
            for (auto column = 0; column < tvModel.columnCount(); ++column) {
                if (mUi->isColumnHidden(column) == false) {
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
        currentSortOrder = mUi->getSortOrder();
        sortIndex = clickedIndex;
        QString clickedColumn = mUi->getClickedColumn(clickedIndex);

        if ((clickedColumn == "time") || (clickedColumn == "id")) {
            sortIndex = sortIndex - 1;
        }

        if (prevIndex == clickedIndex) {
            if (currentSortOrder == Qt::DescendingOrder) {
                mUi->setSorting(sortIndex, clickedIndex, Qt::DescendingOrder);
            } else {
                mUi->setSorting(0, 0, Qt::AscendingOrder);
                prevIndex = 0;
                sortIndex = 0;
            }
        } else {
            mUi->setSorting(sortIndex, clickedIndex, Qt::AscendingOrder);
            prevIndex = clickedIndex;
        }
    }

    void setFilter() { uniqueModel.toggleFilter(); }
};
#endif // CANRAWVIEW_P_H
