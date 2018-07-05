#include <QtWidgets/QApplication>
#include <canrawfilter.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QJsonArray>
#include <QJsonObject>
#include <QSignalSpy>
#include <fakeit.hpp>
#include <gui/canrawfilterguiint.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[canrawfilter]")
{
    CanRawFilter c;

    CHECK(c.mainWidget() != nullptr);
    CHECK(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canrawfilter]")
{
    CanRawFilter c;
    QObject obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[canrawfilter]")
{
    CanRawFilter c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[canrawfilter]")
{
    CanRawFilter c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[canrawfilter]")
{
    CanRawFilter c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[canrawfilter]")
{
    CanRawFilter c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[canrawfilter]")
{
    CanRawFilter c;

    auto props = c.getSupportedProperties();

    CHECK(props.find("name") != props.end());
    CHECK(props.find("dummy") == props.end());
}

QJsonObject getConfig(const CanRawFilterGuiInt::AcceptList_t& acceptList, const QString& listName)
{
    QJsonObject json;
    QJsonArray list;

    for (const auto& lineItem : acceptList) {
        QJsonObject jsonLine;
        jsonLine["id"] = std::get<0>(lineItem);
        jsonLine["payload"] = std::get<1>(lineItem);
        jsonLine["policy"] = std::get<2>(lineItem);
        list.push_back(jsonLine);
    }

    json[listName] = list;

    return json;
}

TEST_CASE("default accept list RX", "[canrawfilter]")
{
    CanRawFilter c;
    QCanBusFrame frame;
    QSignalSpy spy(&c, &CanRawFilter::rxFrameOut);

    c.startSimulation();
    c.rxFrameIn(frame);

    c.stopSimulation();
    c.rxFrameIn(frame);

    CHECK(spy.count() == 1);
}

template <typename M, typename TX, typename RX> void setupMock(M& mock, TX& txCbk, RX& rxCbk)
{
    using namespace fakeit;
    Fake(Dtor(mock));
    Fake(Method(mock, mainWidget));
    When(Method(mock, setTxListCbk)).AlwaysDo([&](auto&& fn) { txCbk = fn; });
    When(Method(mock, setRxListCbk)).AlwaysDo([&](auto&& fn) { rxCbk = fn; });
    Fake(Method(mock, setListTx));
    Fake(Method(mock, setListRx));
}

TEST_CASE("empty accept list RX", "[canrawfilter]")
{
    fakeit::Mock<CanRawFilterGuiInt> gui;
    CanRawFilterGuiInt::ListUpdated_t txCbk;
    CanRawFilterGuiInt::ListUpdated_t rxCbk;
    setupMock(gui, txCbk, rxCbk);

    CanRawFilter c(CanRawFilterCtx(&gui.get()));
    QCanBusFrame frame;
    QSignalSpy spy(&c, &CanRawFilter::rxFrameOut);

    c.startSimulation();
    c.rxFrameIn(frame);
    CHECK(spy.count() == 0);
}

// TEST_CASE("custom list RX", "[canrawfilter]")
//{
// fakeit::Mock<CanRawFilterGuiInt> gui;
// CanRawFilterGuiInt::ListUpdated_t txCbk;
// CanRawFilterGuiInt::ListUpdated_t rxCbk;
// setupMock(gui, txCbk, rxCbk);

// CanRawFilter c(CanRawFilterCtx(&gui.get()));
// QCanBusFrame frame;
// QSignalSpy spy(&c, &CanRawFilter::rxFrameOut);

// c.startSimulation();
// c.rxFrameIn(frame);
// CHECK(spy.count() == 0);
//}

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
