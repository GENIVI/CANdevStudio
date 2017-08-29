#ifndef NLMLINEEDIT_HPP
#define NLMLINEEDIT_HPP

#include <guiinterface/nlmlineeditinterface.hpp>
#include <QLineEdit>

struct NLMLineEdit : public NLMLineEditInterface {

    NLMLineEdit()
        : qLineEdit(new QLineEdit)
    {
    }

    void textChangedCbk(const textChanged_t& cb) override { QObject::connect(qLineEdit, &QLineEdit::textChanged, cb); }

    QWidget* getMainWidget() override { return qLineEdit; }

    void init(const QString& placeholderText, QValidator* qValidator) override
    {
        qLineEdit->setFrame(false);
        qLineEdit->setAlignment(Qt::AlignHCenter);
        qLineEdit->setPlaceholderText(placeholderText);
        if (qValidator != nullptr) {
            qLineEdit->setValidator(qValidator);
        }
    }

    void setPlaceholderText(const QString& placeholderText) override { qLineEdit->setPlaceholderText(placeholderText); }

    void setDisabled(bool state) override { qLineEdit->setDisabled(state); }

    int getTextLength() override { return qLineEdit->text().length(); }

    const QString getText() override { return qLineEdit->text(); }

private:
    QLineEdit* qLineEdit;
};

#endif // NLMLINEEDIT_HPP
