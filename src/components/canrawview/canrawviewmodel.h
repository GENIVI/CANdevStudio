#ifndef CANRAWVIEWMODEL_H
#define CANRAWVIEWMODEL_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <canrawview/canrawview.h>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class CanRawViewModel : public NodeDataModel {
    Q_OBJECT

public:
    CanRawViewModel();

    virtual ~CanRawViewModel() {}

public:
    QString caption() const override { return QString("CanRawView Node"); } // TODO

    QString name() const override { return QString("CanRawViewModel"); }

    std::unique_ptr<NodeDataModel> clone() const override { return std::make_unique<CanRawViewModel>(); }

public:
    virtual QString modelName() const { return QString("Raw view"); }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget* embeddedWidget() override { return label; }

    bool resizable() const override { return false; }
    CanRawView canRawView;

private:
    QLabel* label;
    QCanBusFrame _frame;
    CanRawView viewWindow;
};

#endif // CANRAWVIEWMODEL_H
