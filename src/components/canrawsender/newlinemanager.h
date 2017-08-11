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
    NewLineManager(CanRawSender* q);

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
    /// \brief This function adopt lineEdit properties depending on input parameters and roles
    /// \param[in] lineEdit QLineEdit widget
    /// \param[in] placeholderText Default string
    /// \param[in] qValidator Pointer to validator regular expression input text
    void LineEditDefault(QLineEdit& lineEdit, const QString& placeholderText, QValidator* qValidator = nullptr);

    /// \brief This function performs the necessary things when the meter stops
    void StopTimer();

    /// \struct CheckBox
    /// \brief This structure holds the necessary things for the correct functioning of the check box for line manager
    struct CheckBox {
        CheckBox()
        {
            qLayout = new QHBoxLayout(&qWidget);
            qLayout->addWidget(&qCheckBox);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->setContentsMargins(0, 0, 0, 0);
            qWidget.setLayout(qLayout);
        }
        QWidget qWidget;
        QCheckBox qCheckBox;
        QHBoxLayout* qLayout;
    };

    CanRawSender* canRawSender;

    QLineEdit id;
    QLineEdit data;
    QLineEdit interval;
    CheckBox loop;
    QPushButton send;

    QTimer timer;
    QValidator* vDec;
    QValidator* vIdHex;
    QValidator* vDataHex;

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
