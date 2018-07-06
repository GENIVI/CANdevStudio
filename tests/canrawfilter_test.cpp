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
    using namespace fakeit;
    Mock<CanRawFilterGuiInt> mock;
    Fake(Dtor(mock));
    Fake(Method(mock, mainWidget));
    Fake(Method(mock, setTxListCbk));
    Fake(Method(mock, setRxListCbk));
    When(Method(mock, setListRx))
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 1); });
    When(Method(mock, setListTx))
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 0); })
        .Do([](auto& list) { CHECK(list.size() == 1); });

    CanRawFilter c(CanRawFilterCtx(&mock.get()));
    QJsonObject obj;

    obj["name"] = "Test Name";
    c.setConfig(obj);

    obj["rxList"] = 0;
    obj["txList"] = 0;
    c.setConfig(obj);

    QJsonArray a;
    a.push_back(1);
    a.push_back(2);
    obj["rxList"] = a;
    obj["txList"] = a;
    c.setConfig(obj);

    QJsonObject itemObj;
    itemObj["a"] = 10;
    a = QJsonArray();
    a.push_back(itemObj);
    obj["rxList"] = a;
    obj["txList"] = a;
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

TEST_CASE("custom list RX", "[canrawfilter]")
{
    fakeit::Mock<CanRawFilterGuiInt> gui;
    CanRawFilterGuiInt::ListUpdated_t txCbk;
    CanRawFilterGuiInt::ListUpdated_t rxCbk;
    setupMock(gui, txCbk, rxCbk);

    CanRawFilter c(CanRawFilterCtx(&gui.get()));
    QCanBusFrame frame;
    QSignalSpy spy(&c, &CanRawFilter::rxFrameOut);

    c.startSimulation();

    auto idTest = [&](uint32_t start, uint32_t end, uint32_t cnt, CanRawFilterGuiInt::AcceptList_t&& list) {
        spy.clear();
        rxCbk(list);

        for (uint32_t i = start; i <= end; ++i) {
            frame.setFrameId(i);
            c.rxFrameIn(frame);
        }

        CHECK(spy.count() == cnt);
    };

    // clang-format off
    idTest(0, 0x7ff, 0x700, { 
            { "1[0-9,A-F]{2}", ".*", false }, 
            { ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x500, { 
            { "^[0-9,A-F]{1}$", ".*", false }, 
            { "^[0-9,A-F]{2}$", ".*", false }, 
            { "3[0-9,A-F]{2}", ".*", false }, 
            { "7[0-9,A-F]{2}", ".*", false }, 
            { ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x300, { 
            { "1[0-9,A-F]{2}", ".*", true }, 
            { "2[0-9,A-F]{2}", ".*", true }, 
            { "3[0-9,A-F]{2}", ".*", true }, 
            { ".*", ".*", false } 
        });
    
    idTest(0x1000, 0x2000, 0x300, { 
            { "11[0-9,A-F]{2}", ".*", true }, 
            { "12[0-9,A-F]{2}", ".*", true }, 
            { "13[0-9,A-F]{2}", ".*", true }, 
            { ".*", ".*", false } 
        });

    idTest(0x1fff8fff, 0x1fffffff, 0x1000, { 
            { "1f{3}9[0-9,A-F]{3}", ".*", true }, 
            { ".*", ".*", false } 
        });

    idTest(0x20000000, 0x20001000, 0, { 
            { "0", ".*", false },
            { ".*", ".*", true } 
        });

    // clang-format on
}

TEST_CASE("empty accept list TX", "[canrawfilter]")
{
    fakeit::Mock<CanRawFilterGuiInt> gui;
    CanRawFilterGuiInt::ListUpdated_t txCbk;
    CanRawFilterGuiInt::ListUpdated_t rxCbk;
    setupMock(gui, txCbk, rxCbk);

    CanRawFilter c(CanRawFilterCtx(&gui.get()));
    QCanBusFrame frame;
    QSignalSpy spy(&c, &CanRawFilter::txFrameOut);

    c.startSimulation();
    c.txFrameIn(frame);
    CHECK(spy.count() == 0);
}

TEST_CASE("custom list TX", "[canrawfilter]")
{
    fakeit::Mock<CanRawFilterGuiInt> gui;
    CanRawFilterGuiInt::ListUpdated_t txCbk;
    CanRawFilterGuiInt::ListUpdated_t rxCbk;
    setupMock(gui, txCbk, rxCbk);

    CanRawFilter c(CanRawFilterCtx(&gui.get()));
    QCanBusFrame frame;
    QSignalSpy spy(&c, &CanRawFilter::txFrameOut);

    c.startSimulation();

    auto idTest = [&](uint32_t start, uint32_t end, uint32_t cnt, CanRawFilterGuiInt::AcceptList_t&& list) {
        spy.clear();
        txCbk(list);

        for (uint32_t i = start; i <= end; ++i) {
            frame.setFrameId(i);
            c.txFrameIn(frame);
        }

        CHECK(spy.count() == cnt);
    };

    // clang-format off
    idTest(0, 0x7ff, 0x700, { 
            { "1[0-9,A-F]{2}", ".*", false }, 
            { ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x500, { 
            { "^[0-9,A-F]{1}$", ".*", false }, 
            { "^[0-9,A-F]{2}$", ".*", false }, 
            { "3[0-9,A-F]{2}", ".*", false }, 
            { "7[0-9,A-F]{2}", ".*", false }, 
            { ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x300, { 
            { "1[0-9,A-F]{2}", ".*", true }, 
            { "2[0-9,A-F]{2}", ".*", true }, 
            { "3[0-9,A-F]{2}", ".*", true }, 
            { ".*", ".*", false } 
        });
    
    idTest(0x1000, 0x2000, 0x300, { 
            { "11[0-9,A-F]{2}", ".*", true }, 
            { "12[0-9,A-F]{2}", ".*", true }, 
            { "13[0-9,A-F]{2}", ".*", true }, 
            { ".*", ".*", false } 
        });

    idTest(0x1fff8fff, 0x1fffffff, 0x1000, { 
            { "1f{3}9[0-9,A-F]{3}", ".*", true }, 
            { ".*", ".*", false } 
        });

    idTest(0x20000000, 0x20001000, 0, { 
            { "0", ".*", false },
            { ".*", ".*", true } 
        });

    // clang-format on
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
