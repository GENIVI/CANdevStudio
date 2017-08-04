#include "newlinemanager.h"
#include "canrawsender.h"
#include <QRegExpValidator>

NewLineManager::NewLineManager(CanRawSender *q) : canRawSender(q) {
  QRegExp qRegExp("[1]?[0-9A-Fa-f]{7}");
  vIdHex = new QRegExpValidator(qRegExp, this);
  LineEditDefault(id, "Id in hex", vIdHex);

  qRegExp.setPattern("[0-9A-Fa-f]{16}");
  vDataHex = new QRegExpValidator(qRegExp, this);
  LineEditDefault(data, "Data in hex", vDataHex);

  qRegExp.setPattern("[1-9]\\d{0,6}");
  vDec = new QRegExpValidator(qRegExp, this);
  LineEditDefault(cyclic, "Time in ms", vDec);

  send.setText("Send");
  send.setDisabled(true);

  connect(&cyclic, &QLineEdit::textChanged, this,
          &NewLineManager::CyclicTextChanged);
  connect(&loop.qCheckBox, &QCheckBox::released, this,
          &NewLineManager::LoopCheckBoxReleased);
  connect(&id, &QLineEdit::textChanged, this,
          &NewLineManager::SetSendButtonState);
  connect(&data, &QLineEdit::textChanged, this,
          &NewLineManager::SetSendButtonState);
  connect(&send, &QPushButton::pressed, this,
          &NewLineManager::SendButtonPressed);
  connect(&timer, &QTimer::timeout, this, &NewLineManager::TimerExpired);
}

void NewLineManager::LineEditDefault(QLineEdit &lineEdit,
                                     const QString &placeholderText,
                                     QValidator *qValidator) {
  lineEdit.setFrame(false);
  lineEdit.setAlignment(Qt::AlignHCenter);
  lineEdit.setPlaceholderText(placeholderText);
  if (qValidator != nullptr)
    lineEdit.setValidator(qValidator);
}

void NewLineManager::CyclicTextChanged() {
  if (cyclic.text().length() > 0)
    loop.qCheckBox.setCheckable(true);
  else
    loop.qCheckBox.setCheckable(false);
}

void NewLineManager::LoopCheckBoxReleased() {
  if ((loop.qCheckBox.isChecked() == false) && (timer.isActive() == true)) {
    timer.stop();
    id.setDisabled(false);
    data.setDisabled(false);
    cyclic.setDisabled(false);
  }
}

void NewLineManager::SetSendButtonState() {
  if ((id.text().length() > 0) && (data.text().length() > 0)) {
    if (send.isEnabled() == false)
      send.setDisabled(false);
  } else if (send.isEnabled() == true)
    send.setDisabled(true);
}

void NewLineManager::SendButtonPressed() {
  if (id.text().length() && data.text().length()) {
    frame.setFrameId(id.text().toUInt(nullptr, 16));
    frame.setPayload(QByteArray::fromHex(data.text().toUtf8()));
    emit canRawSender->sendFrame(frame);

    if ((timer.isActive() == false) && (loop.qCheckBox.isChecked() == true)) {
      timer.start(cyclic.text().toUInt());
      id.setDisabled(true);
      data.setDisabled(true);
      cyclic.setDisabled(true);
    }
  }
}

void NewLineManager::TimerExpired() {
  emit canRawSender->sendFrame(frame);
}

QWidget *NewLineManager::GetRows(RowNameIterator name) {
  switch (*name) {
  case RowName::IdLine:
    return &id;
  case RowName::DataLine:
    return &data;
  case RowName::CyclicLine:
    return &cyclic;
  case RowName::LoopCheckBox:
    return &loop.qWidget;
  case RowName::SendButton:
    return &send;
  default:
    return nullptr;
  }
}
