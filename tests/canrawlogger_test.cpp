#include <QtWidgets/QApplication>
#include <canrawlogger.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QDir>
#include <QSignalSpy>
#include <fakeit.hpp>
#include <QRegularExpression>
#include <QTextStream>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[canrawlogger]")
{
    CanRawLoggerCtx ctx;
    CanRawLogger c(std::move(ctx));

    CHECK(c.mainWidget() == nullptr);
    CHECK(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canrawlogger]")
{
    CanRawLogger c;
    QObject obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[canrawlogger]")
{
    CanRawLogger c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[canrawlogger]")
{
    CanRawLogger c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[canrawlogger]")
{
    CanRawLogger c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[canrawlogger]")
{
    CanRawLogger c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[canrawlogger]")
{
    CanRawLogger c;

    auto props = c.getSupportedProperties();

    CHECK(props.find("name") != props.end());
    CHECK(props.find("dummy") == props.end());
}

TEST_CASE("logging - directories", "[canrawlogger]")
{
    CanRawLogger c;
    QObject obj;
    QString dirName = "dummy";
    QDir dir;

    dir.setPath(dirName);
    dir.removeRecursively();

    CHECK(dir.exists() == false);

    obj.setProperty("directory", dirName);
    c.setConfig(obj);
    c.startSimulation();

    CHECK(dir.exists() == true);

    c.stopSimulation();

    dirName = "/dummy";
    dir.setPath(dirName);

    CHECK(dir.exists() == false);

    obj.setProperty("directory", dirName);
    c.setConfig(obj);
    c.startSimulation();

    CHECK(dir.exists() == false);

    c.stopSimulation();
}

TEST_CASE("logging - filenames", "[canrawlogger]")
{
    CanRawLogger c1, c2, c3;
    QObject obj;
    QString dirName = "filename_test";
    QDir dir;

    dir.setPath(dirName);
    dir.removeRecursively();
    obj.setProperty("directory", dirName);

    c1.setConfig(obj);
    c2.setConfig(obj);
    c3.setConfig(obj);

    c1.startSimulation();
    c2.startSimulation();
    c3.startSimulation();

    auto fileList = dir.entryList({ "*(1).log" });
    CHECK(fileList.size() == 1);

    auto fileName = fileList[0].replace("(1)", "*");
    fileList = dir.entryList({ fileName });
    CHECK(fileList.size() == 3);

    fileName = fileName.replace("*.log", "(2).log");
    fileList = dir.entryList({ fileName });
    CHECK(fileList.size() == 1);

    c1.stopSimulation();
    c2.stopSimulation();
    c3.stopSimulation();
}

static uint32_t loadTraceFile(const QString& filename)
{
    QFileInfo fileNfo(filename);
    QFile traceFile(filename);
    uint32_t msgCnt = 0;

    if (!traceFile.open(QIODevice::ReadOnly)) {
        cds_error("Failed to open file '{}' for reading", filename.toStdString());
        return msgCnt;
    }

    QTextStream in(&traceFile);

    QRegularExpression re(R"(\((\d+\.\d{6})\)\s*\w*\s*([0-9,A-F]{1,8})\s*\[(\d)\]\s*((\s*[0-9,A-F]{2}){0,8}))");
    while (!in.atEnd()) {
        QString line = in.readLine();

        const auto& match = re.match(line);

        if (match.hasMatch()) {
            ++msgCnt;
        }
    }
    traceFile.close();

    return msgCnt;
}

TEST_CASE("logging - send/receive", "[canrawlogger]")
{
    CanRawLogger c;
    QObject obj;
    QCanBusFrame frame;
    QString dirName = "filename_test";
    QDir dir;

    dir.setPath(dirName);
    dir.removeRecursively();
    obj.setProperty("directory", dirName);

    c.setConfig(obj);
    c.startSimulation();

    frame.setFrameId(0x3bc);
    frame.setPayload(QByteArray::fromHex("123"));
    c.frameReceived(frame);

    frame.setFrameId(0x11abc);
    frame.setPayload(QByteArray::fromHex("abcd"));
    c.frameReceived(frame);

    c.frameSent(true, frame);
    c.frameSent(false, frame);

    // list will include .. and .
    auto fileList = dir.entryList({ "*" });
    CHECK(fileList.size() == 3);

    c.stopSimulation();

    auto msgCnt = loadTraceFile(dirName + "/" + fileList[2]);
    CHECK(msgCnt == 3);

    c.frameReceived(frame);
    c.frameReceived(frame);
    c.frameReceived(frame);
    c.frameReceived(frame);
    
    fileList = dir.entryList({ "*" });
    CHECK(fileList.size() == 3);
    msgCnt = loadTraceFile(dirName + "/" + fileList[2]);
    CHECK(msgCnt == 3);
}

TEST_CASE("logging - send/receive, removed file", "[canrawlogger]")
{
    CanRawLogger c;
    QObject obj;
    QCanBusFrame frame;
    QString dirName = "filename_test";
    QDir dir;

    dir.setPath(dirName);
    dir.removeRecursively();
    obj.setProperty("directory", dirName);

    c.setConfig(obj);
    c.startSimulation();

    frame.setFrameId(0x3bc);
    frame.setPayload(QByteArray::fromHex("123"));
    c.frameReceived(frame);

    frame.setFrameId(0x11abc);
    frame.setPayload(QByteArray::fromHex("abcd"));
    c.frameReceived(frame);

    c.frameSent(true, frame);
    c.frameSent(false, frame);

    // list will include .. and .
    auto fileList = dir.entryList({ "*" });
    CHECK(fileList.size() == 3);

    QFile rmFile(dirName + "/" + fileList[2]);
    CHECK(rmFile.remove());

    c.frameReceived(frame);
    c.frameReceived(frame);
    c.frameReceived(frame);

    c.stopSimulation();
}

TEST_CASE("logging - send/receive while stopped", "[canrawlogger]")
{
    CanRawLogger c;
    QObject obj;
    QCanBusFrame frame;
    QString dirName = "filename_test";
    QDir dir;

    dir.setPath(dirName);
    dir.removeRecursively();
    obj.setProperty("directory", dirName);

    c.setConfig(obj);

    c.frameReceived(frame);
    c.frameSent(true, frame);
    c.frameSent(false, frame);

    auto fileList = dir.entryList({ "*" });
    CHECK(fileList.size() == 0);
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
