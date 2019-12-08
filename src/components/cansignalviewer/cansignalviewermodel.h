#ifndef CANSIGNALVIEWERMODEL_H
#define CANSIGNALVIEWERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <cansignalviewer.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanSignalViewerModel : public ComponentModel<CanSignalViewer, CanSignalViewerModel> {
    Q_OBJECT

public:
    CanSignalViewerModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:

signals:
    void requestRedraw();

private:
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANSIGNALVIEWERMODEL_H
