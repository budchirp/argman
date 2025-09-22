<div align="center">
    <h1>ArgMan</h1>
    <h2>Modern C++20 argument parsing library with subcommand support</h2>
</div>

<div align="center">
  <img alt="Stargazers" src="https://img.shields.io/github/stars/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=ff8e8e" />
  <img alt="Last commit" src="https://img.shields.io/github/last-commit/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=BDB0E4" />
  <img alt="Issues" src="https://img.shields.io/github/issues/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=FBC19D" />
</div>

## Features

- **Declarative API** - Define commands, options, and subcommands directly in class definitions
- **Subcommand Support** - Nested subcommands with individual options and help
- **Type Safety** - Built-in support for `string`, `int`, `bool`, `float`, `double`
- **Automatic Help** - Generated help messages for all commands and subcommands
- **Global Options** - Options that work across all commands
- **Modern C++20** - Uses latest C++ features for clean, efficient code
- **Header-only** - Easy integration with CMake projects

## Quick Start

### Basic Command

```cpp
#include "argman.hpp"

using namespace ArgMan;

class BuildCommand : public Command {
public:
  string name() const override { return "build"; }
  string description() const override { return "Build the project"; }
  
  vector<Option> options() const override {
    return {
      {"debug", "Enable debug mode", "bool", true, "false"},
      {"jobs", "Number of parallel jobs", "int", false, "1"}
    };
  }

  void execute() override {
    cout << "Building project...\n";
    cout << "Debug: " << (get_option_value<bool>("debug") ? "enabled" : "disabled") << "\n";
    cout << "Jobs: " << get_option_value<int>("jobs") << "\n";
  }
};

int main(int argc, char* argv[]) {
  auto parser = CommandLineParser("myapp", "My application");
  parser.add_command(make_shared<BuildCommand>());
  
  try {
    parser.parse(argc, argv);
  } catch (const exception& e) {
    cerr << parser.app_name << ": error: " << e.what() << "\n";
    return 1;
  }
  
  return 0;
}
```

### With Subcommands

```cpp
class TestRunCommand : public Command {
public:
  string name() const override { return "run"; }
  string description() const override { return "Run unit tests"; }
  
  vector<Option> options() const override {
    return {
      {"verbose", "Verbose output", "bool", true, "false"},
      {"filter", "Test filter pattern", "string", false, ""}
    };
  }

  void execute() override {
    cout << "Running tests...\n";
    if (get_option_value<bool>("verbose")) {
      cout << "Verbose mode enabled\n";
    }
  }
};

class TestCommand : public Command {
public:
  string name() const override { return "test"; }
  string description() const override { return "Test commands"; }
  
  void init() override {
    add_command(make_shared<TestRunCommand>());
  }

  void execute() override {
    cout << "Use 'test --help' to see available subcommands.\n";
  }
};
```

### With Global Options

```cpp
int main(int argc, char* argv[]) {
  auto parser = CommandLineParser(
    "myapp",
    "My application with global options",
    {
      {"verbose", "Enable verbose output", "bool", true, "false"},
      {"config", "Configuration file", "string", false, "config.json"}
    }
  );
  
  parser.add_command(make_shared<BuildCommand>());
  parser.add_command(make_shared<TestCommand>());
  
  parser.parse(argc, argv);
  return 0;
}
```

## CMake Integration

### As a Subdirectory

```cmake
# Add argman to your project
add_subdirectory(path/to/argman)

# Link to your executable
target_link_libraries(your_app PRIVATE argman::argman)
```

### With FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  argman
  GIT_REPOSITORY https://github.com/budchirp/argman.git
  GIT_TAG main
)

FetchContent_MakeAvailable(argman)

target_link_libraries(your_app PRIVATE argman::argman)
```

### Build Example

```bash
# Build the example (optional, off by default)
cmake -B build -DARGMAN_BUILD_EXAMPLE=ON
cmake --build build

# Run the example
./build/bin/argman_example --help
```

## Option Types

| Type | Description | Example Values |
|------|-------------|----------------|
| `string` | Text values | `"hello"`, `"path/to/file"` |
| `int` | Integer numbers | `42`, `-10`, `0` |
| `bool` | Boolean flags | `true`, `false`, `1`, `0`, `yes`, `no` |
| `float` | Floating point | `3.14`, `-2.5` |
| `double` | Double precision | `3.141592653589793` |

## Help System

ArgMan automatically generates help messages:

```bash
$ myapp --help
Usage: myapp [GLOBAL_OPTIONS] <COMMAND>

My application

Global Options:
  --verbose        Enable verbose output
  --config <VALUE> Configuration file

Commands:
  build  Build the project
  test   Test commands

Use 'myapp <command> --help' for more information.

$ myapp test --help
Usage: myapp test <SUBCOMMAND>

Test commands

Subcommands:
  run  Run unit tests

Use 'myapp test <subcommand> --help' for more information.
```

## Requirements

- **C++20** compatible compiler (GCC 10+, Clang 10+)
- **CMake 3.20+**