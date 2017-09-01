#ifndef LINEEDITINTERFACE_HPP
#define LINEEDITINTERFACE_HPP

#include <QString>
#include <functional>
class QWidget;

struct LineEditInterface {
    virtual ~LineEditInterface()
    {
    }

    typedef std::function<void()> textChanged_t;
    virtual void textChangedCbk(const textChanged_t& cb) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual void init(const QString& placeholderText, QValidator* qValidator) = 0;
    virtual void setPlaceholderText(const QString& placeholderText) = 0;
    virtual void setDisabled(bool state) = 0;
    virtual int getTextLength() = 0;
    virtual const QString getText() = 0;
};
#endif // LINEEDITINTERFACE_HPP
