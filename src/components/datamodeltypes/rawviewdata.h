#ifndef RAWVIEWDATA_H
#define RAWVIEWDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

class RawViewData : public NodeData
{
public:

  RawViewData() {};
  RawViewData(QCanBusFrame const &frame) :_frame(frame) {}

  NodeDataType type() const override
  {
    return NodeDataType {"rawview",
                         "Raw"};
  }

  QCanBusFrame frame() const { return _frame; };

private:
  QCanBusFrame _frame;
};

#endif // RAWVIEWDATA_H
