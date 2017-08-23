#ifndef NLMCHECKBOX_HPP
#define NLMCHECKBOX_HPP

#include "nlmcheckboxinterface.hpp"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>

struct NLMCheckBox : public NLMCheckBoxInterface {

    NLMCheckBox()
        : qWidget(new QWidget)
        , qCheckBox(new QCheckBox)
        , qLayout(nullptr)
    {
        init();
    }

    void releasedCbk(const released_t& cb) override { QObject::connect(qCheckBox, &QCheckBox::released, cb); }

    QWidget* getMainWidget() override { return qWidget; }

    bool getState() override { return qCheckBox->isChecked(); }

    void init()
    {
        qLayout = new QHBoxLayout(qWidget);
        qLayout->addWidget(qCheckBox);
        qLayout->setAlignment(Qt::AlignCenter);
        qLayout->setContentsMargins(0, 0, 0, 0);
        qWidget->setLayout(qLayout);
    }

private:
    QWidget* qWidget;
    QCheckBox* qCheckBox;
    QHBoxLayout* qLayout;
};

#endif // NLMCHECKBOX_HPP
