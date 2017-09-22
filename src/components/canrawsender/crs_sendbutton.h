#ifndef CRS_SENDBUTTON_H
#define CRS_SENDBUTTON_H

#include <QPushButton>

class CRS_SendButton : public QPushButton
{
    Q_OBJECT
public:
    CRS_SendButton(int row, QWidget *parent = 0) : QPushButton(parent), m_row(row)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
    }

signals:
    void clicked(int row);

private slots:
    void onClicked()
    {
        emit clicked(m_row);
    }
private:
    int m_row;
};

#endif // CRS_SENDBUTTON_H
