
#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "candeviceinterface.hpp"
#include "canfactory.hpp"

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

int main(int argc, char* argv[]) { return Catch::Session().run(argc, argv); }
