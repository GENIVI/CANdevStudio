
#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "candeviceinterface.hpp"
#include "canfactory.hpp"

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Initialization failed", "[candevice]")
{
    using namespace fakeit;
    fakeit::Mock<CanFactoryInterface> factoryMock;
    fakeit::When(Method(factoryMock, create)).Return(nullptr);
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == false);
}

TEST_CASE("Initialization succedded", "[candevice]")
{
    using namespace fakeit;
    fakeit::Mock<CanFactoryInterface> factoryMock;
    fakeit::Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, framesWritten)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, framesReceived));
    Fake(Method(deviceMock, errorOccurred));

    fakeit::When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == true);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    return Catch::Session().run(argc, argv);
}
