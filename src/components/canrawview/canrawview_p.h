
#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H


#include "uibackend.h" // WithUIBackend

#include <QtCore/QElapsedTimer>
#include <QtCore/QStringList>
#include <QtCore/Qt>  // SortOrder
#include <QtGui/QStandardItemModel>

#include <memory> // unique_ptr, make_unique


class CanRawView;
class QCanBusFrame;
class QJsonObject;
class QString;


/** -- this willgo through the ctor
[](CanRawViewPrivate& v)
{
    v._tvModel.setHorizontalHeaderLabels(v._columnsOrder);

    auto& ui = v.backend();

    ui.initTableView(_tvModel);
    ui.setClearCbk([&v]{ v.clear(); });
    ui.setDockUndockCbk([&v]{ v.dockUndock(); });
    ui.setSectionClikedCbk([&v](int index){ v.sort(index); });
}
*/
/*
was in CanRawViewPrivate(CanRawView* q):
    now done in: ui->setupUi(this);  // done in UIBackend ctor

*/


class CanRawViewPrivate
  : public WithUIBackend<
                CanRawViewPrivate
              , CanRawView
              , CanRawView
              >
  , public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

 public:

    using WithUIBackend::WithUIBackend;

    virtual ~CanRawViewPrivate() = default;  // if delete goes through QObject

    void saveSettings(QJsonObject& json);
    void frameView(const QCanBusFrame& frame, const QString& direction);

    QStandardItemModel             _tvModel;
    std::unique_ptr<QElapsedTimer> _timer = std::make_unique<QElapsedTimer>();
    bool                           _simStarted = false;

 public slots:

    void clear();
    void dockUndock();
    void sort(int index);

 private:

    void writeSortingRules(QJsonObject& json) const;
    void writeColumnsOrder(QJsonObject& json) const;
    void writeViewModel(QJsonArray& jsonArray) const;


    int _rowID     = 0;
    int _prevIndex = 0;
    int _sortIndex = 0;
    Qt::SortOrder _currentSortOrder = Qt::AscendingOrder;
    QStringList   _columnsOrder =
        { "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" };
};

#endif // CANRAWVIEW_P_H

