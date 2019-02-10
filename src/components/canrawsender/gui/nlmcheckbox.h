#ifndef NLMCHECKBOX_H
#define NLMCHECKBOX_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <guiinterface/checkboxinterface.h>

struct NLMCheckBox : public CheckBoxInterface {

    NLMCheckBox()
        : qWidget(new QWidget)
        , qCheckBox(new QCheckBox)
        , qLayout(nullptr)
    {
        init();
        qCheckBox->setProperty("type", "nlmItem");
    }

    void toggledCbk(const toggled_t& cb) override
    {
        QObject::connect(qCheckBox, &QCheckBox::toggled, cb);
    }

    QWidget* mainWidget() override
    {
        return qWidget;
    }

    bool getState() override
    {
        return qCheckBox->isChecked();
    }

    void init()
    {
        qLayout = new QHBoxLayout(qWidget);
        qLayout->addWidget(qCheckBox);
        qLayout->setAlignment(Qt::AlignCenter);
        qLayout->setContentsMargins(0, 0, 0, 0);
        qWidget->setLayout(qLayout);
    }

    void setState(bool state) override
    {
        if (qCheckBox->isChecked() != state) {
            qCheckBox->setChecked(state);
            emit qCheckBox->released();
        }
    }

    void setDisabled(bool state) override
    {
        qCheckBox->setDisabled(state);
    }


private:
    QWidget* qWidget;
    QCheckBox* qCheckBox;
    QHBoxLayout* qLayout;
};

#endif // NLMCHECKBOX_H
