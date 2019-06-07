
#include <cassert> // assert

#include "canrawsender.h"
#include "newlinemanager.h"
#include <QRegExpValidator>
#include <log.h>

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
    QRegExp qRegExp("[0-9A-Fa-f]{1,8}");
    _vIdHex = new QRegExpValidator(qRegExp, this);
    _id->init("Id in hex", _vIdHex);
    _id->textChangedCbk(std::bind(&NewLineManager::FrameDataChanged, this));
    _id->editingFinishedCbk([&] {
        QString text = _id->getText();
        bool ok;
        quint32 id = text.toUInt(&ok, 16);

        if (ok && id > 0x1fffffff) {
            _id->setText("1fffffff");
        } else {
            if (text.length() < 3) {
                _id->setText(text.rightJustified(3, '0'));
            } else if (text.length() > 3 && text.length() < 8) {
                _id->setText(text.rightJustified(8, '0'));
            }
        }
    });

    // Data
    _data.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[0-9A-Fa-f]{16}");
    _vDataHex = new QRegExpValidator(qRegExp, this);
    _data->init("Data in hex", _vDataHex);
    _data->textChangedCbk(std::bind(&NewLineManager::FrameDataChanged, this));

    // Interval
    _interval.reset(mFactory.createLineEdit());
    qRegExp.setPattern("[1-9]\\d{0,6}");
    _vDec = new QRegExpValidator(qRegExp, this);
    _interval->init("ms", _vDec);
    _interval->textChangedCbk(std::bind(&NewLineManager::SetSendButtonState, this));

    // Send button
    _send.reset(mFactory.createPushButton());
    _send->init("Send", false);
    _send->pressedCbk(std::bind(&NewLineManager::SendButtonPressed, this));

    // Checkbox
    _loop.reset(mFactory.createCheckBox());
    _loop->toggledCbk(std::bind(&NewLineManager::LoopToggled, this, std::placeholders::_1));

    connect(&_timer, &QTimer::timeout, this, &NewLineManager::TimerExpired);
}

void NewLineManager::StopTimer()
{
    _timer.stop();
    _id->setDisabled(false);
    _data->setDisabled(false);
    _loop->setDisabled(false);
    _interval->setDisabled(false);
}

void NewLineManager::StartTimer()
{
    const auto delay = _interval->getText().toUInt();

    if (delay != 0) {
        _timer.start(delay);
        _id->setDisabled(true);
        _data->setDisabled(true);
        _interval->setDisabled(true);
        _loop->setDisabled(true);
    } else {
        cds_info("Timer not started. Delay set to 0");
    }
}

void NewLineManager::LoopToggled(bool checked)
{
    _send->setCheckable(checked);
    SetSendButtonState();
}

void NewLineManager::SetSendButtonState()
{
    if ((_id->getTextLength() > 0) && ((_simState == true) || _send->checkable())) {
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
    if (_send->checked()) {
        StopTimer();
    } else {
        quint32 id = _id->getText().toUInt(nullptr, 16);

        _frame.setFrameId(id);
        _frame.setPayload(QByteArray::fromHex(_data->getText().toUtf8()));

        if((id > 0x7ff) || (_id->getText().length() == 8)) {
            _frame.setExtendedFrameFormat(true);
        } else {
            _frame.setExtendedFrameFormat(false);
        }

        if (_simState) {
            emit _canRawSender->sendFrame(_frame);

            if (_send->checkable()) {
                StartTimer();
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

    if (_simState) {
        if (_send->checked()) {
            StartTimer();
        }
    } else {
        StopTimer();
    }
}

void NewLineManager::Line2Json(QJsonObject& json) const
{
    json["id"] = _id->getText();
    json["data"] = _data->getText();
    json["interval"] = _interval->getText();
    json["loop"] = (_loop->getState() == true) ? true : false;
    json["send"] = _send->checked();
}

bool NewLineManager::RestoreLine(QString& id, QString data, QString interval, bool loop, bool send)
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

    if (loop && send) {
        _send->setChecked(true);
    }

    // Chcek if adopted correctly
    if ((_id->getText() != id) || (_data->getText() != data)
        || ((_interval->getText() != interval) && (interval.length() > 0)) || (_loop->getState() != loop)) {
        return false;
    }

    return true;
}

void NewLineManager::FrameDataChanged()
{
    // Update frame data
    quint32 id = _id->getText().toUInt(nullptr, 16);
    _frame.setFrameId(id);
    _frame.setPayload(QByteArray::fromHex(_data->getText().toUtf8()));

    // Update Send button state
    SetSendButtonState();
}