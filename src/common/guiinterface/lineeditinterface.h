#ifndef LINEEDITINTERFACE_H
#define LINEEDITINTERFACE_H

#include <QString>
#include <functional>
class QWidget;

struct LineEditInterface {
    virtual ~LineEditInterface()
    {
    }

    typedef std::function<void()> textChanged_t;
    virtual void editingFinishedCbk(const textChanged_t& cb) = 0;
    virtual void textChangedCbk(const textChanged_t& cb) = 0;
    virtual QWidget* mainWidget() = 0;
    virtual void init(const QString& placeholderText, QValidator* qValidator) = 0;
    virtual void setPlaceholderText(const QString& placeholderText) = 0;
    virtual void setDisabled(bool state) = 0;
    virtual int getTextLength() = 0;
    virtual const QString getText() = 0;
    virtual void setText(const QString& text) = 0;
};
#endif // LINEEDITINTERFACE_H
