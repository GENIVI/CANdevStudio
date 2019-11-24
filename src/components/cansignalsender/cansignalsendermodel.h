#ifndef CANSIGNALSENDERMODEL_H
#define CANSIGNALSENDERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <candbpainter.h>
#include <cansignalsender.h>
#include <readerwriterqueue.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanSignalSenderModel : public ComponentModel<CanSignalSender, CanSignalSenderModel> {
    Q_OBJECT

public:
    CanSignalSenderModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:
    void rcvSignal(const QString& name, const QVariant& val);

signals:
    void requestRedraw();

private:
    std::unique_ptr<CanDbPainter> _painter;
    QString _sigName;
    QVariant _sigVal;
    // 127 to use 4 blocks, 512 bytes each
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _rxQueue{ 127 };
};

#endif // CANSIGNALSENDERMODEL_H
