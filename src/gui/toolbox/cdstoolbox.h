#ifndef CDSTOOLBOX_H
#define CDSTOOLBOX_H

#include <QtWidgets/QWidget>

namespace Ui {
class CdsToolbox;
}

//! CANdeviceStudio Toolbox class

class CdsToolbox : public QWidget
{
    Q_OBJECT

public:
    explicit CdsToolbox(QWidget *parent = 0);
    ~CdsToolbox();
signals:
    void simulationStart(); //!< signal emitted when user clicks "start" button in toolbar
    void simulationStop(); //!< signal emitted when user clicks "stop" button in toolbar
private:
    Ui::CdsToolbox *ui;

private slots:
    void startButtonClicked();
    void stopButtonClicked();
};

#endif // CDSTOOLBOX_H
