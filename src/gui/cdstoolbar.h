#ifndef CDSTOOLBAR_H
#define CDSTOOLBAR_H

#include <QtWidgets/QWidget>
#include <memory>

namespace Ui {
class CdsToolbar;
}

//! CANdeviceStudio Toolbar class
class CdsToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit CdsToolbar(QWidget *parent = 0);
    ~CdsToolbar();
signals:
    void simulationStart(); //!< signal emitted when user clicks "start" button in toolbar
    void simulationStop(); //!< signal emitted when user clicks "stop" button in toolbar
private:
    std::unique_ptr<Ui::CdsToolbar> ui;
};

#endif // CDSTOOLBAR_H
