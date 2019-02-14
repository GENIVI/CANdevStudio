#ifndef NLMLINEEDIT_H
#define NLMLINEEDIT_H

#include <QLineEdit>
#include <guiinterface/lineeditinterface.h>

struct NLMLineEdit : public LineEditInterface {

    NLMLineEdit()
        : qLineEdit(new QLineEdit)
    {
        qLineEdit->setProperty("type", "nlmItem");
    }

    void editingFinishedCbk(const textChanged_t& cb) override
    {
        QObject::connect(qLineEdit, &QLineEdit::editingFinished, cb);
    }

    void textChangedCbk(const textChanged_t& cb) override
    {
        QObject::connect(qLineEdit, &QLineEdit::textEdited, cb);
    }

    QWidget* mainWidget() override
    {
        return qLineEdit;
    }

    void init(const QString& placeholderText, QValidator* qValidator) override
    {
        qLineEdit->setFrame(false);
        qLineEdit->setAlignment(Qt::AlignHCenter);
        qLineEdit->setPlaceholderText(placeholderText);
        if (qValidator != nullptr) {
            qLineEdit->setValidator(qValidator);
        }
    }

    void setPlaceholderText(const QString& placeholderText) override
    {
        qLineEdit->setPlaceholderText(placeholderText);
    }

    void setDisabled(bool state) override
    {
        qLineEdit->setDisabled(state);
    }

    int getTextLength() override
    {
        return qLineEdit->text().length();
    }

    const QString getText() override
    {
        return qLineEdit->text();
    }

    void setText(const QString& text) override
    {
        // Inserts new text and validates the result
        qLineEdit->clear();
        qLineEdit->insert(text);
    }

private:
    QLineEdit* qLineEdit;
};

#endif // NLMLINEEDIT_H
