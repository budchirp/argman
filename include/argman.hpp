#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "utils.hpp"

using namespace std;

namespace ArgMan {

enum class OptionType { STRING, INTEGER, BOOLEAN };

class OptionBase {
public:
  string name;
  string description;

  OptionType type;

  virtual void parse(const string value) = 0;
  virtual ~OptionBase() = default;
};

template <typename T> class Option : public OptionBase {
public:
  T value;

  Option(const string name, const string description, OptionType type,
         T default_value) {
    this->name = name;
    this->description = description;

    this->type = type;

    this->value = default_value;
  }

  void parse(const string value) override {
    this->value = from_string<T>(value);
  }
};

class Command {
public:
  string name;
  string description;

  unordered_map<string, unique_ptr<OptionBase>> options;

  Command(const string name, const string description)
      : name(name), description(description) {}

  template <typename T> void add_option(unique_ptr<Option<T>> option) {
    options[option->name] = std::move(option);
  }

  template <typename T>
  unique_ptr<Option<T>> get_option(const string option_name) {
    auto it = options.find(option_name);
    if (it == options.end()) {
      throw invalid_argument("unknown option: " + option_name);
    }

    auto *option = dynamic_cast<Option<T> *>(it->second.get());
    if (!option)
      throw bad_cast();

    return unique_ptr<Option<T>>(new Option<T>(*option));
  }

  void parse(int argc, char *argv[]) {
    for (int i = 2; i < argc; ++i) {
      string arg = argv[i];
      if (arg.starts_with("--")) {
        auto option_name = arg.substr(2);

        auto it = options.find(option_name);
        if (it == options.end())
          throw invalid_argument("unknown option: " + option_name);

        if (i + 1 < argc && !string(argv[i + 1]).starts_with("--")) {
          it->second->parse(argv[++i]);
        } else {
          throw invalid_argument("missing value for: " + option_name);
        }
      }
    }
  }

  virtual void execute() = 0;
  virtual ~Command() = default;
};

class CommandLineParser {
public:
  const string app_name;

  unordered_map<string, unique_ptr<Command>> commands;

  CommandLineParser(const string app_name) : app_name(app_name) {}

  void add_command(unique_ptr<Command> command) {
    commands[command->name] = std::move(command);
  }

  void parse(int argc, char *argv[]) {
    if (argc < 2)
      throw invalid_argument("no command provided");

    string command_name = argv[1];
    auto it = commands.find(command_name);
    if (it == commands.end())
      throw invalid_argument("unknown command: " + command_name);

    auto command = std::move(it->second);
    command->parse(argc, argv);
    command->execute();
  }
};

} // namespace ArgMan
