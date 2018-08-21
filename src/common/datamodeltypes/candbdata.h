#ifndef __CANDBDATA_H
#define __CANDBDATA_H

#include <nodes/NodeDataModel>
#include <cantypes.hpp>

using DbData_t = std::map<CANmessage, std::vector<CANsignal>>;

class CanDbData : public NodeData {
public:
    CanDbData(){};
    CanDbData(const DbData_t& messages)
        : _messages(messages)
    {
    }

    NodeDataType type() const override
    {
        return NodeDataType{ "DbData", "DB" };
    }

    DbData_t messages() const
    {
        return _messages;
    }

private:
    DbData_t _messages;
};

#endif /* !__CANDBDATA_H */
