#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QApplication>
#include <canrawviewmodel.h>
#include <datamodeltypes/canrawdata.h>
#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <catch.hpp>
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
    REQUIRE(canRawViewModel.caption() == "CanRawView");
    REQUIRE(canRawViewModel.name() == "CanRawView");
    REQUIRE(canRawViewModel.resizable() == false);
    REQUIRE(dynamic_cast<CanRawViewModel*>(canRawViewModel.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(canRawViewModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    CanRawData canRawViewDataIn;
    REQUIRE(canRawViewModel.nPorts(PortType::Out) == 0);
    REQUIRE(canRawViewModel.nPorts(PortType::In) == 1);
    REQUIRE(canRawViewModel.nPorts(PortType::None) == 0);
    REQUIRE(canRawViewModel.dataType(PortType::In, 0).id == canRawViewDataIn.type().id);
    REQUIRE(canRawViewModel.dataType(PortType::In, 0).name == canRawViewDataIn.type().name);
}

TEST_CASE("Calling setInData with direction TX will result in frameSent being emitted", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canRawViewDataIn = std::make_shared<CanRawData>(testFrame, Direction::TX, true);
    QSignalSpy frameSentSpy(&canRawViewModel, &CanRawViewModel::frameSent);

    canRawViewModel.setInData(canRawViewDataIn, 0);
    REQUIRE(frameSentSpy.count() == 1);
    REQUIRE(qvariant_cast<QCanBusFrame>(frameSentSpy.takeFirst().at(1)).frameId() == testFrame.frameId());
}

TEST_CASE("Calling setInData with direction RX will result in frameReceived being emitted", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canRawViewDataIn = std::make_shared<CanRawData>(testFrame, Direction::RX, true);
    QSignalSpy frameReceivedSpy(&canRawViewModel, &CanRawViewModel::frameReceived);

    canRawViewModel.setInData(canRawViewDataIn, 0);
    REQUIRE(frameReceivedSpy.count() == 1);
    REQUIRE(qvariant_cast<QCanBusFrame>(frameReceivedSpy.takeFirst().at(0)).frameId() == testFrame.frameId());
}

TEST_CASE("Invalid direction", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canRawViewDataIn = std::make_shared<CanRawData>(testFrame, static_cast<Direction>(11), true);
    QSignalSpy frameSentSpy(&canRawViewModel, &CanRawViewModel::frameSent);

    canRawViewModel.setInData(canRawViewDataIn, 0);
    REQUIRE(frameSentSpy.count() == 0);
}

TEST_CASE("Test save configuration", "[canrawview]")
{
    CanRawViewModel canRawViewModel;
    QJsonObject json = canRawViewModel.save();
    REQUIRE(json.find("name") != json.end());
    REQUIRE(json.find("viewColumns") != json.end());
    REQUIRE(json.find("scrolling") != json.end());
}

bool compareJson(const QJsonObject& patern, const QJsonObject& check)
{
    if (patern.contains("name") != check.contains("name"))
        return false;

    auto paternName = patern["name"].toString();
    auto checkName = check["name"].toString();
    if (paternName.compare(checkName))
        return false;

    if (patern.contains("scrolling") != check.contains("scrolling"))
        return false;
    if (patern["scrolling"].isBool() != check["scrolling"].isBool())
        return false;
    if (patern["scrolling"].toBool() != check["scrolling"].toBool())
        return false;

    if (patern.contains("viewColumns") != check.contains("viewColumns"))
        return false;
    if (patern["viewColumns"].isArray() != check["viewColumns"].isArray())
        return false;

    auto paternArray = patern["viewColumns"].toArray();
    auto checkArray = check["viewColumns"].toArray();
    if (paternArray.size() != checkArray.size())
        return false;

    for (int ii = 0; ii < paternArray.size(); ++ii) {
        if (paternArray[ii].isObject() != checkArray[ii].isObject())
            return false;
        auto paternObj = paternArray[ii].toObject();
        auto checkObj = checkArray[ii].toObject();
        if ((paternObj.count() != checkObj.count()) || (paternObj.contains("name") != checkObj.contains("name"))
            || (paternObj.contains("vIdx") != checkObj.contains("vIdx"))
            || (paternObj.contains("width") != checkObj.contains("width")))
            return false;
        if ((paternObj["name"].isString() != checkObj["name"].isString())
            || (paternObj["vIdx"].isDouble() != checkObj["vIdx"].isDouble())
            || (paternObj["width"].isDouble() != checkObj["width"].isDouble()))
            return false;

        auto paternObjName = paternObj["name"].toString();
        auto checkObjName = checkObj["name"].toString();
        if (paternObjName.compare(checkObjName) || (paternObj["vIdx"].toInt() != checkObj["vIdx"].toInt())
            || (paternObj["width"].toInt() != checkObj["width"].toInt()))
            return false;
    }
    return true;
}

TEST_CASE("setCaption", "[canrawview]")
{
    CanRawViewModel canRawViewModel;

    REQUIRE_NOTHROW(canRawViewModel.setCaption("foobar"));
}

TEST_CASE("Test restore - no name", "[canrawview]")
{
    QJsonObject jsonObject;
    jsonObject["name"] = "aaa";

    CanRawViewModel canRawViewModel;

    REQUIRE_NOTHROW(canRawViewModel.restore(jsonObject));
}

TEST_CASE("Test restore - no caption", "[canrawview]")
{
    QJsonObject jsonObject;
    jsonObject["caption"] = "aaa";

    CanRawViewModel canRawViewModel;

    REQUIRE_NOTHROW(canRawViewModel.restore(jsonObject));
}

TEST_CASE("Test restore configuration", "[canrawview]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawviewconfig.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();

    CanRawViewModel canRawViewModel;
    canRawViewModel.restore(jsonObject);
    QJsonObject json = canRawViewModel.save();

    REQUIRE(compareJson(jsonObject, json) == true);
}

TEST_CASE("Test restore configuration - lack of column", "[canrawview]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawviewconfig_columnluck.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();

    CanRawViewModel canRawViewModel;
    canRawViewModel.restore(jsonObject);
    QJsonObject json = canRawViewModel.save();

    REQUIRE(compareJson(jsonObject, json) == false);
}

TEST_CASE("Test restore configuration - sorting format incorrect", "[canrawview]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawviewconfig_sortingformat.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();

    CanRawViewModel canRawViewModel;
    canRawViewModel.restore(jsonObject);
    QJsonObject json = canRawViewModel.save();

    REQUIRE(compareJson(jsonObject, json) == false);
}

TEST_CASE("Test restore configuration - visual index incorrect", "[canrawview]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawviewconfig_visualindexproblem.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();

    CanRawViewModel canRawViewModel;
    canRawViewModel.restore(jsonObject);
    QJsonObject json = canRawViewModel.save();

    REQUIRE(compareJson(jsonObject, json) == false);
}

TEST_CASE("Test restore configuration - width incorrect", "[canrawview]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawviewconfig_widthproblem.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();

    CanRawViewModel canRawViewModel;
    canRawViewModel.restore(jsonObject);
    QJsonObject json = canRawViewModel.save();

    REQUIRE(compareJson(jsonObject, json) == false);
}

TEST_CASE("Getters", "[canrawview]")
{
    CanRawViewModel crvModel;

    REQUIRE(crvModel.painterDelegate() != nullptr);
    REQUIRE(crvModel.outData(0));
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit configfiles");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
