#ifndef __CANRAWDATA_H
#define __CANRAWDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>
#include "datadirection.h"

using QtNodes::NodeDataType;

/**
*   @brief The class describing data model used as output for CanDevice node
*/
class CanRawData : public NodeData {
public:
    CanRawData(){};
    CanRawData(QCanBusFrame const& frame, Direction const direction = Direction::TX, bool status = true)
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
        return NodeDataType{ "rawframe", "RAW" };
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

#endif /* !__CANRAWDATA_H */
