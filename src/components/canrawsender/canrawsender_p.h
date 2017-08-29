#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "crsfactory.hpp"
#include "crsgui.hpp"
#include "newlinemanager.h"
#include "ui_canrawsender.h"
#include <QJsonObject>
#include <QtGui/QStandardItemModel>
#include <memory>

namespace Ui {
class CanRawSenderPrivate;
}

class CanRawSender;

/// \class CanRawSenderPrivate
/// \brief This class is responsible for manage can raw sender window
class CanRawSenderPrivate : public QObject {
    Q_OBJECT

public:
    /// \brief constructor
    /// \brief Create new CanRawSenderPrivate class
    /// \param[in] q Pointer to CanRawSender class
    CanRawSenderPrivate(CanRawSender* q);

    /// \brief constructor
    /// \brief Create new CanRawSenderPrivate class
    /// \param[in] q Pointer to CanRawSender class
    /// \param[in] factory Reference to factory inteface
    CanRawSenderPrivate(CanRawSender* q, CRSFactoryInterface& factory);

    /// \brief destructor
    virtual ~CanRawSenderPrivate() = default;

    /// \brief This method propagates the system simulator state
    /// \param[in] state Actual simulator state
    void SetSimulationState(bool state);

    /// \brief This method writes to json format actual can raw senred window properties
    /// \param[in] json Json object
    void saveSettings(QJsonObject& json) const;

private:
    /// \brief This method writes columns order
    /// \param[in] json Json object
    void writeColumnsOrder(QJsonObject& json) const;

    /// \brief This method writes columns order
    /// \param[in] json Json object
    void writeSortingRules(QJsonObject& json) const;

private slots:
    /// \brief This method remove selected rows from table
    void removeRowsSelectedByMouse();

    /// \brief This method adds new line to table
    void addNewItem();

    /// \brief This method dock/undock can raw sender window
    void dockUndock();

public:
    CRSFactoryInterface& mFactory;
    std::unique_ptr<CRSGuiInterface> mUi;

private:
    std::vector<std::unique_ptr<NewLineManager>> lines;
    QStandardItemModel tvModel;
    CanRawSender* canRawSender;
    bool simulationState;
    CRSFactory mDefFactory;
    int currentIndex;
    QStringList columnsOrder;
};

#endif // CANRAWSENDER_P_H
