#ifndef CANRAWPLAYERMODEL_H
#define CANRAWPLAYERMODEL_H

#include "canrawplayer.h"
#include "componentmodel.h"
#include "nodepainter.h"
#include <QCanBusFrame>
#include <QtCore/QObject>
#include <readerwriterqueue.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanRawPlayerModel : public ComponentModel<CanRawPlayer, CanRawPlayerModel> {
    Q_OBJECT

public:
    CanRawPlayerModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

    virtual bool hasSeparateThread() const override
    {
        return true;
    }

public slots:

    /**
     *   @brief  Callback, called when CanRawSender emits signal sendFrame, sends frame
     *   @param  received frame
     */
    void sendFrame(const QCanBusFrame& frame);

signals:
    void requestRedraw();

private:
    std::unique_ptr<NodePainter> _painter;
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _msgQueue{ 127 };
};

#endif // CANRAWPLAYERMODEL_H
