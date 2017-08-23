#ifndef NEWLINEMANAGER_H
#define NEWLINEMANAGER_H

#include "nlmfactory.hpp"
#include <QTimer>
#include <QValidator>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

class CanRawSender;
/// \class Iterator
/// \brief This class provide implementation of iterator used to correctly work for loop with enum class
/// \tparam typeName Name of enum class
/// \tparam beginVal Begin value of enum
/// \tparam endVal End value of enum
template <typename typeName, typeName beginVal, typeName endVal> class Iterator {
    typedef typename std::underlying_type<typeName>::type val_t;
    int val;

public:
    Iterator(const typeName& f)
        : val(static_cast<val_t>(f))
    {
    }
    Iterator()
        : val(static_cast<val_t>(beginVal))
    {
    }
    Iterator operator++()
    {
        ++val;
        return *this;
    }
    typeName operator*() { return static_cast<typeName>(val); }
    Iterator begin() { return *this; }
    Iterator end()
    {
        static const Iterator endIter = ++Iterator(endVal);
        return endIter;
    }
    bool operator!=(const Iterator& f) { return val != f.val; }
};

/// \class NewLineManager
/// \brief This class holds informations about all the columns in line and relationships beteewn them
class NewLineManager : public QObject {
    Q_OBJECT
public:
    /// \brief constructor
    /// \brief Create new line manager class
    /// \param[in] q Pointer to CanRawSender class
    /// \param[in] simulationState Actual simulation state
    NewLineManager(CanRawSender* q, bool simulationState);

    /// \brief constructor
    /// \brief Create new line manager class
    /// \param[in] q Pointer to CanRawSender class
    /// \param[in] simulationState Actual simulation state
    /// \param[in] factory Reference to the factory interface
    /// \throw if CanRawSender pointer not exist
    NewLineManager(CanRawSender* q, bool simulationState, NLMFactoryInterface& factory);

    /// \enum CalName class enumeration
    /// \brief This is an enumeration used by functions reponsible for return columns information
    enum class ColName {
        IdLine = 0,
        DataLine,
        LoopCheckBox,
        IntervalLine,
        SendButton,
    };
    /// \typedef ColNameIterator
    /// \brief Iteratot through ColName content
    typedef Iterator<ColName, ColName::IdLine, ColName::SendButton> ColNameIterator;

    /// \brief The function return information about an appropriate column
    /// \param[in] name Column name iterator
    /// \return QWidget pointer to the column information, nullptr otherwise
    QWidget* GetColsWidget(ColNameIterator name);

    /// \brief The function is responsible for set simulation state. The state influence for sending frame out and the
    /// internal timer (if was started before)
    /// \param[in] state Actual simulator state
    void SetSimulationState(bool state);

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

    NLMFactory mDefFactory;
    NLMFactoryInterface& mFactory;
    std::unique_ptr<NLMCheckBoxInterface> mCheckBox;
    std::unique_ptr<NLMLineEditInterface> mId;
    std::unique_ptr<NLMLineEditInterface> mData;
    std::unique_ptr<NLMLineEditInterface> mInterval;
    std::unique_ptr<NLMPushButtonInterface> mSend;

signals:

private slots:
    void LoopCheckBoxReleased();
    void SetSendButtonState();
    void SendButtonPressed();
    void TimerExpired();
};

#endif // NEWLINEMANAGER_H
