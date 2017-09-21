
#include <cassert> // assert

#include "canrawsender.h"
#include "newlinemanager.h"
#include <QRegExpValidator>

NewLineManager::NewLineManager(CanRawSender* q, bool _simulationState, NLMFactoryInterface& factory)
    : _canRawSender(q)
    , _simState(_simulationState)
    , mFactory(factory)
{
    if (q == nullptr) {
        throw std::runtime_error("CanRawSender is nullptr");
    }
    // Id
    _id.reset(mFactory.createLineEdit());
    QRegExp qRegExp("[1]?[0-9A-Fa-f]{7}");
    _vIdHex = new QRegExpValidator(qRegExp, this);
    _id->init("Id in hex", _vIdHex);
    _id->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Data
    _data.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[0-9A-Fa-f]{16}");
    _vDataHex = new QRegExpValidator(qRegExp, this);
    _data->init("Data in hex", _vDataHex);
    _data->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Interval
    _interval.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[1-9]\\d{0,6}");
    _vDec = new QRegExpValidator(qRegExp, this);
    _interval->init("Select Loop", _vDec);
    _interval->setDisabled(true);
    _interval->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Checkbox
    _loop.reset(mFactory.createCheckBox());
    _loop->releasedCbk(std::bind(&NewLineManager::LoopCheckBoxReleased, this));

    // Send button
    _send.reset(mFactory.createPushButton());
    _send->init("Send", false);
    _send->pressedCbk(std::bind(&NewLineManager::SendButtonPressed, this));

    connect(&_timer, &QTimer::timeout, this, &NewLineManager::TimerExpired);
}

void NewLineManager::StopTimer()
{
    _timer.stop();
    _id->setDisabled(false);
    _data->setDisabled(false);
}

void NewLineManager::LoopCheckBoxReleased()
{
    if (_loop->getState() == false) {
        _interval->setDisabled(true);
        if (_timer.isActive() == true) {
            StopTimer();
        } else if (_interval->getTextLength() == 0) {
            _interval->setPlaceholderText("Select Loop");
        }
    } else if (_timer.isActive() == false) {
        _interval->setDisabled(false);
        if (_interval->getTextLength() == 0) {
            _interval->setPlaceholderText("Time in ms");
        }
    }
}

void NewLineManager::SetSendButtonState()
{
    if ((_id->getTextLength() > 0) && (_simState == true)) {
        if (_send->isEnabled() == false) {
            _send->setDisabled(false);
        }
    } else {
        if (_send->isEnabled() == true) {
            _send->setDisabled(true);
        }
    }
}

void NewLineManager::SendButtonPressed()
{
    if (_id->getTextLength() > 0) {
        _frame.setFrameId(_id->getText().toUInt(nullptr, 16));
        _frame.setPayload(QByteArray::fromHex(_data->getText().toUtf8()));
        emit _canRawSender->sendFrame(_frame);

        if ((_timer.isActive() == false) && (_loop->getState() == true)) {
            const auto delay = _interval->getText().toUInt();
            if (delay != 0) {
                _timer.start(delay);
                _id->setDisabled(true);
                _data->setDisabled(true);
                _interval->setDisabled(true);
            }
        }
    }
}

void NewLineManager::TimerExpired()
{
    emit _canRawSender->sendFrame(_frame);
}

QWidget* NewLineManager::GetColsWidget(ColNameIterator name)
{
    if (name.end() != name) {
        switch (*name) {
        case ColName::IdLine:
            return _id->mainWidget();
        case ColName::DataLine:
            return _data->mainWidget();
        case ColName::IntervalLine:
            return _interval->mainWidget();
        case ColName::LoopCheckBox:
            return _loop->mainWidget();
        case ColName::SendButton:
            return _send->mainWidget();
        default:
            assert(false); // BUG: impossible!
        }
    }

    return nullptr;
}

void NewLineManager::SetSimulationState(bool state)
{
    _simState = state;
    SetSendButtonState();
    if ((_simState == false) && (_timer.isActive() == true)) {
        StopTimer();
        _interval->setDisabled(false);
    }
}

void NewLineManager::Line2Json(QJsonObject& json) const
{
    json["id"] = _id->getText();
    json["data"] = _data->getText();
    json["interval"] = _interval->getText();
    json["loop"] = (_loop->getState() == true) ? true : false;
}

bool NewLineManager::RestoreLine(QString& id, QString data, QString interval, bool loop)
{
    // Adopt new line requirement
    _id->setText(id);
    _data->setText(data);
    if (interval.length() > 0) {
        _loop->setState(true); // get possibility to write interval data
        _interval->setText(interval);
    }
    if (_loop->getState() != loop) {
        _loop->setState(loop);
    }

    // Chcek if adopted correctly
    if ((_id->getText() != id) || (_data->getText() != data)
        || ((_interval->getText() != interval) && (interval.length() > 0)) || (_loop->getState() != loop)) {
        return false;
    }

    return true;
}
