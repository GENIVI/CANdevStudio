#include <QtWidgets/QApplication>
#include <canrawfiltermodel.h>
#include <datamodeltypes/canrawdata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[canrawfilterModel]")
{
    using namespace fakeit;
    CanRawFilterModel cm;
    CHECK(cm.caption() == "CanRawFilter");
    CHECK(cm.name() == "CanRawFilter");
    CHECK(cm.resizable() == false);
    CHECK(cm.hasSeparateThread() == false);
    CHECK(dynamic_cast<CanRawFilterModel*>(cm.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    CHECK(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canrawfilterModel]")
{
    CanRawFilterModel cm;

    CHECK(cm.nPorts(QtNodes::PortType::Out) == 1);
    CHECK(cm.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[canrawfilterModel]")
{
    CanRawFilterModel cm;

    NodeDataType ndt;

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    CHECK(ndt.id == "rawframe");
    CHECK(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::In, 0);
    CHECK(ndt.id == "rawframe");
    CHECK(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::In, 1);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");
}

TEST_CASE("outData empty", "[canrawfilterModel]")
{
    CanRawFilterModel cm;

    auto nd = cm.outData(0);
    CHECK(!nd);
}

TEST_CASE("setInData RX", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    auto data = std::make_shared<CanRawData>(frame, Direction::RX);
    QSignalSpy spy(&cm, &CanRawFilterModel::filterRx);

    cm.setInData(data, 0);
    CHECK(spy.count() == 1);
}

TEST_CASE("setInData TX success", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    auto data = std::make_shared<CanRawData>(frame, Direction::TX, true);
    QSignalSpy spy(&cm, &CanRawFilterModel::filterTx);

    cm.setInData(data, 0);
    CHECK(spy.count() == 1);
}

TEST_CASE("setInData TX fail", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    auto data = std::make_shared<CanRawData>(frame, Direction::TX, false);
    QSignalSpy spy(&cm, &CanRawFilterModel::filterTx);

    cm.setInData(data, 0);
    CHECK(spy.count() == 0);
}

TEST_CASE("setInData Undefined", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    auto data = std::make_shared<CanRawData>(frame, (Direction)10, true);
    QSignalSpy spy1(&cm, &CanRawFilterModel::filterTx);
    QSignalSpy spy2(&cm, &CanRawFilterModel::filterRx);

    cm.setInData(data, 0);
    cm.setInData({}, 0);
    CHECK(spy1.count() == 0);
    CHECK(spy2.count() == 0);
}

TEST_CASE("filteredTx success", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    QSignalSpy spy(&cm, &CanRawFilterModel::dataUpdated);

    CHECK(cm.outData(0).get() == nullptr);

    cm.filteredTx(frame);
    CHECK(spy.count() == 1);
    CHECK(cm.outData(0).get() != nullptr);
}

TEST_CASE("filteredTx queue full", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    QSignalSpy spy(&cm, &CanRawFilterModel::dataUpdated);

    for (int i = 0; i < 200; ++i) {
        cm.filteredTx(frame);
    }

    CHECK(spy.count() == 127);
}

TEST_CASE("filteredRx success", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    QSignalSpy spy(&cm, &CanRawFilterModel::dataUpdated);

    CHECK(cm.outData(0).get() == nullptr);

    cm.filteredRx(frame);
    CHECK(spy.count() == 1);
    CHECK(cm.outData(0).get() != nullptr);
}

TEST_CASE("filteredRx queue full", "[canrawfilterModel]")
{
    CanRawFilterModel cm;
    QCanBusFrame frame;
    QSignalSpy spy(&cm, &CanRawFilterModel::dataUpdated);

    for (int i = 0; i < 200; ++i) {
        cm.filteredRx(frame);
    }

    CHECK(spy.count() == 127);
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
