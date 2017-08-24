#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "canrawviewbackend.hpp"
#include "uibackend.hpp"

#include <QtCore/QStringList>
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>

#include <memory> // unique_ptr

class CanRawView;
class QCanBusFrame;
class QString;
class QJsonObject;
class QJsonArray;

namespace Ui { class CanRawViewPrivate; }

class CanRawViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:

    CanRawViewPrivate(CanRawView* q);

    CanRawViewPrivate(CanRawView* q, UIBackend<CanRawView>& backend);

    ~CanRawViewPrivate() = default;

    void frameView(const QCanBusFrame& frame, const QString& direction);

    UIBackend<CanRawView>& backend();
    const UIBackend<CanRawView>& backend() const;

    void saveSettings(QJsonObject& json);

    std::unique_ptr<QElapsedTimer> timer;
    bool simStarted = false;
    QStandardItemModel tvModel;

private:

    void init();

    CanRawView* q_ptr = nullptr;

    /** @{ If backend is passed explicitly in constructor, uiHandle stores
     *     pointer to it, and backend memory is not managed by uiRep.
     *     Otherwise, uiRep manages memory.
     *
     *     NEVER perform delete on uiHandle.
     *     DO NOT reorder uiRep and uiHandle.
     *
     * */
    std::unique_ptr<UIBackend<CanRawView>> uiRep;
    UIBackend<CanRawView>* uiHandle;
    /** @} */

    int rowID = 0;
    int prevIndex = 0;
    int sortIndex = 0;
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;
    
    QStringList columnsOrder;

    void writeSortingRules(QJsonObject& json) const;
    
    void writeColumnsOrder(QJsonObject& json) const;

    void writeViewModel(QJsonArray& jsonArray) const;

private slots:

    /** Clears whole table. */
    void clear();

    void dockUndock();

    void sort(int clickedIndex);
};
#endif // CANRAWVIEW_P_H
