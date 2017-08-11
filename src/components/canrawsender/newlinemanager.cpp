#include "newlinemanager.h"
#include "canrawsender.h"
#include <QRegExpValidator>

NewLineManager::NewLineManager(CanRawSender* q)
    : canRawSender(q)
    , simState(false)
{
    if (q == nullptr) {
        throw std::runtime_error("CanRawSender is nullptr");
    }
    QRegExp qRegExp("[1]?[0-9A-Fa-f]{7}");
    vIdHex = new QRegExpValidator(qRegExp, this);
    LineEditDefault(id, "Id in hex", vIdHex);

    qRegExp.setPattern("[0-9A-Fa-f]{16}");
    vDataHex = new QRegExpValidator(qRegExp, this);
    LineEditDefault(data, "Data in hex", vDataHex);

    qRegExp.setPattern("[1-9]\\d{0,6}");
    vDec = new QRegExpValidator(qRegExp, this);
    LineEditDefault(interval, "Select Loop", vDec);
    interval.setDisabled(true);

    send.setText("Send");
    send.setDisabled(true);

    connect(&loop.qCheckBox, &QCheckBox::released, this, &NewLineManager::LoopCheckBoxReleased);
    connect(&id, &QLineEdit::textChanged, this, &NewLineManager::SetSendButtonState);
    connect(&data, &QLineEdit::textChanged, this, &NewLineManager::SetSendButtonState);
    connect(&send, &QPushButton::pressed, this, &NewLineManager::SendButtonPressed);
    connect(&timer, &QTimer::timeout, this, &NewLineManager::TimerExpired);
}

void NewLineManager::LineEditDefault(QLineEdit& lineEdit, const QString& placeholderText, QValidator* qValidator)
{
    lineEdit.setFrame(false);
    lineEdit.setAlignment(Qt::AlignHCenter);
    lineEdit.setPlaceholderText(placeholderText);
    if (qValidator != nullptr) {
        lineEdit.setValidator(qValidator);
    }
}

void NewLineManager::StopTimer()
{
    timer.stop();
    id.setDisabled(false);
    data.setDisabled(false);
}

void NewLineManager::LoopCheckBoxReleased()
{

    if (loop.qCheckBox.isChecked() == false) {
        interval.setDisabled(true);
        if (timer.isActive() == true) {
            StopTimer();
        } else if (interval.text().length() == 0) {
            interval.setPlaceholderText("Select Loop");
        }
    }

    if ((loop.qCheckBox.isChecked() == true) && (timer.isActive() == false)) {
        interval.setDisabled(false);
        if (interval.text().length() == 0) {
            interval.setPlaceholderText("Time in ms");
        }
    }
}

void NewLineManager::SetSendButtonState()
{
    if ((id.text().length() > 0) && (simState == true)) {
        if (send.isEnabled() == false) {
            send.setDisabled(false);
        }
    } else if (send.isEnabled() == true)
        send.setDisabled(true);
}

void NewLineManager::SendButtonPressed()
{
    if (id.text().length()) {
        frame.setFrameId(id.text().toUInt(nullptr, 16));
        frame.setPayload(QByteArray::fromHex(data.text().toUtf8()));
        emit canRawSender->sendFrame(frame);

        if ((timer.isActive() == false) && (loop.qCheckBox.isChecked() == true)) {
            auto delay = interval.text().toUInt();
            if (delay != 0) {
                timer.start(delay);
                id.setDisabled(true);
                data.setDisabled(true);
                interval.setDisabled(true);
            }
        }
    }
}

void NewLineManager::TimerExpired() { emit canRawSender->sendFrame(frame); }

QWidget* NewLineManager::GetColsWidget(ColNameIterator name)
{
    switch (*name) {
    case ColName::IdLine:
        return &id;
    case ColName::DataLine:
        return &data;
    case ColName::IntervalLine:
        return &interval;
    case ColName::LoopCheckBox:
        return &loop.qWidget;
    case ColName::SendButton:
        return &send;
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
        interval.setDisabled(false);
    }
}
