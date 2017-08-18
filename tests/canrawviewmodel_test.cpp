#include <canrawview/canrawviewmodel.h>
#include <QtWidgets/QApplication>

#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

//#include <QSignalSpy>
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
//Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("dummy", "[candevice]")
{
    using namespace fakeit;
    CanRawViewModel canRawViewModel;
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    QApplication a(argc, argv);  // QApplication must exist when contructing QWidgets TODO check QTest
//    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    return Catch::Session().run(argc, argv);
}
