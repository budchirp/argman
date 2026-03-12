<div align="center">
    <h1>ArgMan</h1>
    <h2>Modern C++26 argument parsing library with subcommand support</h2>
</div>

<div align="center">
  <img alt="Stargazers" src="https://img.shields.io/github/stars/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=ff8e8e" />
  <img alt="Last commit" src="https://img.shields.io/github/last-commit/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=BDB0E4" />
  <img alt="Issues" src="https://img.shields.io/github/issues/budchirp/argman?style=for-the-badge&colorA=0b1221&colorB=FBC19D" />
</div>

## Features

- **Declarative API** - Define commands via a single `info()` override
- **Subcommand Support** - Nested subcommands with individual options and help
- **Type Safety** - Built-in support for `string`, `int`, `bool`, `float`, `double`
- **Automatic Help** - Generated help messages for all commands and subcommands
- **Modern C++26** - Uses C++26 modules for clean, fast builds
- **Static library** - Easy integration with CMake projects

## Quick Start

### Basic Command

```cpp
#include <print>

import argman;

class BuildCommand : public argman::Command {
public:
    Info info() override {
        return {.name = "build",
                .description = "Build the project",
                .options = {
                    argman::Option("debug", "Enable debug mode", false, true),
                    argman::Option("jobs", "Number of parallel jobs", 1),
                }};
    }

    void execute() override {
        std::println("Building project...");
        std::println("Debug: {}", get<bool>("debug") ? "enabled" : "disabled");
        std::println("Jobs: {}", get<int>("jobs"));
    }
};

class RootCommand : public argman::Command {
private:
    BuildCommand build_command;

public:
    Info info() override {
        return {.name = "myapp",
                .description = "My application",
                .commands = {&build_command}};
    }

    void execute() override {
        std::println("Use --help for available commands.");
    }
};

int main(int argc, char* argv[]) {
    RootCommand root;
    argman::CommandLineParser parser(root);

    try {
        parser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::println(stderr, "{}: error: {}", root.info().name, e.what());
        return 1;
    }

    return 0;
}
```

### With Subcommands

```cpp
class TestRunCommand : public argman::Command {
public:
    Info info() override {
        return {.name = "run",
                .description = "Run unit tests",
                .options = {
                    argman::Option("verbose", "Verbose output", false, true),
                    argman::Option("filter", "Test filter pattern", std::string("")),
                }};
    }

    void execute() override {
        std::println("Running tests...");
        if (get<bool>("verbose")) {
            std::println("Verbose mode enabled");
        }
    }
};

class TestCommand : public argman::Command {
private:
    TestRunCommand run_command;

public:
    Info info() override {
        return {.name = "test",
                .description = "Test commands",
                .commands = {&run_command}};
    }

    void execute() override {
        std::println("Use 'test --help' to see available subcommands.");
    }
};
```

### With Global Options

```cpp
class RootCommand : public argman::Command {
private:
    BuildCommand build_command;
    TestCommand test_command;

public:
    Info info() override {
        return {.name = "myapp",
                .description = "My application with global options",
                .options = {
                    argman::Option("verbose", "Enable verbose output", false, true),
                    argman::Option("config", "Configuration file", std::string("config.json")),
                },
                .commands = {&build_command, &test_command}};
    }

    void execute() override {
        std::println("Use --help for available commands.");
    }
};

int main(int argc, char* argv[]) {
    RootCommand root;
    argman::CommandLineParser parser(root);
    parser.parse(argc, argv);
    return 0;
}
```

## CMake Integration

### As a Subdirectory

```cmake
add_subdirectory(path/to/argman)
target_link_libraries(your_app PRIVATE argman)
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

target_link_libraries(your_app PRIVATE argman)
```

### Build Example

```bash
cmake --preset debug -DARGMAN_BUILD_EXAMPLE=ON
cmake --build --preset debug

./cmake-build-debug/bin/argman_example --help
```

## Option Types

Options use type-safe default values: `Option("name", "description", default_value, is_flag)`.

| Type | Default Example | Example Values |
|------|-----------------|----------------|
| `std::string` | `std::string("")` | `"hello"`, `"path/to/file"` |
| `int` | `0`, `1`, `100` | `42`, `-10`, `0` |
| `bool` | `false`, `true` | `true`, `false`, `1`, `0`, `yes`, `no` |
| `float` | `0.0f`, `3.14f` | `3.14`, `-2.5` |
| `double` | `0.0`, `3.14159` | `3.141592653589793` |

## Help System

ArgMan automatically generates help messages:

```bash
$ myapp --help
Usage: myapp [OPTIONS] <SUBCOMMAND>

My application

Options:
  --verbose        Enable verbose output
  --config <VALUE> Configuration file

Subcommands:
  build  Build the project
  test   Test commands

Use 'myapp <subcommand> --help' for more information.

$ myapp test --help
Usage: myapp test <SUBCOMMAND>

Test commands

Subcommands:
  run  Run unit tests

Use 'myapp test <subcommand> --help' for more information.
```

## Requirements

- **C++26** compatible compiler (Clang 16+ with module support)
- **CMake 3.30+**
