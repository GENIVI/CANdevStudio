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

  enum class RowName {
    IdLine = 0,
    DataLine,
    CyclicLine,
    LoopCheckBox,
    SendButton,
  };

  typedef Iterator<RowName, RowName::IdLine, RowName::SendButton> RowNameIterator;
  QWidget *GetRows(RowNameIterator name);

private:
  void LineEditDefault(QLineEdit &lineEdit, const QString &placeholderText,
                       QValidator *qValidator = nullptr);

  struct CheckBox {
    CheckBox() {
      qLayout = new QHBoxLayout(&qWidget);
      qCheckBox.setCheckable(false);
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
  QLineEdit cyclic;
  CheckBox loop;
  QPushButton send;

  QTimer timer;
  QValidator *vDec;
  QValidator *vIdHex;
  QValidator *vDataHex;

  QCanBusFrame frame;

signals:

private slots:
  void CyclicTextChanged();
  void LoopCheckBoxReleased();
  void SetSendButtonState();
  void SendButtonPressed();
  void TimerExpired();
};

#endif // NEWLINEMANAGER_H
