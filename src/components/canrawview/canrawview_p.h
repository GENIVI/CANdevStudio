
#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H


#include "uibackend.h" // WithUIBackend

#include <QJsonArray>
#include <QJsonObject>
#include <QtCore/QElapsedTimer>
#include <QtCore/QStringList>
#include <QtCore/Qt>  // SortOrder
#include <QtGui/QStandardItemModel>

#include <memory> // unique_ptr, make_unique


class CanRawView;
class QCanBusFrame;
class QString;


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

    QJsonObject makeSortingRules() const;
    QJsonObject makeColumnsOrder() const;
    QJsonArray makeViewModel() const;


    int _rowID     = 0;
    int _prevIndex = 0;
    int _sortIndex = 0;
    Qt::SortOrder _currentSortOrder = Qt::AscendingOrder;
    QStringList   _columnsOrder =
        { "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" };
};

#endif // CANRAWVIEW_P_H

