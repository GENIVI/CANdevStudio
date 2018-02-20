#ifndef CANRAWPLAYERMODEL_H
#define CANRAWPLAYERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <canrawplayer.h>
#include <QCanBusFrame>
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

    static QColor headerColor1()
    {
        return QColor(144, 187, 62);
    }

    static QColor headerColor2()
    {
        return QColor(84, 84, 84);
    }

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

private:
    std::unique_ptr<NodePainter> _painter;
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _msgQueue { 127 };
};

#endif // CANRAWPLAYERMODEL_H
