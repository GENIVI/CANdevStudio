#ifndef RAWVIEWDATA_H
#define RAWVIEWDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

class RawViewData : public NodeData {
public:
    RawViewData(){};
    RawViewData(QCanBusFrame const& frame, QString const direction, bool status)
        : _frame(frame)
        , _direction(direction)
        , _status(status)
    {
    }

    NodeDataType type() const override { return NodeDataType{ "rawview", "Raw" }; }

    QCanBusFrame frame() const { return _frame; };
    QString direction() const { return _direction; };
    bool status() const { return _status; };

private:
    QCanBusFrame _frame;
    QString _direction;
    bool _status; // used only for frameSent, ignored for frameReceived
};

#endif // RAWVIEWDATA_H
