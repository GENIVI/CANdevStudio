#ifndef NEWLINEMANAGER_H
#define NEWLINEMANAGER_H

#include "enumiterator.h" // EnumIterator
#include "nlmfactory.h"
#include <QJsonObject>
#include <QTimer>
#include <QValidator>
#include <QtSerialBus/QCanBusFrame>

#include <memory> // unique_ptr

class CanRawSender;

/// \class NewLineManager
/// \brief This class holds informations about all the columns in line and relationships beteewn them
class NewLineManager : public QObject {
    Q_OBJECT
public:
    /// \brief constructor
    /// \brief Create new line manager class
    /// \param[in] q Pointer to CanRawSender class
    /// \param[in] _simulationState Actual simulation state
    /// \param[in] factory Reference to the factory interface
    /// \throw if CanRawSender pointer not exist
    NewLineManager(CanRawSender* q, bool _simulationState, NLMFactoryInterface& factory);

    /// \enum CalName class enumeration
    /// \brief This is an enumeration used by functions reponsible for return columns information
    enum class ColName {
        IdLine = 0,
        DataLine,
        LoopCheckBox,
        IntervalLine,
        SendButton
    };
    /// \typedef ColNameIterator
    /// \brief Iteratot through ColName content
    using ColNameIterator = EnumIterator<ColName, ColName::IdLine, ColName::SendButton>;

    /// \brief The function return information about an appropriate column
    /// \param[in] name Column name iterator
    /// \return QWidget pointer to the column information, nullptr otherwise
    QWidget* GetColsWidget(ColNameIterator name);

    /// \brief The function is responsible for set simulation state. The state influence for sending frame out and the
    /// internal timer (if was started before)
    /// \param[in] state Actual simulator state
    void SetSimulationState(bool state);

    /// \brief The function is responsible for convert line content to json format
    /// \param[in] json Json object
    void Line2Json(QJsonObject& json) const;

    /// \brief The function is responsible for restore line content from config
    /// \param[in] id Id string
    /// \param[in] data Data string
    /// \param[in] interval Interval string
    /// \param[in] loop Loop enable state
    /// \return true if new line is adopted all input data successfully, false otherwise
    bool RestoreLine(QString& id, QString data, QString interval, bool loop, bool send);

private:
    /// \brief This function performs the necessary things when the meter stops
    void StopTimer();
    void StartTimer();

private:
    CanRawSender* _canRawSender;
    QCanBusFrame _frame;
    bool _simState;

    QTimer _timer;
    QValidator* _vDec;
    QValidator* _vIdHex;
    QValidator* _vDataHex;

    NLMFactoryInterface& mFactory;
    std::unique_ptr<CheckBoxInterface> _loop;
    std::unique_ptr<LineEditInterface> _id;
    std::unique_ptr<LineEditInterface> _data;
    std::unique_ptr<LineEditInterface> _interval;
    std::unique_ptr<PushButtonInterface> _send;

signals:

private slots:
    void LoopToggled(bool checked);
    void SetSendButtonState();
    void SendButtonPressed();
    void TimerExpired();
    void FrameDataChanged();
};

#endif // NEWLINEMANAGER_H
