#include "newlinemanager.h"
#include "canrawsender.h"
#include <QRegExpValidator>

NewLineManager::NewLineManager(CanRawSender* q, bool simulationState)
    : NewLineManager(q, simulationState, mDefFactory)
{
}

NewLineManager::NewLineManager(CanRawSender* q, bool simulationState, NLMFactoryInterface& factory)
    : canRawSender(q)
    , simState(simulationState)
    , mFactory(factory)
{
    if (q == nullptr) {
        throw std::runtime_error("CanRawSender is nullptr");
    }

    // Id
    mId.reset(mFactory.createLineEdit());
    QRegExp qRegExp("[1]?[0-9A-Fa-f]{7}");
    vIdHex = new QRegExpValidator(qRegExp, this);
    mId->init("Id in hex", vIdHex);
    mId->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Data
    mData.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[0-9A-Fa-f]{16}");
    vDataHex = new QRegExpValidator(qRegExp, this);
    mData->init("Id in hex", vDataHex);
    mData->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Interval
    mInterval.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[1-9]\\d{0,6}");
    vDec = new QRegExpValidator(qRegExp, this);
    mInterval->init("Id in hex", vDec);
    mInterval->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Checkbox
    mCheckBox.reset(mFactory.createCheckBox());
    mCheckBox->releasedCbk(std::bind(&NewLineManager::LoopCheckBoxReleased, this));

    // Send button
    mSend.reset(mFactory.createPushButton());
    mSend->init("Send", true);
    mSend->pressedCbk(std::bind(&NewLineManager::SendButtonPressed, this));

    connect(&timer, &QTimer::timeout, this, &NewLineManager::TimerExpired);
}

void NewLineManager::StopTimer()
{
    timer.stop();
    mId->setDisabled(false);
    mData->setDisabled(false);
}

void NewLineManager::LoopCheckBoxReleased()
{
    if (mCheckBox->getState() == false) {
        mInterval->setDisabled(true);
        if (timer.isActive() == true) {
            StopTimer();
        } else if (mInterval->getTextLength() == 0) {
            mInterval->setPlaceholderText("Select Loop");
        }
    } else if (timer.isActive() == false) {
        mInterval->setDisabled(false);
        if (mInterval->getTextLength() == 0) {
            mInterval->setPlaceholderText("Time in ms");
        }
    }
}

void NewLineManager::SetSendButtonState()
{
    if ((mId->getTextLength() > 0) && (simState == true)) {
        if (mSend->isEnabled() == false) {
            mSend->setDisabled(false);
        }
    } else {
        if (mSend->isEnabled() == true) {
            mSend->setDisabled(true);
        }
    }
}

void NewLineManager::SendButtonPressed()
{
    if (mId->getTextLength() > 0) {
        frame.setFrameId(mId->getText().toUInt(nullptr, 16));
        frame.setPayload(QByteArray::fromHex(mData->getText().toUtf8()));
        emit canRawSender->sendFrame(frame);

        if ((timer.isActive() == false) && (mCheckBox->getState() == true)) {
            const auto delay = mInterval->getText().toUInt();
            if (delay != 0) {
                timer.start(delay);
                mId->setDisabled(true);
                mData->setDisabled(true);
                mInterval->setDisabled(true);
            }
        }
    }
}

void NewLineManager::TimerExpired() { emit canRawSender->sendFrame(frame); }

QWidget* NewLineManager::GetColsWidget(ColNameIterator name)
{
    switch (*name) {
    case ColName::IdLine:
        return mId->getMainWidget();
    case ColName::DataLine:
        return mData->getMainWidget();
    case ColName::IntervalLine:
        return mInterval->getMainWidget();
    case ColName::LoopCheckBox:
        return mCheckBox->getMainWidget();
    case ColName::SendButton:
        return mSend->getMainWidget();
    default:
        return nullptr;
    }
}

void NewLineManager::SetSimulationState(bool state)
{
    simState = state;
    SetSendButtonState();
    if ((simState == false) && (timer.isActive() == true)) {
        StopTimer();
        mInterval->setDisabled(false);
    }
}
