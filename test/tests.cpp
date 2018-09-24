#include <catch.hpp>
#include <rekt/string_enum.hpp>
#include <string>
#include <iostream>

constexpr auto color = rekt::make_string_enum("red", "green", "blue");

static_assert(color("blue") == 0, "blue -> 0");
static_assert(color("green") == 1, "green -> 1");
static_assert(color("red") == 2, "red -> 2");

TEST_CASE("basic usage")
{
  std::string strings[] = { "red", "green", "blue", "octarine" };

  for (auto s : strings)
  {
    switch (color(s.c_str()))
    {
    case color("red"):
      REQUIRE(s == "red");
      break;
      
    case color("green"):
      REQUIRE(s == "green");
      break;
      
    case color("blue"):
      REQUIRE(s == "blue");
      break;
      
    default:
      REQUIRE(s == "octarine");
      break;
    };
  }
}