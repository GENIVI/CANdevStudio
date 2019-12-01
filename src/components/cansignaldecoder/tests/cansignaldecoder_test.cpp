#include <QtWidgets/QApplication>
#include <cansignaldecoder.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <cansignaldata.h>
#include <catch.hpp>
#include <datamodeltypes/datadirection.h>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignaldecoder]")
{
    CanSignalDecoder c;

    REQUIRE(c.mainWidget() == nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignaldecoder]")
{
    CanSignalDecoder c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);

    auto w = c.getQConfig();
    REQUIRE(w->property("color").isValid());
}

TEST_CASE("setConfig - json", "[cansignaldecoder]")
{
    CanSignalDecoder c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);

    auto w = c.getQConfig();
    REQUIRE(w->property("color").isValid());
}

TEST_CASE("getConfig", "[cansignaldecoder]")
{
    CanSignalDecoder c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignaldecoder]")
{
    CanSignalDecoder c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignaldecoder]")
{
    CanSignalDecoder c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignaldecoder]")
{
    CanSignalDecoder c;

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

TEST_CASE("UnsignedSignals_LE", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("4b84201010208000");

    QCanBusFrame frame;
    frame.setFrameId(0x1000);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 11);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1000_Test01");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1000_Test02");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1000_Test03");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1000_Test04");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1000_Test05");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1000_Test06");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1000_Test07");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1000_Test08");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1000_Test09");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1000_Test10");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(10).at(0).toString() == "0x1000_Test11");
    REQUIRE(sigSndSpy.at(10).at(1).toInt() == 1);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("2542100808104080");

    frame.setFrameId(0x1000);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 11);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1000_Test01");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1000_Test02");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 2);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1000_Test03");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 4);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1000_Test04");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 8);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1000_Test05");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 16);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1000_Test06");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 32);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1000_Test07");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 64);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1000_Test08");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 128);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1000_Test09");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 256);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1000_Test10");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 512);
    REQUIRE(sigSndSpy.at(10).at(0).toString() == "0x1000_Test11");
    REQUIRE(sigSndSpy.at(10).at(1).toInt() == 256);
}

TEST_CASE("SignedSignals_LE", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("eddef7fbfbf7df7f");

    QCanBusFrame frame;
    frame.setFrameId(0x1001);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1001_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1001_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 3);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1001_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 7);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1001_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 15);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1001_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 31);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1001_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 63);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1001_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 127);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1001_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 255);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1001_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 511);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1001_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 511);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("ffffffffffffffff");

    frame.setFrameId(0x1001);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1001_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1001_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1001_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1001_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1001_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1001_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1001_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1001_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1001_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1001_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -1);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("3763180c0c186080");

    frame.setFrameId(0x1001);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1001_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1001_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -3);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1001_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -7);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1001_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -15);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1001_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -31);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1001_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -63);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1001_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -127);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1001_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -255);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1001_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -511);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1001_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -511);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("1221080404082080");

    frame.setFrameId(0x1001);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1001_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -2);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1001_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -4);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1001_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -8);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1001_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -16);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1001_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -32);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1001_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -64);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1001_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -128);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1001_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -256);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1001_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -512);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1001_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -512);
}

TEST_CASE("UnsignedSignals_BE", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("2542100808104080");

    QCanBusFrame frame;
    frame.setFrameId(0x1002);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 11);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1002_Test01");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1002_Test02");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1002_Test03");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1002_Test04");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1002_Test05");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1002_Test06");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1002_Test07");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1002_Test08");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1002_Test09");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1002_Test10");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(10).at(0).toString() == "0x1002_Test11");
    REQUIRE(sigSndSpy.at(10).at(1).toInt() == 1);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("4b84201010208000");

    frame.setFrameId(0x1002);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 11);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1002_Test01");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1002_Test02");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 2);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1002_Test03");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 4);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1002_Test04");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 8);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1002_Test05");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 16);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1002_Test06");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 32);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1002_Test07");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 64);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1002_Test08");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 128);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1002_Test09");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 256);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1002_Test10");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 512);
    REQUIRE(sigSndSpy.at(10).at(0).toString() == "0x1002_Test11");
    REQUIRE(sigSndSpy.at(10).at(1).toInt() == 256);
}

TEST_CASE("SignedSignals_BE", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("dabdeff7f7efbfff");

    QCanBusFrame frame;
    frame.setFrameId(0x1003);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1003_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == 1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1003_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == 3);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1003_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == 7);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1003_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == 15);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1003_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == 31);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1003_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == 63);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1003_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == 127);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1003_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == 255);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1003_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == 511);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1003_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == 511);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("ffffffffffffffff");

    frame.setFrameId(0x1003);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1003_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1003_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1003_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1003_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1003_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1003_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1003_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1003_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1003_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1003_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -1);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("3763180c0c186080");

    frame.setFrameId(0x1003);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1003_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -1);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1003_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -3);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1003_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -7);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1003_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -15);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1003_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -31);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1003_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -63);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1003_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -127);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1003_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -255);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1003_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -511);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1003_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -511);

    data.stopSimulation();
    c.stopSimulation();
    data.startSimulation();
    c.startSimulation();
    sigSndSpy.clear();

    p = QByteArray::fromHex("2542100808104000");

    frame.setFrameId(0x1003);
    frame.setPayload(p);

    c.rcvFrame(frame, Direction::RX, true);

    REQUIRE(sigSndSpy.count() == 10);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x1003_Test02");
    REQUIRE(sigSndSpy.at(0).at(1).toInt() == -2);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x1003_Test03");
    REQUIRE(sigSndSpy.at(1).at(1).toInt() == -4);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x1003_Test04");
    REQUIRE(sigSndSpy.at(2).at(1).toInt() == -8);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x1003_Test05");
    REQUIRE(sigSndSpy.at(3).at(1).toInt() == -16);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x1003_Test06");
    REQUIRE(sigSndSpy.at(4).at(1).toInt() == -32);
    REQUIRE(sigSndSpy.at(5).at(0).toString() == "0x1003_Test07");
    REQUIRE(sigSndSpy.at(5).at(1).toInt() == -64);
    REQUIRE(sigSndSpy.at(6).at(0).toString() == "0x1003_Test08");
    REQUIRE(sigSndSpy.at(6).at(1).toInt() == -128);
    REQUIRE(sigSndSpy.at(7).at(0).toString() == "0x1003_Test09");
    REQUIRE(sigSndSpy.at(7).at(1).toInt() == -256);
    REQUIRE(sigSndSpy.at(8).at(0).toString() == "0x1003_Test10");
    REQUIRE(sigSndSpy.at(8).at(1).toInt() == -512);
    REQUIRE(sigSndSpy.at(9).at(0).toString() == "0x1003_Test11");
    REQUIRE(sigSndSpy.at(9).at(1).toInt() == -512);
}

TEST_CASE("Signal too big", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("00000000");
    QCanBusFrame f;
    f.setPayload(p);
    f.setFrameId(0x488);

    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);

    REQUIRE(sigSndSpy.count() == 0);
}

TEST_CASE("Signal caching", "[cansignaldecoder]")
{
    CanSignalData data;
    QJsonObject obj;
    CanSignalDecoder c;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalDecoder::simBcastRcv);
    QObject::connect(&c, &CanSignalDecoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalDecoder::sndSignal);

    data.startSimulation();
    c.startSimulation();

    QByteArray p = QByteArray::fromHex("1122334455667788");
    QCanBusFrame f;
    f.setPayload(p);
    f.setFrameId(0x3);

    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);

    // change one signal only
    p = QByteArray::fromHex("2122334455667788");
    f.setPayload(p);

    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);
    c.rcvFrame(f, Direction::TX, true);

    REQUIRE(sigSndSpy.count() == 5);

    REQUIRE(sigSndSpy.at(0).at(0).toString() == "0x003_CRC_STW_ANGL_STAT");
    REQUIRE(sigSndSpy.at(0).at(1).type() == QVariant::Double);
    REQUIRE(sigSndSpy.at(1).at(0).toString() == "0x003_MC_STW_ANGL_STAT");
    REQUIRE(sigSndSpy.at(1).at(1).type() == QVariant::Double);
    REQUIRE(sigSndSpy.at(2).at(0).toString() == "0x003_StW_Angl");
    REQUIRE(sigSndSpy.at(2).at(1).type() == QVariant::Double);
    REQUIRE(sigSndSpy.at(3).at(0).toString() == "0x003_StW_AnglSens_Id");
    REQUIRE((QMetaType::Type)sigSndSpy.at(3).at(1).type() == QMetaType::Long);
    REQUIRE(sigSndSpy.at(4).at(0).toString() == "0x003_StW_Angl");
    REQUIRE(sigSndSpy.at(4).at(1).type() == QVariant::Double);
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
