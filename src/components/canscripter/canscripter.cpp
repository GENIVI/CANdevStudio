#include "canscripter.h"
#include "canscripter_p.h"
#include <QCanBusFrame>
#include <QVariant>

CanScripter::CanScripter(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanScripterPrivate())
{
    Q_D(CanScripter);
}

CanScripter::~CanScripter()
{
}
