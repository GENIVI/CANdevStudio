#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "canrawsender.h"
#include "gui/crsgui.h"
#include "newlinemanager.h"
#include <QJsonObject>
#include <QtGui/QStandardItemModel>
#include <context.h>
#include <memory>
#include <nlmfactory.h>

namespace Ui {
class CanRawSenderPrivate;
}

/// \class CanRawSenderPrivate
/// \brief This class is responsible for manage can raw sender window
class CanRawSenderPrivate : public QObject {
    Q_OBJECT

public:
    /// \brief constructor
    /// \brief Create new CanRawSenderPrivate class
    /// \param[in] q Pointer to CanRawSender class
    /// \param[in] ctx CanRawSender context
    CanRawSenderPrivate(CanRawSender* q, CanRawSenderCtx&& ctx = CanRawSenderCtx(new CRSGui, new NLMFactory))
        : _ctx(std::move(ctx))
        , _ui(_ctx.get<CRSGuiInterface>())
        , _nlmFactory(_ctx.get<NLMFactoryInterface>())
        , _simulationState(false)
        , _columnsOrder({ "Id", "Data", "Loop", "Interval", "" })
        , q_ptr(q)
    {
        // NOTE: Implementation must be kept here. Otherwise VS2015 fails to link.

        _tvModel.setHorizontalHeaderLabels(_columnsOrder);

        _ui.initTableView(_tvModel);

        _ui.setAddCbk(std::bind(&CanRawSenderPrivate::addNewItem, this));
        _ui.setRemoveCbk(std::bind(&CanRawSenderPrivate::removeRowsSelectedByMouse, this));

        _ui.setDockUndockCbk([this] {
            docked = !docked;
            emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
        });
    }

    /// \brief destructor
    virtual ~CanRawSenderPrivate() = default;

    /// \brief This method propagates the system simulator state
    /// \param[in] state Actual simulator state
    void setSimulationState(bool state);

    /// \brief This method writes to json format actual can raw senred window properties
    /// \param[in] json Json object
    void saveSettings(QJsonObject& json) const;

    /// \brief This method return actual number of lines in table
    /// \return Line count
    int getLineCount() const;

    /// \brief Used to restore rawsender configurations
    /// \param[in] json Constant reference to json object
    /// \return true if configuration was adopted successfully, false otherwise
    bool restoreConfiguration(const QJsonObject& json);

private:
    /// \brief This method writes columns order
    /// \param[in] json Json object
    void writeColumnsOrder(QJsonObject& json) const;

    /// \brief This method writes columns order
    /// \param[in] json Json object
    void writeSortingRules(QJsonObject& json) const;

    /// \brief This method adopts column properties
    /// \return true if column properties ware adopted successfully, false otherwise
    bool columnAdopt(QJsonObject const& json);

    /// \brief This method adopts sorting properties
    /// \return true if sorting was adopted successfully, false otherwise
    bool sortingAdopt(QJsonObject const& json);

    /// \brief This method adopts content properties
    /// \return true if contents ware adopted successfully, false otherwise
    bool contentAdopt(QJsonObject const& json);

private slots:
    /// \brief This method remove selected rows from table
    void removeRowsSelectedByMouse();

    /// \brief This method adds new line to table
    void addNewItem();

public:
    CanRawSenderCtx _ctx;
    CRSGuiInterface& _ui;
    NLMFactoryInterface& _nlmFactory;
    bool docked{ true };

private:
    std::vector<std::unique_ptr<NewLineManager>> _lines;
    QStandardItemModel _tvModel;
    bool _simulationState;
    int _currentIndex;
    QStringList _columnsOrder;
    CanRawSender* q_ptr;
};

#endif // CANRAWSENDER_P_H
