#include <QtWidgets/QApplication>
#include <projectconfig/canrawviewmodel.h>
#include <datamodeltypes/canrawviewdata.h>
#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <fakeit.hpp>
#include <log.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[canrawview]")
{
    using namespace fakeit;
    CanRawViewModel canRawViewModel;
    CHECK(canRawViewModel.caption() == "CanRawView Node");
    CHECK(canRawViewModel.name() == "CanRawViewModel");
    CHECK(canRawViewModel.modelName() == "Raw view");
    CHECK(canRawViewModel.resizable() == false);
    CHECK(dynamic_cast<CanRawViewModel*>(canRawViewModel.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(canRawViewModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    CanRawViewDataIn canRawViewDataIn;
    CHECK(canRawViewModel.nPorts(PortType::Out) == 0);
    CHECK(canRawViewModel.nPorts(PortType::In) == 1);
    CHECK(canRawViewModel.nPorts(PortType::None) == 0);
    CHECK(canRawViewModel.dataType(PortType::In, 0).id == canRawViewDataIn.type().id);
    CHECK(canRawViewModel.dataType(PortType::In, 0).name == canRawViewDataIn.type().name);
}

TEST_CASE("Calling setInData with direction TX will result in frameSent being emitted", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canRawViewDataIn = std::make_shared<CanRawViewDataIn>(testFrame, Direction::TX, true);
    QSignalSpy frameSentSpy(&canRawViewModel, &CanRawViewModel::frameSent);

    canRawViewModel.setInData(canRawViewDataIn, 0);
    CHECK(frameSentSpy.count() == 1);
    CHECK(qvariant_cast<QCanBusFrame>(frameSentSpy.takeFirst().at(1)).frameId() == testFrame.frameId());
}

TEST_CASE("Calling setInData with direction RX will result in frameReceived being emitted", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canRawViewDataIn = std::make_shared<CanRawViewDataIn>(testFrame, Direction::RX, true);
    QSignalSpy frameReceivedSpy(&canRawViewModel, &CanRawViewModel::frameReceived);

    canRawViewModel.setInData(canRawViewDataIn, 0);
    CHECK(frameReceivedSpy.count() == 1);
    CHECK(qvariant_cast<QCanBusFrame>(frameReceivedSpy.takeFirst().at(0)).frameId() == testFrame.frameId());
}

TEST_CASE("Test save configuration", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QJsonObject json = canRawViewModel.save();
    CHECK(json.find("name") != json.end());
    CHECK(json.find("columns") != json.end());
    CHECK(json.find("scrolling") != json.end());
    CHECK(json.find("models") != json.end());
    CHECK(json.find("sorting") != json.end());
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when contructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
