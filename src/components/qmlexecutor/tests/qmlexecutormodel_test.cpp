#define CATCH_CONFIG_RUNNER
#include <QCanBusFrame>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QTableView>
#include <Qt>
#include <QtWidgets/QApplication>
#include <catch.hpp>
#include <context.h>
#include <fakeit.hpp>


#include "qmlexecutormodel.h"
#include "qmlexecutor.h"
#include "gui/qmlexecutorguiint.h"


using namespace fakeit;

std::shared_ptr<spdlog::logger> kDefaultLogger;

Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[qmlexecutorModel]")
{
    using namespace fakeit;
    QMLExecutorModel cm;
    REQUIRE(cm.caption() == "QMLExecutor");
    REQUIRE(cm.name() == "QMLExecutor");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<QMLExecutorModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 2);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 2);
}

TEST_CASE("dataType", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    NodeDataType ndt;

    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "signal");
    REQUIRE(ndt.name == "SIG");

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "signal");
    REQUIRE(ndt.name == "SIG");

    ndt = cm.dataType(QtNodes::PortType::None, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::In, 3);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::Out, 3);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData_nodata_raw", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("outData_nodata_signal", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    auto nd = cm.outData(1);

    REQUIRE(!nd);
}

TEST_CASE("outData_frame_simple", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    quint32 frameId = 123;
    QByteArray frameData("ab\0cd", 5);

    cm.sendFrame(frameId, frameData);
    auto nd = cm.outData(0);

    REQUIRE(nd);

    auto data = std::dynamic_pointer_cast<CanRawData>(nd);

    REQUIRE(frameId == data->frame().frameId());
    REQUIRE(frameData == data->frame().payload());

    nd = cm.outData(0);

    REQUIRE(!nd);
}

TEST_CASE("outData_signal_simple", "[qmlexecutorModel]")
{
   QMLExecutorModel cm;

   QString name{ "name" };
   QString value{ "abcdef" };

   cm.sendSignal(name, value);

   auto nd = cm.outData(1);

   REQUIRE(nd);

   auto data = std::dynamic_pointer_cast<CanSignalModel>(nd);

   REQUIRE(name == data->name());
   REQUIRE(value == data->value());
}

TEST_CASE("outData_nodata_corrupt", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    auto nd = cm.outData(12);

    REQUIRE(!nd);
}

TEST_CASE("setInData_corrupt_frame", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    auto bad = std::make_shared<CanRawData>(QCanBusFrame(QCanBusFrame::InvalidFrame));

    int count{0};
    QObject::connect(cm.getCANBusModel(), &CANBusModel::frameReceived,
        [&count](const quint32&, const QByteArray&)
        {
            count++;
        });

    cm.setInData(bad, 0);

    REQUIRE(count == 0);
}

TEST_CASE("setInData_corrupt_signal", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;


    int count{ 0 };

    QObject::connect(cm.getCANBusModel(), &CANBusModel::signalReceived,
        [&count](const QString&, const QVariant&)
        {
            count++;
        });


    cm.setInData(std::make_shared<CanSignalModel>("", ""), 1);

    REQUIRE(count == 0);
}

TEST_CASE("setInData_corrupt_port", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;


    int count{ 0 };

    QObject::connect(cm.getCANBusModel(), &CANBusModel::signalReceived,
        [&count](const QString&, const QVariant&)
        {
            count++;
        });

    QObject::connect(cm.getCANBusModel(), &CANBusModel::frameReceived,
        [&count](const quint32&, const QByteArray&)
        {
            count++;
        });


    cm.setInData(std::make_shared<CanSignalModel>("", ""), 123);

    REQUIRE(count == 0);
}

TEST_CASE("setInData_frame", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    int frameId = 21;
    QByteArray data = QByteArray("ab\0cd", 5);
    auto frameData = std::make_shared<CanRawData>(QCanBusFrame(frameId, data));

    int count{ 0 };
    int checkFrameId;
    QByteArray checkData;

    QObject::connect(cm.getCANBusModel(), &CANBusModel::frameReceived,
        [&count, &checkFrameId, &checkData](const quint32& frameId, const QByteArray& frameData)
        {
            count++;
            checkFrameId = frameId;
            checkData = frameData;
        });

    cm.setInData(frameData, 0);

    REQUIRE(count == 1);
    REQUIRE(frameId == checkFrameId);
    REQUIRE(data == checkData);
}

TEST_CASE("setInData_signal", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    QString name{ "name" };
    QString value{ "abcdef" };

    int count{ 0 };
    QString checkName;
    QVariant checkValue;

    QObject::connect(cm.getCANBusModel(), &CANBusModel::signalReceived,
        [&count, &checkName, &checkValue](const QString& name, const QVariant& value)
        {
            count++;
            checkName = name;
            checkValue = value;
        });


    cm.setInData(std::make_shared<CanSignalModel>(name, value), 1);

    REQUIRE(count == 1);
    REQUIRE(checkName == checkName);
    REQUIRE(value == checkValue.toString());
}

TEST_CASE("outData_frame", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    uint32_t frameId = 21;
    QByteArray frameData = QByteArray("ab\0cd", 5);

    int count{ 0 };
    int checkPort{ -9 };
    QByteArray checkData;

    QObject::connect(&cm, &QMLExecutorModel::dataUpdated, [&count, &checkPort](int portIndex) {
        count++;
        checkPort = portIndex;
    });

    cm.getCANBusModel()->sendFrame(frameId, frameData);

    auto data = std::dynamic_pointer_cast<CanRawData>(cm.outData(0));

    REQUIRE(count == 1);
    REQUIRE(checkPort == 0);
    REQUIRE(data);
    REQUIRE(frameId == data->frame().frameId());
    REQUIRE(frameData == data->frame().payload());
}

TEST_CASE("outData_signal", "[qmlexecutorModel]")
{
    QMLExecutorModel cm;

    QString name{ "name" };
    QString value{ "abcdef" };

    int count{ 0 };
    QString checkName;
    QVariant checkValue;

    QObject::connect(cm.getCANBusModel(), &CANBusModel::signalReceived,
        [&count, &checkName, &checkValue](const QString& name, const QVariant& value) {
            count++;
            checkName = name;
            checkValue = value;
        });

    cm.setInData(std::make_shared<CanSignalModel>(name, value), 1);

    REQUIRE(count == 1);
    REQUIRE(checkName == checkName);
    REQUIRE(value == checkValue.toString());
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
