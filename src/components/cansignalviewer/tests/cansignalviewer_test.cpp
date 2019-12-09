#include <QtWidgets/QApplication>
#include <cansignalviewer.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QAbstractItemModel>
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <datamodeltypes/datadirection.h>
#include <fakeit.hpp>
#include <gui/cansignalviewerguiint.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignalviewer]")
{
    CanSignalViewer c;

    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignalviewer]")
{
    CanSignalViewer c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignalviewer]")
{
    CanSignalViewer c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[cansignalviewer]")
{
    CanSignalViewer c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignalviewer]")
{
    CanSignalViewer c;

    auto abc = c.getQConfig();
}

TEST_CASE("misc", "[cansignalviewer]")
{
    CanSignalViewer c;

    c.configChanged();
    c.simBcastRcv({}, {});
}

TEST_CASE("getSupportedProperties", "[cansignalviewer]")
{
    CanSignalViewer c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 1);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);
}

TEST_CASE("Start/Stop", "[cansignalviewer]")
{
    using namespace fakeit;

    QAbstractItemModel* sModel = nullptr;
    QAbstractItemModel* uModel = nullptr;
    CanSignalViewerGuiInt::filter_t filterCbk = nullptr;
    CanSignalViewerGuiInt::clear_t clearCbk = nullptr;

    Mock<CanSignalViewerGuiInt> guiMock;

    When(Method(guiMock, setClearCbk)).Do([&](auto&& fn) { clearCbk = fn; });
    Fake(Method(guiMock, setDockUndockCbk));
    Fake(Method(guiMock, setSectionClikedCbk));
    When(Method(guiMock, setFilterCbk)).Do([&](auto&& fn) { filterCbk = fn; });
    When(Method(guiMock, setModel)).Do([&](auto&& model) { sModel = model; }).Do([&](auto&& model) { uModel = model; });
    Fake(Method(guiMock, setSorting));
    Fake(Method(guiMock, initTableView));
    Fake(Method(guiMock, scrollToBottom));
    Fake(Method(guiMock, mainWidget));
    Fake(Method(guiMock, isViewFrozen));
    Fake(Method(guiMock, getSortOrder));
    Fake(Method(guiMock, getClickedColumn));
    Fake(Method(guiMock, isColumnHidden));

    auto& i = guiMock.get();
    CanSignalViewer c{ CanSignalViewerCtx(&i) };

    REQUIRE(sModel != nullptr);
    REQUIRE(filterCbk != nullptr);

    filterCbk(true);

    REQUIRE(uModel != nullptr);
    REQUIRE(sModel != uModel);

    c.rcvSignal("0x123_Signal_name_1", { 10 }, Direction::RX);
    c.rcvSignal("0x10000123_Signal_name_2", { 20 }, Direction::TX);

    REQUIRE(sModel->rowCount() == 0);
    REQUIRE(uModel->rowCount() == 0);

    c.startSimulation();

    c.rcvSignal("0x123_Signal_name_1", { 10 }, Direction::RX);
    c.rcvSignal("0x10000123_Signal_name_2", { 20 }, Direction::TX);

    c.stopSimulation();

    REQUIRE(sModel->rowCount() == 2);
    REQUIRE(uModel->rowCount() == 2);

    c.startSimulation();

    REQUIRE(sModel->rowCount() == 0);
    REQUIRE(uModel->rowCount() == 0);
}

TEST_CASE("Standard/Unique test", "[cansignalviewer]")
{
    using namespace fakeit;

    QAbstractItemModel* sModel = nullptr;
    QAbstractItemModel* uModel = nullptr;
    CanSignalViewerGuiInt::filter_t filterCbk = nullptr;
    CanSignalViewerGuiInt::clear_t clearCbk = nullptr;

    Mock<CanSignalViewerGuiInt> guiMock;

    When(Method(guiMock, setClearCbk)).Do([&](auto&& fn) { clearCbk = fn; });
    Fake(Method(guiMock, setDockUndockCbk));
    Fake(Method(guiMock, setSectionClikedCbk));
    When(Method(guiMock, setFilterCbk)).Do([&](auto&& fn) { filterCbk = fn; });
    When(Method(guiMock, setModel)).Do([&](auto&& model) { sModel = model; }).Do([&](auto&& model) { uModel = model; });
    Fake(Method(guiMock, setSorting));
    Fake(Method(guiMock, initTableView));
    Fake(Method(guiMock, scrollToBottom));
    Fake(Method(guiMock, mainWidget));
    When(Method(guiMock, isViewFrozen)).Return(true, false, true, false, true, false);
    Fake(Method(guiMock, getSortOrder));
    Fake(Method(guiMock, getClickedColumn));
    Fake(Method(guiMock, isColumnHidden));

    auto& i = guiMock.get();
    CanSignalViewer c{ CanSignalViewerCtx(&i) };

    REQUIRE(sModel != nullptr);
    REQUIRE(filterCbk != nullptr);

    filterCbk(true);

    REQUIRE(uModel != nullptr);
    REQUIRE(sModel != uModel);

    c.startSimulation();

    c.rcvSignal("AAA", { 10 }, Direction::TX);
    c.rcvSignal("YYY_AAA", { 10 }, Direction::TX);
    c.rcvSignal("AAA_AAA", { 10 }, Direction::Uninitialized);

    REQUIRE(sModel->rowCount() == 0);
    REQUIRE(sModel->columnCount() == 6);
    REQUIRE(uModel->rowCount() == 0);
    REQUIRE(uModel->columnCount() == 6);

    c.rcvSignal("0x123_Signal_name_1", { 10 }, Direction::RX);
    c.rcvSignal("0x123_Signal_name_1", { 11 }, Direction::TX);
    c.rcvSignal("0x123_Signal_name_1", { 14 }, Direction::RX);

    c.rcvSignal("0x10000123_Signal_name_2", { 20 }, Direction::TX);
    c.rcvSignal("0x10000123_Signal_name_2", { 22 }, Direction::RX);
    c.rcvSignal("0x10000123_Signal_name_2", { 24 }, Direction::TX);

    Verify(Method(guiMock, scrollToBottom)).Exactly(3);

    REQUIRE(sModel->rowCount() == 6);
    REQUIRE(sModel->columnCount() == 6);
    REQUIRE(uModel->rowCount() == 4);
    REQUIRE(uModel->columnCount() == 6);

    REQUIRE(sModel->data(sModel->index(0, 2)).toString() == "RX");
    REQUIRE(sModel->data(sModel->index(0, 3)).toString() == "0x123");
    REQUIRE(sModel->data(sModel->index(0, 4)).toString() == "Signal_name_1");
    REQUIRE(sModel->data(sModel->index(0, 5)).toString() == "10");

    REQUIRE(sModel->data(sModel->index(1, 2)).toString() == "TX");
    REQUIRE(sModel->data(sModel->index(1, 3)).toString() == "0x123");
    REQUIRE(sModel->data(sModel->index(1, 4)).toString() == "Signal_name_1");
    REQUIRE(sModel->data(sModel->index(1, 5)).toString() == "11");

    REQUIRE(sModel->data(sModel->index(2, 2)).toString() == "RX");
    REQUIRE(sModel->data(sModel->index(2, 3)).toString() == "0x123");
    REQUIRE(sModel->data(sModel->index(2, 4)).toString() == "Signal_name_1");
    REQUIRE(sModel->data(sModel->index(2, 5)).toString() == "14");

    REQUIRE(sModel->data(sModel->index(3, 2)).toString() == "TX");
    REQUIRE(sModel->data(sModel->index(3, 3)).toString() == "0x10000123");
    REQUIRE(sModel->data(sModel->index(3, 4)).toString() == "Signal_name_2");
    REQUIRE(sModel->data(sModel->index(3, 5)).toString() == "20");

    REQUIRE(sModel->data(sModel->index(4, 2)).toString() == "RX");
    REQUIRE(sModel->data(sModel->index(4, 3)).toString() == "0x10000123");
    REQUIRE(sModel->data(sModel->index(4, 4)).toString() == "Signal_name_2");
    REQUIRE(sModel->data(sModel->index(4, 5)).toString() == "22");

    REQUIRE(sModel->data(sModel->index(5, 2)).toString() == "TX");
    REQUIRE(sModel->data(sModel->index(5, 3)).toString() == "0x10000123");
    REQUIRE(sModel->data(sModel->index(5, 4)).toString() == "Signal_name_2");
    REQUIRE(sModel->data(sModel->index(5, 5)).toString() == "24");

    // Unique
    REQUIRE(uModel->data(uModel->index(0, 2)).toString() == "RX");
    REQUIRE(uModel->data(uModel->index(0, 3)).toString() == "0x123");
    REQUIRE(uModel->data(uModel->index(0, 4)).toString() == "Signal_name_1");
    REQUIRE(uModel->data(uModel->index(0, 5)).toString() == "14");

    REQUIRE(uModel->data(uModel->index(1, 2)).toString() == "TX");
    REQUIRE(uModel->data(uModel->index(1, 3)).toString() == "0x123");
    REQUIRE(uModel->data(uModel->index(1, 4)).toString() == "Signal_name_1");
    REQUIRE(uModel->data(uModel->index(1, 5)).toString() == "11");

    REQUIRE(uModel->data(uModel->index(2, 2)).toString() == "TX");
    REQUIRE(uModel->data(uModel->index(2, 3)).toString() == "0x10000123");
    REQUIRE(uModel->data(uModel->index(2, 4)).toString() == "Signal_name_2");
    REQUIRE(uModel->data(uModel->index(2, 5)).toString() == "24");

    REQUIRE(uModel->data(uModel->index(3, 2)).toString() == "RX");
    REQUIRE(uModel->data(uModel->index(3, 3)).toString() == "0x10000123");
    REQUIRE(uModel->data(uModel->index(3, 4)).toString() == "Signal_name_2");
    REQUIRE(uModel->data(uModel->index(3, 5)).toString() == "22");

    REQUIRE(clearCbk != nullptr);
    clearCbk();

    REQUIRE(sModel->rowCount() == 0);
    REQUIRE(sModel->columnCount() == 6);
    REQUIRE(uModel->rowCount() == 0);
    REQUIRE(uModel->columnCount() == 6);
}

TEST_CASE("Overrun", "[cansignalviewer]")
{
    using namespace fakeit;

    QAbstractItemModel* sModel = nullptr;
    QAbstractItemModel* uModel = nullptr;
    CanSignalViewerGuiInt::filter_t filterCbk = nullptr;
    CanSignalViewerGuiInt::clear_t clearCbk = nullptr;

    Mock<CanSignalViewerGuiInt> guiMock;

    When(Method(guiMock, setClearCbk)).Do([&](auto&& fn) { clearCbk = fn; });
    Fake(Method(guiMock, setDockUndockCbk));
    Fake(Method(guiMock, setSectionClikedCbk));
    When(Method(guiMock, setFilterCbk)).Do([&](auto&& fn) { filterCbk = fn; });
    When(Method(guiMock, setModel)).Do([&](auto&& model) { sModel = model; }).Do([&](auto&& model) { uModel = model; });
    Fake(Method(guiMock, setSorting));
    Fake(Method(guiMock, initTableView));
    Fake(Method(guiMock, scrollToBottom));
    Fake(Method(guiMock, mainWidget));
    Fake(Method(guiMock, isViewFrozen));
    Fake(Method(guiMock, getSortOrder));
    Fake(Method(guiMock, getClickedColumn));
    Fake(Method(guiMock, isColumnHidden));

    auto& i = guiMock.get();
    CanSignalViewer c{ CanSignalViewerCtx(&i) };

    REQUIRE(sModel != nullptr);
    REQUIRE(filterCbk != nullptr);

    filterCbk(true);

    REQUIRE(uModel != nullptr);
    REQUIRE(sModel != uModel);

    c.startSimulation();

    for (int i = 3000; i < 6000; ++i) {
        auto sigName = QString(fmt::format("0x{:08x}_Signal_Name", i).c_str());
        c.rcvSignal(sigName, { 10 }, Direction::RX);
    }

    REQUIRE(sModel->rowCount() == 2000);
    REQUIRE(uModel->rowCount() == 3000);

    for (int i = 0; i < 2000; ++i) {
        REQUIRE(sModel->data(sModel->index(i, 3)).toString().toUInt(nullptr, 16) == i + 4000);;
    }

    for (int i = 0; i < 3000; ++i) {
        REQUIRE(uModel->data(uModel->index(i, 3)).toString().toUInt(nullptr, 16) == i + 3000);;
    }
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
