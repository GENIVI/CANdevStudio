#ifndef __PROPERTYFIELDS_H
#define __PROPERTYFIELDS_H

#include <QLineEdit>

struct PropertyFieldInterface {
    virtual void setPropText(const QString& text) = 0;
    virtual QString propText() = 0;
};

struct PropertyTextEdit : public QLineEdit, PropertyFieldInterface {
    PropertyTextEdit()
    {
        setFrame(false);
    }

    void setPropText(const QString& text) override
    {
        setText(text);
    }

    QString propText() override
    {
        return text();
    }
};

#endif /* !__PROPERTYFIELDS_H */
