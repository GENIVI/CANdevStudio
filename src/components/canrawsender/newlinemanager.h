#ifndef NEWLINEMANAGER_H
#define NEWLINEMANAGER_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QValidator>
#include <QtSerialBus/QCanBusFrame>

class CanRawSender;

template < typename typeName, typeName beginVal, typeName endVal>
class Iterator {
  typedef typename std::underlying_type<typeName>::type val_t;
  int val;
public:
  Iterator(const typeName & f) : val(static_cast<val_t>(f)) {}
  Iterator() : val(static_cast<val_t>(beginVal)) {}
  Iterator operator++() {
    ++val;
    return *this;
  }
  typeName operator*() { return static_cast<typeName>(val); }
  Iterator begin() { return *this; }
  Iterator end() {
      static const Iterator endIter = ++Iterator(endVal);
      return endIter;
  }
  bool operator!=(const Iterator& f) { return val != f.val; }
};

class NewLineManager : public QObject {
  Q_OBJECT
public:
  NewLineManager(CanRawSender *q);

  enum class ColName {
    IdLine = 0,
    DataLine,
    LoopCheckBox,
    IntervalLine,
    SendButton,
  };

  typedef Iterator<ColName, ColName::IdLine, ColName::SendButton> ColNameIterator;
  QWidget *GetColsWidget(ColNameIterator name);
  void SetSimulationState(bool state);

private:
  void LineEditDefault(QLineEdit &lineEdit, const QString &placeholderText,
                       QValidator *qValidator = nullptr);
  void StopTimer();

  struct CheckBox {
    CheckBox() {
      qLayout = new QHBoxLayout(&qWidget);
      qLayout->addWidget(&qCheckBox);
      qLayout->setAlignment(Qt::AlignCenter);
      qLayout->setContentsMargins(0, 0, 0, 0);
      qWidget.setLayout(qLayout);
    }
    QWidget qWidget;
    QCheckBox qCheckBox;
    QHBoxLayout *qLayout;
  };

  CanRawSender *canRawSender;

  QLineEdit id;
  QLineEdit data;
  QLineEdit interval;
  CheckBox loop;
  QPushButton send;

  QTimer timer;
  QValidator *vDec;
  QValidator *vIdHex;
  QValidator *vDataHex;

  QCanBusFrame frame;
  bool simState;

signals:

private slots:
  void LoopCheckBoxReleased();
  void SetSendButtonState();
  void SendButtonPressed();
  void TimerExpired();
};

#endif // NEWLINEMANAGER_H
