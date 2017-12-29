#define CATCH_CONFIG_RUNNER
#include <projectconfigvalidator.h>
#include <fakeit.hpp>
#include <log.h>
#include <propertyeditordialog.h>
#include <candevice.h>
#include <QApplication>

std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Validation - failed to load schema", "[projectconfig]")
{
    CHECK(ProjectConfigValidator::validateConfiguration({}) == false);
}

TEST_CASE("PropertyEditorDialog", "[projectconfig]")
{
    CanDevice obj;
    PropertyEditorDialog dialog("title", *obj.getQConfig().get());

    auto props = dialog.properties();

    CHECK(props->property("name").isValid());
    CHECK(props->property("backend").isValid());
    CHECK(props->property("interface").isValid());
    CHECK(props->property("unsupported").isValid() == false);
}

TEST_CASE("PropertyEditorDialog no exposed props", "[projectconfig]")
{
    QObject obj;
    PropertyEditorDialog dialog("title", obj);

    auto props = dialog.properties();

    CHECK(props->property("name").isValid() == false);
    CHECK(props->property("backend").isValid() == false);
    CHECK(props->property("interface").isValid() == false);
    CHECK(props->property("unsupported").isValid() == false);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}

