#ifndef __CANDEVICEDATA_H
#define __CANDEVICEDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

enum class Direction { RX, TX };

class CanDeviceDataIn : public NodeData {
public:
    CanDeviceDataIn(){};
    CanDeviceDataIn(QCanBusFrame const& frame)
        : _frame(frame)
    {
    }

    NodeDataType type() const override { return NodeDataType{ "rawsender", "Raw" }; }

    QCanBusFrame frame() const { return _frame; };

private:
    QCanBusFrame _frame;
};

class CanDeviceDataOut : public NodeData {
public:
    CanDeviceDataOut(){};
    CanDeviceDataOut(QCanBusFrame const& frame, Direction const direction, bool status)
        : _frame(frame)
        , _direction(direction)
        , _status(status)
    {
    }

    NodeDataType type() const override { return NodeDataType{ "rawview", "Raw" }; }

    QCanBusFrame frame() const { return _frame; };
    Direction direction() const { return _direction; };
    bool status() const { return _status; };

private:
    QCanBusFrame _frame;
    Direction _direction;
    bool _status; // used only for frameSent, ignored for frameReceived
};

#endif /* !__CANDEVICEDATA_H */
