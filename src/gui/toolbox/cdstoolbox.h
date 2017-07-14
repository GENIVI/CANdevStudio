#ifndef CDSTOOLBOX_H
#define CDSTOOLBOX_H

#include <QtWidgets/QWidget>

namespace Ui {
class CdsToolbox;
}

class CdsToolbox : public QWidget
{
    Q_OBJECT

public:
    explicit CdsToolbox(QWidget *parent = 0);
    ~CdsToolbox();

private:
    Ui::CdsToolbox *ui;

private slots:
    void startButtonClicked();
    void stopButtonClicked();
};

#endif // CDSTOOLBOX_H
