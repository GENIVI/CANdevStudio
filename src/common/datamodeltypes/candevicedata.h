#ifndef __CANDEVICEDATA_H
#define __CANDEVICEDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

/**
*   @brief The enum class describing frame direction
*/
enum class Direction { RX, TX };

/**
*   @brief The class describing data model used as input for CanDevice node
*/
class CanDeviceDataIn : public NodeData {
public:
    CanDeviceDataIn(){};
    CanDeviceDataIn(QCanBusFrame const& frame)
        : _frame(frame)
    {
    }
    /**
    *   @brief  Used to get data type id and displayed text for ports
    *   @return NodeDataType of rawsender
    */
    NodeDataType type() const override
    {
        return NodeDataType{ "rawsender", "Raw" };
    }
    /**
    *   @brief  Used to get frame
    */
    QCanBusFrame frame() const
    {
        return _frame;
    };

private:
    QCanBusFrame _frame;
};

/**
*   @brief The class describing data model used as output for CanDevice node
*/
class CanDeviceDataOut : public NodeData {
public:
    CanDeviceDataOut(){};
    CanDeviceDataOut(QCanBusFrame const& frame, Direction const direction, bool status)
        : _frame(frame)
        , _direction(direction)
        , _status(status)
    {
    }
    /**
    *   @brief  Used to get data type id and displayed text for ports
    *   @return NodeDataType of rawview
    */
    NodeDataType type() const override
    {
        return NodeDataType{ "rawview", "Raw" };
    }

    /**
    *   @brief  Used to get frame
    */
    QCanBusFrame frame() const
    {
        return _frame;
    };

    /**
    *   @brief  Used to get direction
    */
    Direction direction() const
    {
        return _direction;
    };

    /**
    *   @brief  Used to get status
    */
    bool status() const
    {
        return _status;
    };

private:
    QCanBusFrame _frame;
    Direction _direction;
    bool _status; // used only for frameSent, ignored for frameReceived
};

#endif /* !__CANDEVICEDATA_H */
