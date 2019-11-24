#ifndef CANSIGNALMODEL_H
#define CANSIGNALMODEL_H

#include <nodes/NodeDataModel>
#include "datadirection.h"

class CanSignalModel : public NodeData {
public:
    CanSignalModel(){};
    CanSignalModel(const QString& sigName, const QVariant& value, const Direction& dir = Direction::TX)
        : _sigName(sigName)
        , _value(value)
        , _dir(dir)
    {
    }

    NodeDataType type() const override
    {
        return NodeDataType{ "signal", "SIG" };
    }
    
    QVariant value() const
    {
        return _value;
    }

    QString name() const 
    {
        return _sigName;
    }

    Direction direction() const
    {
        return _dir;
    }

private:
    QString _sigName;
    QVariant _value;
    Direction _dir;
};

#endif // CANSIGNALMODEL_H
