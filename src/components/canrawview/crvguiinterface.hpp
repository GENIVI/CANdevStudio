#ifndef CRVGUIINTERFACE_H
#define CRVGUIINTERFACE_H

class QWidget;
class QAbstractItemModel;

struct CRVGuiInterface {
    virtual ~CRVGuiInterface() {}

    virtual QWidget* getMainWidget() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
};

#endif // CRVGUIINTERFACE_H
