#include <QtWidgets/QApplication>
#include <cansignalencoder.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QJsonDocument>
#include <QSignalSpy>
#include <bcastmsgs.h>
#include <cansignaldata.h>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignalencoder]")
{
    CanSignalEncoder c;

    REQUIRE(c.mainWidget() == nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignalencoder]")
{
    CanSignalEncoder c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);

    auto w = c.getQConfig();
    REQUIRE(w->property("color").isValid());
}

TEST_CASE("setConfig - json", "[cansignalencoder]")
{
    CanSignalEncoder c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);

    auto w = c.getQConfig();
    REQUIRE(w->property("color").isValid());
}

TEST_CASE("getConfig", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignalencoder]")
{
    CanSignalEncoder c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "CAN database");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);
}

QString testJson = R"(
{
    "msgSettings": [
        {
            "cycle": "100",
            "id": "3",
            "initVal": ""
        },
        {
            "cycle": "200",
            "id": "e",
            "initVal": "1122334455667788"
        },
        {
            "cycle": "500",
            "id": "45",
            "initVal": "aabbccddeeff0099"
        },
        {
            "cycle": "2000",
            "id": "6d",
            "initVal": "AABBCCDD"
        }
    ],
    "name": "CanSignalData"
}
)";

TEST_CASE("send preconfigured", "[cansignalencoder]")
{
    CanSignalData data;
    CanSignalEncoder c;
    QJsonObject obj = QJsonDocument::fromJson(testJson.toUtf8()).object();
    QThread th;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.moveToThread(&th);
    c.moveToThread(&th);

    QObject::connect(&th, &QThread::started, &c, &CanSignalEncoder::startSimulation);
    QObject::connect(&th, &QThread::finished, &c, &CanSignalEncoder::stopSimulation);
    QObject::connect(&th, &QThread::started, &data, &CanSignalData::startSimulation);
    QObject::connect(&th, &QThread::finished, &data, &CanSignalData::stopSimulation);

    th.start();

    // Mesages with defined cycle should not cause message being sent instantly
    c.rcvSignal("0x06d_MC_SBW_RQ_SCCM", 15);
    c.rcvSignal("0x06d_MC_SBW_RQ_SCCM", 15);
    c.rcvSignal("0x06d_MC_SBW_RQ_SCCM", 15);
    c.rcvSignal("0x06d_MC_SBW_RQ_SCCM", 15);

    QThread::sleep(1);

    th.quit();
    th.wait();

    REQUIRE(sigSndSpy.count() > 0);

    std::map<uint32_t, int> sigCnt;
    for (auto&& item : sigSndSpy) {
        QCanBusFrame frame = qvariant_cast<QCanBusFrame>(item.at(0));

        sigCnt[frame.frameId()] += 1;
    }

    REQUIRE(sigCnt[0x03] > 0);
    REQUIRE(sigCnt[0x03] <= 10);
    REQUIRE(sigCnt[0x0e] > 0);
    REQUIRE(sigCnt[0x0e] <= 5);
    REQUIRE(sigCnt[0x45] > 0);
    REQUIRE(sigCnt[0x45] <= 2);
    REQUIRE(sigCnt[0x6d] == 0);
}

TEST_CASE("recive signals", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    // Send before starting
    c.rcvSignal("0x348_GTW_statusChecksum", 100);

    data.startSimulation();
    c.startSimulation();

    // Correct Signal
    c.rcvSignal("0x348_GTW_statusChecksum", 100);
    c.rcvSignal("0x00e_StW_AnglHP_Sens_Stat", 1);

    // No such msg ID
    c.rcvSignal("0x666_GTW_statusChecksum", 100);
    // msg ID exists but no such signal
    c.rcvSignal("0x348_No_Such_Message", 100);

    c.rcvSignal("0x666", 100);
    c.rcvSignal("0x348", 100);

    c.rcvSignal("0x666_", 100);
    c.rcvSignal("0x348_", 100);

    c.rcvSignal("ZZZ_ZZZ", 100);
    c.rcvSignal("ZZZ_", 100);
    c.rcvSignal("ZZZ", 100);
    c.rcvSignal("", 100);

    REQUIRE(sigSndSpy.count() == 2);
}

TEST_CASE("factor 0", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x488_DAS_steeringControlType", 100);
    c.rcvSignal("0x488_DAS_steeringControlType", 100);
    c.rcvSignal("0x488_DAS_steeringControlType", 100);

    REQUIRE(sigSndSpy.count() == 0);
}

TEST_CASE("Signal too big", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x488_DAS_steeringControlChecksum", 100);
    c.rcvSignal("0x488_DAS_steeringControlChecksum", 100);
    c.rcvSignal("0x488_DAS_steeringControlChecksum", 100);

    REQUIRE(sigSndSpy.count() == 0);
}

QString testJson2 = R"(
{
    "msgSettings": [
        {
            "cycle": "500",
            "id": "6d",
            "initVal": "AABBCCDD"
        }
    ],
    "name": "CanSignalData"
}
)";

TEST_CASE("reset init value", "[cansignalencoder]")
{
    CanSignalData data;
    CanSignalEncoder c;
    QJsonObject obj = QJsonDocument::fromJson(testJson2.toUtf8()).object();
    QThread th;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.moveToThread(&th);
    c.moveToThread(&th);

    QObject::connect(&th, &QThread::started, &c, &CanSignalEncoder::startSimulation);
    QObject::connect(&th, &QThread::finished, &c, &CanSignalEncoder::stopSimulation);
    QObject::connect(&th, &QThread::started, &data, &CanSignalData::startSimulation);
    QObject::connect(&th, &QThread::finished, &data, &CanSignalData::stopSimulation);

    th.start();

    auto rcvSignal = [&](const QString& sig, const QVariant& val) {
        QMetaObject::invokeMethod(&c, "rcvSignal", Qt::AutoConnection, Q_ARG(QString, sig), Q_ARG(QVariant, val));
    };

    rcvSignal("0x06d_MC_SBW_RQ_SCCM", 0);
    rcvSignal("0x06d_MC_SBW_RQ_SCCM", 0);
    rcvSignal("0x06d_MC_SBW_RQ_SCCM", 0);
    rcvSignal("0x06d_MC_SBW_RQ_SCCM", 0);

    QThread::sleep(1);

    th.quit();
    th.wait();

    REQUIRE(sigSndSpy.count() > 0);
    REQUIRE(sigSndSpy.count() <= 2);

    QCanBusFrame frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));
    sigSndSpy.clear();

    REQUIRE(frame.frameId() == 0x6d);
    REQUIRE(frame.payload() == QByteArray::fromHex(QByteArray::number(0xaabb0cdd, 16)));

    th.start();

    QThread::sleep(1);

    th.quit();
    th.wait();

    REQUIRE(sigSndSpy.count() > 0);
    REQUIRE(sigSndSpy.count() <= 2);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));
    sigSndSpy.clear();

    REQUIRE(frame.frameId() == 0x6d);
    REQUIRE(frame.payload() == QByteArray::fromHex(QByteArray::number(0xaabbccdd, 16)));
}

TEST_CASE("UnsignedSignals_LE", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x1000_Test01", 1);
    c.rcvSignal("0x1000_Test02", 1);
    c.rcvSignal("0x1000_Test03", 1);
    c.rcvSignal("0x1000_Test04", 1);
    c.rcvSignal("0x1000_Test05", 1);
    c.rcvSignal("0x1000_Test06", 1);
    c.rcvSignal("0x1000_Test07", 1);
    c.rcvSignal("0x1000_Test08", 1);
    c.rcvSignal("0x1000_Test09", 1);
    c.rcvSignal("0x1000_Test10", 1);
    c.rcvSignal("0x1000_Test11", 1);

    REQUIRE(sigSndSpy.count() == 11);

    QCanBusFrame frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1000);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  0000 0000 1000 0000 0010 0000 0001 0000
    //  0001 0000 0010 0000 1000 0100 0100 1011

    REQUIRE(frame.payload()[0] == (char)0x4b);
    REQUIRE(frame.payload()[1] == (char)0x84);
    REQUIRE(frame.payload()[2] == (char)0x20);
    REQUIRE(frame.payload()[3] == (char)0x10);
    REQUIRE(frame.payload()[4] == (char)0x10);
    REQUIRE(frame.payload()[5] == (char)0x20);
    REQUIRE(frame.payload()[6] == (char)0x80);
    REQUIRE(frame.payload()[7] == (char)0x00);

    sigSndSpy.clear();

    c.rcvSignal("0x1000_Test01", 1);
    c.rcvSignal("0x1000_Test02", 2);
    c.rcvSignal("0x1000_Test03", 4);
    c.rcvSignal("0x1000_Test04", 8);
    c.rcvSignal("0x1000_Test05", 16);
    c.rcvSignal("0x1000_Test06", 32);
    c.rcvSignal("0x1000_Test07", 64);
    c.rcvSignal("0x1000_Test08", 128);
    c.rcvSignal("0x1000_Test09", 256);
    c.rcvSignal("0x1000_Test10", 512);
    c.rcvSignal("0x1000_Test11", 256);

    REQUIRE(sigSndSpy.count() == 11);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1000);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1000 0000 0100 0000 0001 0000 0000 1000
    //  0000 1000 0001 0000 0100 0010 0010 0101

    REQUIRE(frame.payload()[0] == (char)0x25);
    REQUIRE(frame.payload()[1] == (char)0x42);
    REQUIRE(frame.payload()[2] == (char)0x10);
    REQUIRE(frame.payload()[3] == (char)0x08);
    REQUIRE(frame.payload()[4] == (char)0x08);
    REQUIRE(frame.payload()[5] == (char)0x10);
    REQUIRE(frame.payload()[6] == (char)0x40);
    REQUIRE(frame.payload()[7] == (char)0x80);
}

TEST_CASE("SignedSignals_LE", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x1001_Test02", 1);
    c.rcvSignal("0x1001_Test03", 3);
    c.rcvSignal("0x1001_Test04", 7);
    c.rcvSignal("0x1001_Test05", 15);
    c.rcvSignal("0x1001_Test06", 31);
    c.rcvSignal("0x1001_Test07", 63);
    c.rcvSignal("0x1001_Test08", 127);
    c.rcvSignal("0x1001_Test09", 255);
    c.rcvSignal("0x1001_Test10", 511);
    c.rcvSignal("0x1001_Test11", 511);

    REQUIRE(sigSndSpy.count() == 10);

    QCanBusFrame frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1001);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  0111 1111 1101 1111 1111 0111 1111 1011
    //  1111 1011 1111 0111 1101 1110 1110 1101

    REQUIRE(frame.payload()[0] == (char)0xed);
    REQUIRE(frame.payload()[1] == (char)0xde);
    REQUIRE(frame.payload()[2] == (char)0xf7);
    REQUIRE(frame.payload()[3] == (char)0xfb);
    REQUIRE(frame.payload()[4] == (char)0xfb);
    REQUIRE(frame.payload()[5] == (char)0xf7);
    REQUIRE(frame.payload()[6] == (char)0xdf);
    REQUIRE(frame.payload()[7] == (char)0x7f);

    sigSndSpy.clear();

    c.rcvSignal("0x1001_Test02", -1);
    c.rcvSignal("0x1001_Test03", -1);
    c.rcvSignal("0x1001_Test04", -1);
    c.rcvSignal("0x1001_Test05", -1);
    c.rcvSignal("0x1001_Test06", -1);
    c.rcvSignal("0x1001_Test07", -1);
    c.rcvSignal("0x1001_Test08", -1);
    c.rcvSignal("0x1001_Test09", -1);
    c.rcvSignal("0x1001_Test10", -1);
    c.rcvSignal("0x1001_Test11", -1);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1001);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1111 1111 1111 1111 1111 1111 1111 1111
    //  1111 1111 1111 1111 1111 1111 1111 1111

    REQUIRE(frame.payload()[0] == (char)0xff);
    REQUIRE(frame.payload()[1] == (char)0xff);
    REQUIRE(frame.payload()[2] == (char)0xff);
    REQUIRE(frame.payload()[3] == (char)0xff);
    REQUIRE(frame.payload()[4] == (char)0xff);
    REQUIRE(frame.payload()[5] == (char)0xff);
    REQUIRE(frame.payload()[6] == (char)0xff);
    REQUIRE(frame.payload()[7] == (char)0xff);

    sigSndSpy.clear();

    c.rcvSignal("0x1001_Test02", -1);
    c.rcvSignal("0x1001_Test03", -3);
    c.rcvSignal("0x1001_Test04", -7);
    c.rcvSignal("0x1001_Test05", -15);
    c.rcvSignal("0x1001_Test06", -31);
    c.rcvSignal("0x1001_Test07", -63);
    c.rcvSignal("0x1001_Test08", -127);
    c.rcvSignal("0x1001_Test09", -255);
    c.rcvSignal("0x1001_Test10", -511);
    c.rcvSignal("0x1001_Test11", -511);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1001);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1000 0000 0110 0000 0001 1000 0000 1100
    //  0000 1100 0001 1000 0110 0011 0011 0111

    REQUIRE(frame.payload()[0] == (char)0x37);
    REQUIRE(frame.payload()[1] == (char)0x63);
    REQUIRE(frame.payload()[2] == (char)0x18);
    REQUIRE(frame.payload()[3] == (char)0x0c);
    REQUIRE(frame.payload()[4] == (char)0x0c);
    REQUIRE(frame.payload()[5] == (char)0x18);
    REQUIRE(frame.payload()[6] == (char)0x60);
    REQUIRE(frame.payload()[7] == (char)0x80);

    sigSndSpy.clear();

    c.rcvSignal("0x1001_Test02", -2);
    c.rcvSignal("0x1001_Test03", -4);
    c.rcvSignal("0x1001_Test04", -8);
    c.rcvSignal("0x1001_Test05", -16);
    c.rcvSignal("0x1001_Test06", -32);
    c.rcvSignal("0x1001_Test07", -64);
    c.rcvSignal("0x1001_Test08", -128);
    c.rcvSignal("0x1001_Test09", -256);
    c.rcvSignal("0x1001_Test10", -512);
    c.rcvSignal("0x1001_Test11", -512);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1001);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1000 0000 0010 0000 0000 1000 0000 0100
    //  0000 0100 0000 1000 0010 0001 0001 0010

    REQUIRE(frame.payload()[0] == (char)0x12);
    REQUIRE(frame.payload()[1] == (char)0x21);
    REQUIRE(frame.payload()[2] == (char)0x08);
    REQUIRE(frame.payload()[3] == (char)0x04);
    REQUIRE(frame.payload()[4] == (char)0x04);
    REQUIRE(frame.payload()[5] == (char)0x08);
    REQUIRE(frame.payload()[6] == (char)0x20);
    REQUIRE(frame.payload()[7] == (char)0x80);
}

TEST_CASE("UnsignedSignals_BE", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x1002_Test01", 1);
    c.rcvSignal("0x1002_Test02", 1);
    c.rcvSignal("0x1002_Test03", 1);
    c.rcvSignal("0x1002_Test04", 1);
    c.rcvSignal("0x1002_Test05", 1);
    c.rcvSignal("0x1002_Test06", 1);
    c.rcvSignal("0x1002_Test07", 1);
    c.rcvSignal("0x1002_Test08", 1);
    c.rcvSignal("0x1002_Test09", 1);
    c.rcvSignal("0x1002_Test10", 1);
    c.rcvSignal("0x1002_Test11", 1);

    REQUIRE(sigSndSpy.count() == 11);

    QCanBusFrame frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1002);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1000 0000 0100 0000 0001 0000 0000 1000
    //  0000 1000 0001 0000 0100 0010 0010 0101

    REQUIRE(frame.payload()[0] == (char)0x25);
    REQUIRE(frame.payload()[1] == (char)0x42);
    REQUIRE(frame.payload()[2] == (char)0x10);
    REQUIRE(frame.payload()[3] == (char)0x08);
    REQUIRE(frame.payload()[4] == (char)0x08);
    REQUIRE(frame.payload()[5] == (char)0x10);
    REQUIRE(frame.payload()[6] == (char)0x40);
    REQUIRE(frame.payload()[7] == (char)0x80);

    sigSndSpy.clear();

    c.rcvSignal("0x1002_Test01", 1);
    c.rcvSignal("0x1002_Test02", 2);
    c.rcvSignal("0x1002_Test03", 4);
    c.rcvSignal("0x1002_Test04", 8);
    c.rcvSignal("0x1002_Test05", 16);
    c.rcvSignal("0x1002_Test06", 32);
    c.rcvSignal("0x1002_Test07", 64);
    c.rcvSignal("0x1002_Test08", 128);
    c.rcvSignal("0x1002_Test09", 256);
    c.rcvSignal("0x1002_Test10", 512);
    c.rcvSignal("0x1002_Test11", 256);

    REQUIRE(sigSndSpy.count() == 11);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1002);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  0000 0000 1000 0000 0010 0000 0001 0000
    //  0001 0000 0010 0000 1000 0100 0100 1011

    REQUIRE(frame.payload()[0] == (char)0x4b);
    REQUIRE(frame.payload()[1] == (char)0x84);
    REQUIRE(frame.payload()[2] == (char)0x20);
    REQUIRE(frame.payload()[3] == (char)0x10);
    REQUIRE(frame.payload()[4] == (char)0x10);
    REQUIRE(frame.payload()[5] == (char)0x20);
    REQUIRE(frame.payload()[6] == (char)0x80);
    REQUIRE(frame.payload()[7] == (char)0x00);
}

TEST_CASE("SignedSignals_BE", "[cansignalencoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalEncoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.startSimulation();
    c.startSimulation();

    c.rcvSignal("0x1003_Test02", 1);
    c.rcvSignal("0x1003_Test03", 3);
    c.rcvSignal("0x1003_Test04", 7);
    c.rcvSignal("0x1003_Test05", 15);
    c.rcvSignal("0x1003_Test06", 31);
    c.rcvSignal("0x1003_Test07", 63);
    c.rcvSignal("0x1003_Test08", 127);
    c.rcvSignal("0x1003_Test09", 255);
    c.rcvSignal("0x1003_Test10", 511);
    c.rcvSignal("0x1003_Test11", 511);

    REQUIRE(sigSndSpy.count() == 10);

    QCanBusFrame frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1003);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1111 1111 1011 1111 1110 1111 1111 0111
    //  1111 0111 1110 1111 1011 1101 1101 1010

    REQUIRE(frame.payload()[0] == (char)0xda);
    REQUIRE(frame.payload()[1] == (char)0xbd);
    REQUIRE(frame.payload()[2] == (char)0xef);
    REQUIRE(frame.payload()[3] == (char)0xf7);
    REQUIRE(frame.payload()[4] == (char)0xf7);
    REQUIRE(frame.payload()[5] == (char)0xef);
    REQUIRE(frame.payload()[6] == (char)0xbf);
    REQUIRE(frame.payload()[7] == (char)0xff);

    sigSndSpy.clear();

    c.rcvSignal("0x1003_Test02", -1);
    c.rcvSignal("0x1003_Test03", -1);
    c.rcvSignal("0x1003_Test04", -1);
    c.rcvSignal("0x1003_Test05", -1);
    c.rcvSignal("0x1003_Test06", -1);
    c.rcvSignal("0x1003_Test07", -1);
    c.rcvSignal("0x1003_Test08", -1);
    c.rcvSignal("0x1003_Test09", -1);
    c.rcvSignal("0x1003_Test10", -1);
    c.rcvSignal("0x1003_Test11", -1);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1003);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1111 1111 1111 1111 1111 1111 1111 1111
    //  1111 1111 1111 1111 1111 1111 1111 1111

    REQUIRE(frame.payload()[0] == (char)0xff);
    REQUIRE(frame.payload()[1] == (char)0xff);
    REQUIRE(frame.payload()[2] == (char)0xff);
    REQUIRE(frame.payload()[3] == (char)0xff);
    REQUIRE(frame.payload()[4] == (char)0xff);
    REQUIRE(frame.payload()[5] == (char)0xff);
    REQUIRE(frame.payload()[6] == (char)0xff);
    REQUIRE(frame.payload()[7] == (char)0xff);

    sigSndSpy.clear();

    c.rcvSignal("0x1003_Test02", -1);
    c.rcvSignal("0x1003_Test03", -3);
    c.rcvSignal("0x1003_Test04", -7);
    c.rcvSignal("0x1003_Test05", -15);
    c.rcvSignal("0x1003_Test06", -31);
    c.rcvSignal("0x1003_Test07", -63);
    c.rcvSignal("0x1003_Test08", -127);
    c.rcvSignal("0x1003_Test09", -255);
    c.rcvSignal("0x1003_Test10", -511);
    c.rcvSignal("0x1003_Test11", -511);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1003);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  1000 0000 0110 0000 0001 1000 0000 1100
    //  0000 1100 0001 1000 0110 0011 0011 0111

    REQUIRE(frame.payload()[0] == (char)0x37);
    REQUIRE(frame.payload()[1] == (char)0x63);
    REQUIRE(frame.payload()[2] == (char)0x18);
    REQUIRE(frame.payload()[3] == (char)0x0c);
    REQUIRE(frame.payload()[4] == (char)0x0c);
    REQUIRE(frame.payload()[5] == (char)0x18);
    REQUIRE(frame.payload()[6] == (char)0x60);
    REQUIRE(frame.payload()[7] == (char)0x80);

    sigSndSpy.clear();

    c.rcvSignal("0x1003_Test02", -2);
    c.rcvSignal("0x1003_Test03", -4);
    c.rcvSignal("0x1003_Test04", -8);
    c.rcvSignal("0x1003_Test05", -16);
    c.rcvSignal("0x1003_Test06", -32);
    c.rcvSignal("0x1003_Test07", -64);
    c.rcvSignal("0x1003_Test08", -128);
    c.rcvSignal("0x1003_Test09", -256);
    c.rcvSignal("0x1003_Test10", -512);
    c.rcvSignal("0x1003_Test11", -512);

    REQUIRE(sigSndSpy.count() == 10);

    frame = qvariant_cast<QCanBusFrame>(sigSndSpy.takeLast().at(0));

    REQUIRE(frame.frameId() == 0x1003);
    REQUIRE(frame.hasExtendedFrameFormat() == true);

    //  0000 0000 0100 0000 0001 0000 0000 1000
    //  0000 1000 0001 0000 0100 0010 0010 0101

    REQUIRE(frame.payload()[0] == (char)0x25);
    REQUIRE(frame.payload()[1] == (char)0x42);
    REQUIRE(frame.payload()[2] == (char)0x10);
    REQUIRE(frame.payload()[3] == (char)0x08);
    REQUIRE(frame.payload()[4] == (char)0x08);
    REQUIRE(frame.payload()[5] == (char)0x10);
    REQUIRE(frame.payload()[6] == (char)0x40);
    REQUIRE(frame.payload()[7] == (char)0x00);
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
