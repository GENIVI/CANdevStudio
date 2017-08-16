#include "canrawsender/newlinemanager.h"
#include "canrawsender/canrawsender.h"
#define CATCH_CONFIG_RUNNER
#include <QtWidgets/QApplication>
#include <catch.hpp>
#include <fakeit.hpp>
#include <memory>

TEST_CASE("Create NewLineManager class with wrong argument", "[newlinemanager]")
{
    REQUIRE_THROWS(new NewLineManager(nullptr, false));
}

TEST_CASE("Create NewLineManager class with correct argument", "[newlinemanager]")
{
    CanRawSender canRawSender;
    REQUIRE_NOTHROW(new NewLineManager(&canRawSender, false));
}

TEST_CASE("Check initial wigdets settings", "[newlinemanager]")
{
    CanRawSender canRawSender;
    NewLineManager newLine(&canRawSender, false);

    auto intervalWidget = newLine.GetColsWidget(NewLineManager::ColName::IntervalLine);
    CHECK(intervalWidget->isEnabled() == false);

    auto sendButtonWidget = newLine.GetColsWidget(NewLineManager::ColName::SendButton);
    CHECK(sendButtonWidget->isEnabled() == false);
}

TEST_CASE("Check get column widget returns", "[newlinemanager]")
{
    CanRawSender canRawSender;
    NewLineManager newLine(&canRawSender, false);

    auto idWidget = newLine.GetColsWidget(NewLineManager::ColName::IdLine);
    CHECK(idWidget != nullptr);

    auto batWigdet = newLine.GetColsWidget(NewLineManager::ColNameIterator().end());
    CHECK(batWigdet == nullptr);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}
