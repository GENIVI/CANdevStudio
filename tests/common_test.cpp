
#include "enumiterator.h"

#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include <cstddef>  // size_t, ptrdiff_t
#include <cstdint> // uint16_t


enum class Trivial { A, B, C };

enum class NonTrivial : std::uint16_t { A = 100, B = 110, C = 111  };

enum class NoPastTheEndUnsigned : std::size_t
{
    A = std::numeric_limits<std::size_t>::min()
  , B = std::numeric_limits<std::size_t>::max()
};

enum class NoPastTheEndSigned : std::ptrdiff_t
{
    A = std::numeric_limits<std::ptrdiff_t>::min()
  , B = std::numeric_limits<std::ptrdiff_t>::max()
};

TEST_CASE("EnumIterator operator*", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{Trivial::A};
    CHECK(it.end() != it);

    CHECK(I{Trivial::B} == ++it);
    CHECK(Trivial::B == *it);
    CHECK(I{Trivial::C} == ++it);
    CHECK(Trivial::C == *it);
}

int main(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}

