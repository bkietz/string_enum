# string enumerations

simple logarithmic time lookup of strings, also at compile time

no macros

```c++
int get_color()
{
  constexpr auto color = rekt::make_string_enum("red", "green", "blue");

  std::string user_value;
  std::cout << "pick a color: ";
  std::cin >> user_value;

  switch (color(user_value))
  {
  case color("red"):
    return 0xf00;
  case color("green"):
    return 0x0f0;
  case color("blue"):
    return 0x0f0;
  default:
    std::cout << "sorry, I don't know that one. Please enter hex value:\n0x";
    std::cin >> std::hex >> user_value;
    return std::stoi(user_value);
  };
}
```
