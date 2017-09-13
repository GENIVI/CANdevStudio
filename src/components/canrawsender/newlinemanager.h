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

private:
    /// \brief This function performs the necessary things when the meter stops
    void StopTimer();

private:
    CanRawSender* canRawSender;
    QCanBusFrame frame;
    bool simState;

    QTimer timer;
    QValidator* vDec;
    QValidator* vIdHex;
    QValidator* vDataHex;

    NLMFactoryInterface& mFactory;
    std::unique_ptr<CheckBoxInterface> mCheckBox;
    std::unique_ptr<LineEditInterface> mId;
    std::unique_ptr<LineEditInterface> mData;
    std::unique_ptr<LineEditInterface> mInterval;
    std::unique_ptr<PushButtonInterface> mSend;

signals:

private slots:
    void LoopCheckBoxReleased();
    void SetSendButtonState();
    void SendButtonPressed();
    void TimerExpired();
};

#endif // NEWLINEMANAGER_H
