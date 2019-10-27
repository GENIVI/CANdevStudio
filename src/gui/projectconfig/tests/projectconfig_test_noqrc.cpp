#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <candevice.h>
#include <catch.hpp>
#include <fakeit.hpp>
#include <log.h>
#include <projectconfigvalidator.h>
#include <propertyeditordialog.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Validation - failed to load schema", "[projectconfig]")
{
    REQUIRE(ProjectConfigValidator::validateConfiguration({}) == false);
}

TEST_CASE("PropertyEditorDialog", "[projectconfig]")
{
    CanDevice obj;
    PropertyEditorDialog dialog("title", *obj.getQConfig().get());

    auto props = dialog.properties();

    REQUIRE(props->property("name").isValid());
    REQUIRE(props->property("backend").isValid());
    REQUIRE(props->property("interface").isValid());
    REQUIRE(props->property("unsupported").isValid() == false);
}

TEST_CASE("PropertyEditorDialog no exposed props", "[projectconfig]")
{
    QWidget obj;
    PropertyEditorDialog dialog("title", obj);

    auto props = dialog.properties();

    REQUIRE(props->property("name").isValid() == false);
    REQUIRE(props->property("backend").isValid() == false);
    REQUIRE(props->property("interface").isValid() == false);
    REQUIRE(props->property("unsupported").isValid() == false);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}
