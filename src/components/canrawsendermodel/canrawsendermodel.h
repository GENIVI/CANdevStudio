#ifndef CANRAWSENDERMODEL_H
#define CANRAWSENDERMODEL_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <canrawsender/canrawsender.h>

#include <QCanBusFrame>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class CanRawSenderModel : public NodeDataModel {
    Q_OBJECT

public:
    CanRawSenderModel();

    virtual ~CanRawSenderModel() {}

public:
    QString caption() const override { return QString("CanRawSender Node"); }

    QString name() const override { return QString("CanRawSenderModel"); }

    std::unique_ptr<NodeDataModel> clone() const override { return std::make_unique<CanRawSenderModel>(); }

public:
    virtual QString modelName() const { return QString("Raw sender"); }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex) override;

    QWidget* embeddedWidget() override { return label; }

    bool resizable() const override { return false; }

    CanRawSender canRawSender;

private:
    QLabel* label;
    QCanBusFrame _frame;

public slots:
    void sendFrame(const QCanBusFrame& frame);
};

#endif // CANRAWSENDERMODEL_H
