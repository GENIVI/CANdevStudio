#ifndef NEWLINEMANAGER_H
#define NEWLINEMANAGER_H

#include <QtSerialBus/QCanBusFrame>
#include <QCheckBox>
#include <QLineEdit>
#include <QTimer>
#include <QHBoxLayout>
#include <QPushButton>
#include <QValidator>

class CanRawSender;

class NewLineManager : public QObject {
    Q_OBJECT
public:
    NewLineManager(CanRawSender *q);

    enum RowName { IdLine = 0,
                   DataLine = 1,
                   CyclicLine = 2,
                   LoopCheckBox = 3,
                   SendButton = 4,
                   RowMAX };

    QWidget* GetRows(RowName name);

private:
    void LineEditDefault(QLineEdit &lineEdit, const QString &placeholderText, QValidator *qValidator = nullptr);

    struct CheckBox {
        CheckBox() {
            qLayout = new QHBoxLayout(&qWidget);
            qCheckBox.setCheckable(false);
            qLayout->addWidget(&qCheckBox);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->setContentsMargins(0,0,0,0);
            qWidget.setLayout(qLayout);
        }
        QWidget qWidget;
        QCheckBox qCheckBox;
        QHBoxLayout *qLayout;
    };

    CanRawSender* canRawSender;

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
    QVariant ctx = 0;

signals:

private slots:
    void CyclicTextChanged();
    void LoopCheckBoxReleased();
    void SetSendButtonState();
    void SendButtonPressed();
    void TimerExpired();
};

#endif // NEWLINEMANAGER_H
