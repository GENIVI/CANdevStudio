#include <log.h>
#include <QDebug>

#include "qmlexecutormodel.h"
#include "qmlexecutorplugin.h"

#include <datamodeltypes/canrawdata.h>
#include <datamodeltypes/cansignalmodel.h>


namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In,
        {
            { CanRawData{}.type(),
              CanSignalModel{}.type()
            }
        }
    },
    { PortType::Out,
        {
            { CanRawData{}.type(),
              CanSignalModel{}.type()
            }
        }
    }
};
// clang-format on

} // namespace


QMLExecutorModel::QMLExecutorModel()
    : ComponentModel("QMLExecutor")
    , _painter(std::make_unique<NodePainter>(QMLExecutorPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    _CANBusModel = std::make_unique<CANBusModel>();

    _component.setCANBusModel(_CANBusModel.get());

    connect(_CANBusModel.get(), &CANBusModel::sendFrame, this, &QMLExecutorModel::sendFrame);
    connect(_CANBusModel.get(), &CANBusModel::sendSignal, this, &QMLExecutorModel::sendSignal);
    connect(this, &QMLExecutorModel::frameReceived, _CANBusModel.get(), &CANBusModel::frameReceived);
    connect(this, &QMLExecutorModel::signalReceived, _CANBusModel.get(), &CANBusModel::signalReceived);
    connect(this, &QMLExecutorModel::simulationStarted, _CANBusModel.get(), &CANBusModel::simulationStarted);
    connect(this, &QMLExecutorModel::simulationStopped, _CANBusModel.get(), &CANBusModel::simulationStopped);
}

QtNodes::NodePainterDelegate* QMLExecutorModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int QMLExecutorModel::nPorts(PortType portType) const
{
    return (int)portMappings.at(portType).size();
}

NodeDataType QMLExecutorModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.find(portType) != std::end(portMappings) && portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);

    return { };
}

std::shared_ptr<NodeData> QMLExecutorModel::outData(PortIndex index)
{
    if(index == 0)
    {
        return getNextQueuedFrame();
    }

    if(index == 1)
    {
        return getNextQueuedSignal();
    }

    // this should not happen
    return {};
}

std::shared_ptr<NodeData> QMLExecutorModel::getNextQueuedFrame()
{
    if(_rawSendQueue.size() > 0)
    {
        auto frame = _rawSendQueue.front();
        _rawSendQueue.pop_front();

        auto data = std::make_shared<CanRawData>(frame);

        return data;
    }

    return {};
}

std::shared_ptr<NodeData> QMLExecutorModel::getNextQueuedSignal()
{
    if(_signalSendQueue.size() > 0)
    {
        auto signal = _signalSendQueue.front();
        _signalSendQueue.pop_front();

        auto data = std::make_shared<CanSignalModel>(signal);

        return data;
    }

    return {};
}


void QMLExecutorModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex index)
{
    if(index == 0)
    {
        auto data = std::dynamic_pointer_cast<CanRawData>(nodeData);

        if(data && data->frame().isValid())
        {
            const auto& id = data->frame().frameId();
            const auto& payload = data->frame().payload();

            emit frameReceived(id, payload);
        }
        else
            cds_warn("Corrupt frame received!");

        return;
    }

    if(index == 1)
    {
        auto signal = std::dynamic_pointer_cast<CanSignalModel>(nodeData);

        if(signal && signal->name().size() > 0)
        {
            emit signalReceived(signal->name(), signal->value());
        }
        else
        {
            cds_warn("Corrupt signal received");
        }

        return;
    }

    // this should not happen
    cds_error("Corrupt in data!");
}

void QMLExecutorModel::sendFrame(const quint32& frameId, const QByteArray& frameData)
{
    QCanBusFrame frame;

    frame.setFrameId(frameId);
    frame.setPayload(frameData);

    _rawSendQueue.push_back(frame);

    emit dataUpdated(0);
}

void QMLExecutorModel::sendSignal(const QString& name, const QVariant& value )
{
    auto data = CanSignalModel(name, value, Direction::TX);

    _signalSendQueue.push_back(data);

    emit dataUpdated(1);
}

CANBusModel* QMLExecutorModel::getCANBusModel()
{
    return _CANBusModel.get();
}
