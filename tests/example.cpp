#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

TEST_CASE("Simple test", "[example]") {
}

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}
