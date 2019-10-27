#include <QtWidgets/QApplication>
#include <canrawfilter.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QJsonArray>
#include <QJsonObject>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <gui/canrawfilterguiint.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[canrawfilter]")
{
    CanRawFilter c;

    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canrawfilter]")
{
    CanRawFilter c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[canrawfilter]")
{
    using namespace fakeit;
    Mock<CanRawFilterGuiInt> mock;
    Fake(Method(mock, mainWidget));
    Fake(Method(mock, setTxListCbk));
    Fake(Method(mock, setRxListCbk));
    When(Method(mock, setListRx))
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 1); });
    When(Method(mock, setListTx))
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 0); })
        .Do([](auto& list) { REQUIRE(list.size() == 1); });

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

    REQUIRE(std::find(std::begin(props), std::end(props), std::make_tuple("name", QVariant::String, true, nullptr))
        != std::end(props));
    REQUIRE(std::find(std::begin(props), std::end(props), std::make_tuple("dummy", QVariant::String, true, nullptr))
        == std::end(props));
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

    REQUIRE(spy.count() == 1);
}

template <typename M, typename TX, typename RX> void setupMock(M& mock, TX& txCbk, RX& rxCbk)
{
    using namespace fakeit;
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
    REQUIRE(spy.count() == 0);
}

TEST_CASE("custom list RX", "[canrawfilter]")
{
    using I = CanRawFilterGuiInt::AcceptListItem_t;

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

        REQUIRE(spy.count() == cnt);
    };

    // clang-format off
    idTest(0, 0x7ff, 0x700, { 
            I{ "1[0-9,A-F]{2}", ".*", false }, 
            I{ ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x500, { 
            I{ "^[0-9,A-F]{1}$", ".*", false }, 
            I{ "^[0-9,A-F]{2}$", ".*", false }, 
            I{ "3[0-9,A-F]{2}", ".*", false }, 
            I{ "7[0-9,A-F]{2}", ".*", false }, 
            I{ ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x300, { 
            I{ "1[0-9,A-F]{2}", ".*", true }, 
            I{ "2[0-9,A-F]{2}", ".*", true }, 
            I{ "3[0-9,A-F]{2}", ".*", true }, 
            I{ ".*", ".*", false } 
        });
    
    idTest(0x1000, 0x2000, 0x300, { 
            I{ "11[0-9,A-F]{2}", ".*", true }, 
            I{ "12[0-9,A-F]{2}", ".*", true }, 
            I{ "13[0-9,A-F]{2}", ".*", true }, 
            I{ ".*", ".*", false } 
        });

    idTest(0x1fff8fff, 0x1fffffff, 0x1000, { 
            I{ "1f{3}9[0-9,A-F]{3}", ".*", true }, 
            I{ ".*", ".*", false } 
        });

    idTest(0x20000000, 0x20001000, 0, { 
            I{ "0", ".*", false },
            I{ ".*", ".*", true } 
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
    REQUIRE(spy.count() == 0);
}

TEST_CASE("custom list TX", "[canrawfilter]")
{
    using I = CanRawFilterGuiInt::AcceptListItem_t;

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

        REQUIRE(spy.count() == cnt);
    };

    // clang-format off
    idTest(0, 0x7ff, 0x700, { 
            I{ "1[0-9,A-F]{2}", ".*", false }, 
            I{ ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x500, { 
            I{ "^[0-9,A-F]{1}$", ".*", false }, 
            I{ "^[0-9,A-F]{2}$", ".*", false }, 
            I{ "3[0-9,A-F]{2}", ".*", false }, 
            I{ "7[0-9,A-F]{2}", ".*", false }, 
            I{ ".*", ".*", true } 
        });
    
    idTest(0, 0x7ff, 0x300, { 
            I{ "1[0-9,A-F]{2}", ".*", true }, 
            I{ "2[0-9,A-F]{2}", ".*", true }, 
            I{ "3[0-9,A-F]{2}", ".*", true }, 
            I{ ".*", ".*", false } 
        });
    
    idTest(0x1000, 0x2000, 0x300, { 
            I{ "11[0-9,A-F]{2}", ".*", true }, 
            I{ "12[0-9,A-F]{2}", ".*", true }, 
            I{ "13[0-9,A-F]{2}", ".*", true }, 
            I{ ".*", ".*", false } 
        });

    idTest(0x1fff8fff, 0x1fffffff, 0x1000, { 
            I{ "1f{3}9[0-9,A-F]{3}", ".*", true }, 
            I{ ".*", ".*", false } 
        });

    idTest(0x20000000, 0x20001000, 0, { 
            I{ "0", ".*", false },
            I{ ".*", ".*", true } 
        });

    // clang-format on
}

TEST_CASE("Payload filtering", "[canrawfilter]")
{
    using I = CanRawFilterGuiInt::AcceptListItem_t;

    fakeit::Mock<CanRawFilterGuiInt> gui;
    CanRawFilterGuiInt::ListUpdated_t txCbk;
    CanRawFilterGuiInt::ListUpdated_t rxCbk;
    setupMock(gui, txCbk, rxCbk);

    CanRawFilter c(CanRawFilterCtx(&gui.get()));
    std::vector<QCanBusFrame> frames;
    QSignalSpy spyRx(&c, &CanRawFilter::rxFrameOut);
    QSignalSpy spyTx(&c, &CanRawFilter::txFrameOut);

    c.startSimulation();

    auto addFrame = [&](const char* payload) {
        QCanBusFrame frame;
        frame.setPayload(QByteArray::fromHex(payload));
        frames.push_back(frame);
    };

    addFrame("");
    addFrame("1");
    addFrame("11");
    addFrame("112");
    addFrame("1122");
    addFrame("11223");
    addFrame("112233");
    addFrame("1122334");
    addFrame("11223344");
    addFrame("112233445");
    addFrame("1122334455");
    addFrame("11223344556");
    addFrame("112233445566");
    addFrame("1122334455667");
    addFrame("11223344556677");
    addFrame("112233445566778");
    addFrame("1122334455667788");
    addFrame("aa");
    addFrame("AA");
    addFrame("aaBB");
    addFrame("AAbb");
    addFrame("AAbbCC");
    addFrame("AAbbCCdd");
    addFrame("AAbbCCddEE");
    addFrame("AAbbCCddEEff");

    auto payloadTest = [&](uint32_t cnt, CanRawFilterGuiInt::AcceptList_t&& list) {
        spyRx.clear();
        spyTx.clear();

        rxCbk(list);
        txCbk(list);

        for (auto& frame : frames) {
            c.txFrameIn(frame);
        }

        REQUIRE(spyRx.count() == 0);
        REQUIRE(spyTx.count() == cnt);

        for (auto& frame : frames) {
            c.rxFrameIn(frame);
        }

        REQUIRE(spyRx.count() == cnt);
        REQUIRE(spyTx.count() == cnt);
    };

    // clang-format off

    payloadTest(25, { 
            I{ ".*", ".*", true },
        });

    // empty payload
    payloadTest(1, { 
            I{ ".*", "^$", true },
            I{ ".*", ".*", false },
        });

    // One byte has always two digits
    payloadTest(0, { 
            I{ ".*", "^1$", true },
            I{ ".*", ".*", false },
        });

                              // 1  2  3  4  5  6  7  8
    std::array<uint8_t, 8> dlc { 4, 4, 3, 3, 3, 3, 2, 2 };
    for(long unsigned int i = 0; i < dlc.size(); ++i) {
        payloadTest(dlc[i], { 
                I{ ".*", "^[0-9,a-f]{" + QString::number((i+1) * 2) + "}$", true },
                I{ ".*", ".*", false },
            });

    }

    payloadTest(8, { 
            I{ ".*", "A", true },
            I{ ".*", ".*", false },
        });

    payloadTest(8, { 
            I{ ".*", "a", true },
            I{ ".*", ".*", false },
        });

    payloadTest(3, { 
            I{ ".*", "CD", true },
            I{ ".*", ".*", false },
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
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
