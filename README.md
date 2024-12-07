<div align="center">
    <h1>argman</h1>
    <h2>Small C++ library for basic command line argument parsing</h2>
</div>

<div align="center">
  <img alt="Stargazers" src="https://img.shields.io/github/stars/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=ff8e8e" />
  <img alt="Last commit" src="https://img.shields.io/github/last-commit/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=BDB0E4" />
  <img alt="Issues" src="https://img.shields.io/github/issues/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=FBC19D" />
</div>

## Usage

```cpp
#include <iostream>

#include "argman.hpp"

using namespace ArgMan;

class BuildCommand : public Command {
public:
  BuildCommand(const string &name, const string &description)
      : Command(name, description) {}

  void execute() override {
    auto debug_option = get_option<bool>("debug");
    auto ldflags_option = get_option<string>("ldflags");

    cout << "Debug: " << (debug_option->value ? "true" : "false") << endl;
    cout << "LDFLAGS: " << ldflags_option->value << endl;
  }
};

int main(int argc, char *argv[]) {
  auto command_line_parser = make_shared<CommandLineParser>("argman");

  auto build_command = make_unique<BuildCommand>("build", "Build the project");
  build_command->add_option(make_unique<Option<string>>(
      "ldflags", "Linker flags", OptionType::STRING, ""));
  build_command->add_option(make_unique<Option<bool>>(
      "debug", "Debug mode", OptionType::BOOLEAN, false));

  command_line_parser->add_command(std::move(build_command));

  try {
    command_line_parser->parse(argc, argv);
  } catch (const exception &e) {
    cerr << command_line_parser->app_name << ": error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
```
