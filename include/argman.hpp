#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hpp"

using namespace std;

namespace ArgMan {

struct Option {
  string name;
  string description;
  string type_name;
  bool is_flag;
  string default_value;
};

class OptionBase {
public:
  const string name;
  const string description;
  const bool is_flag;

  OptionBase(string name, string description, bool is_flag = false)
      : name(move(name)), description(move(description)), is_flag(is_flag) {}

  virtual void parse(const string &value) = 0;
  virtual void set_flag() {}
  virtual ~OptionBase() = default;
};

template <typename T> class OptionImpl : public OptionBase {
public:
  T value;

  OptionImpl(string name, string description, T default_value,
             bool is_flag = false)
      : OptionBase(move(name), move(description), is_flag),
        value(move(default_value)) {}

  void parse(const string &str) override { value = from_string<T>(str); }

  void set_flag() override {
    if constexpr (is_same_v<T, bool>) {
      value = true;
    }
  }
};

inline unique_ptr<OptionBase> create_option(const Option &def) {
  if (def.type_name == "string") {
    return make_unique<OptionImpl<string>>(def.name, def.description,
                                           def.default_value, def.is_flag);
  } else if (def.type_name == "int") {
    int default_val = def.default_value.empty() ? 0 : stoi(def.default_value);
    return make_unique<OptionImpl<int>>(def.name, def.description, default_val,
                                        def.is_flag);
  } else if (def.type_name == "bool") {
    bool default_val = def.default_value == "true" || def.default_value == "1";
    return make_unique<OptionImpl<bool>>(def.name, def.description, default_val,
                                         def.is_flag);
  } else if (def.type_name == "float") {
    float default_val =
        def.default_value.empty() ? 0.0f : stof(def.default_value);
    return make_unique<OptionImpl<float>>(def.name, def.description,
                                          default_val, def.is_flag);
  } else if (def.type_name == "double") {
    double default_val =
        def.default_value.empty() ? 0.0 : stod(def.default_value);
    return make_unique<OptionImpl<double>>(def.name, def.description,
                                           default_val, def.is_flag);
  }
  throw invalid_argument("unsupported option type: " + def.type_name);
}

class Command {
  unordered_map<string, unique_ptr<OptionBase>> option_map;
  unordered_map<string, shared_ptr<Command>> subcommands;
  Command *parent = nullptr;

  void initialize_options() {
    for (const auto &def : options()) {
      option_map[def.name] = create_option(def);
    }
  }

  static bool is_option_arg(const string &arg) {
    return arg.size() >= 2 && arg.substr(0, 2) == "--";
  }

  void parse_single_option(int &i, int argc, char *argv[]) {
    const string arg = argv[i];
    const string option_name = arg.substr(2);

    auto it = option_map.find(option_name);
    if (it == option_map.end()) {
      throw invalid_argument("unknown option: " + option_name);
    }

    if (it->second->is_flag) {
      it->second->set_flag();
    } else {
      if (i + 1 >= argc || is_option_arg(argv[i + 1])) {
        throw invalid_argument("missing value for option: " + option_name);
      }
      it->second->parse(argv[++i]);
    }
  }

public:
  vector<string> positional_args;

  Command() = default;
  virtual ~Command() = default;

  void initialize() {
    initialize_options();
    init();
  }

  virtual string name() const = 0;
  virtual string description() const = 0;
  virtual vector<Option> options() const { return {}; }
  virtual void init() {}

protected:
  void add_command(shared_ptr<Command> subcommand) {
    subcommand->parent = this;
    subcommand->initialize();
    subcommands[subcommand->name()] = move(subcommand);
  }

  template <typename T>
  const T &get_option_value(const string &option_name) const {
    auto it = option_map.find(option_name);
    if (it == option_map.end()) {
      throw invalid_argument("unknown option: " + option_name);
    }

    auto *option = dynamic_cast<OptionImpl<T> *>(it->second.get());
    if (!option) {
      throw bad_cast();
    }

    return option->value;
  }

public:
  void show_help(const string &program_name = "") const {
    string full_path = get_full_command_path();
    if (!program_name.empty()) {
      full_path = program_name + " " + full_path;
    }

    cout << "Usage: " << full_path;
    if (!option_map.empty())
      cout << " [OPTIONS]";
    if (!subcommands.empty())
      cout << " <SUBCOMMAND>";
    cout << "\n\n" << description() << "\n";

    if (!option_map.empty()) {
      cout << "\nOptions:\n";
      for (const auto &[name, option] : option_map) {
        cout << "  --" << name;
        if (!option->is_flag)
          cout << " <VALUE>";
        cout << "\t" << option->description << "\n";
      }
    }

    if (!subcommands.empty()) {
      cout << "\nSubcommands:\n";
      for (const auto &[name, subcommand] : subcommands) {
        cout << "  " << name << "\t" << subcommand->description() << "\n";
      }
      cout << "\nUse '" << full_path
           << " <subcommand> --help' for more information.\n";
    }
  }

  string get_full_command_path() const {
    if (parent) {
      string parent_path = parent->get_full_command_path();
      return parent_path.empty() ? name() : parent_path + " " + name();
    }
    return name();
  }

  pair<Command *, int> find_target_command(int argc, char *argv[],
                                           int start_index = 1) {
    Command *current = this;
    int i = start_index;

    while (i < argc && !is_option_arg(argv[i])) {
      if (string arg = argv[i]; arg == "--help" || arg == "-h") {
        current->show_help(argc > 0 ? argv[0] : "");
        exit(0);
      }

      auto it = current->subcommands.find(argv[i]);
      if (it != current->subcommands.end()) {
        current = it->second.get();
        i++;
      } else {
        current->positional_args.emplace_back(argv[i]);
        i++;
      }
    }

    return {current, i};
  }

  void parse_options(int argc, char *argv[], int start_index) {
    for (int i = start_index; i < argc; ++i) {
      const string arg = argv[i];

      if (arg == "--help" || arg == "-h") {
        show_help(argc > 0 ? argv[0] : "");
        exit(0);
      }

      if (is_option_arg(arg)) {
        parse_single_option(i, argc, argv);
      } else {
        positional_args.emplace_back(arg);
      }
    }
  }

  virtual void execute() = 0;
};

class CommandLineParser {
  unordered_map<string, shared_ptr<Command>> commands;
  unordered_map<string, unique_ptr<OptionBase>> global_options;

  void initialize_global_options(const vector<Option> &options) {
    for (const auto &def : options) {
      global_options[def.name] = create_option(def);
    }
  }

  void parse_global_options(int &global_end, int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
      const string arg = argv[i];

      if (arg.size() < 2 || arg.substr(0, 2) != "--") {
        global_end = i;
        break;
      }

      const string option_name = arg.substr(2);
      auto it = global_options.find(option_name);

      if (it != global_options.end()) {
        if (it->second->is_flag) {
          it->second->set_flag();
        } else {
          if (i + 1 >= argc || string(argv[i + 1]).substr(0, 2) == "--") {
            throw invalid_argument("missing value for global option: " +
                                   option_name);
          }
          it->second->parse(argv[++i]);
        }
        global_end = i + 1;
      } else {
        global_end = i;
        break;
      }
    }
  }

public:
  const string app_name;
  const string description;

  CommandLineParser(string app_name, string description = "",
                    vector<Option> global_options = {})
      : app_name(move(app_name)), description(move(description)) {
    initialize_global_options(global_options);
  }

  void add_command(shared_ptr<Command> command) {
    command->initialize();
    commands[command->name()] = move(command);
  }

  void show_help() const {
    cout << "Usage: " << app_name;
    if (!global_options.empty())
      cout << " [GLOBAL_OPTIONS]";
    if (!commands.empty())
      cout << " <COMMAND>";
    cout << "\n\n"
         << (description.empty() ? "Command line application" : description)
         << "\n";

    if (!global_options.empty()) {
      cout << "\nGlobal Options:\n";
      for (const auto &[name, option] : global_options) {
        cout << "  --" << name;
        if (!option->is_flag)
          cout << " <VALUE>";
        cout << "\t" << option->description << "\n";
      }
    }

    if (!commands.empty()) {
      cout << "\nCommands:\n";
      for (const auto &[name, command] : commands) {
        cout << "  " << name << "\t" << command->description() << "\n";
      }
      cout << "\nUse '" << app_name
           << " <command> --help' for more information.\n";
    }
  }

  void parse(int argc, char *argv[]) {
    if (argc < 2 || string(argv[1]) == "--help" || string(argv[1]) == "-h") {
      show_help();
      return;
    }

    int global_end = 1;
    parse_global_options(global_end, argc, argv);

    if (global_end >= argc) {
      show_help();
      return;
    }

    const string command_name = argv[global_end];
    auto it = commands.find(command_name);
    if (it == commands.end()) {
      throw invalid_argument("unknown command: " + command_name);
    }

    auto [target_command, options_start] =
        it->second->find_target_command(argc, argv, global_end + 1);
    target_command->parse_options(argc, argv, options_start);
    target_command->execute();
  }
};

} // namespace ArgMan