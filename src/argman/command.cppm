module;

#include <cstdlib>
#include <print>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module argman.command;

import argman.option;

namespace argman {

export class Command {
  private:
    std::unordered_map<std::string, Option> options;
    std::unordered_map<std::string, Command*> subcommands;
    std::vector<std::string> arguments;
    Command* parent = nullptr;

  public:
    Command() = default;
    virtual ~Command() = default;

    void initialize() {
        for (const auto& option : init_options()) {
            options.insert_or_assign(option.name, option);
        }
        init();
    }

    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual std::vector<Option> init_options() const { return {}; }
    virtual void init() {}

    Command* get_subcommand(const std::string& name) const {
        auto it = subcommands.find(name);
        return it != subcommands.end() ? it->second : nullptr;
    }

    template <typename T>
    const T& get(const std::string& name) const {
        auto it = options.find(name);
        if (it == options.end()) {
            throw std::invalid_argument("unknown option: " + name);
        }
        return it->second.template get<T>();
    }

    const std::vector<std::string>& get_arguments() const { return arguments; }

    const std::string& get_argument(std::size_t index) const { return arguments.at(index); }

  protected:
    void add_command(Command& command) {
        command.parent = this;
        command.initialize();
        subcommands.insert_or_assign(command.name(), &command);
    }

  public:
    void show_help(const std::string& program_name = "") const {
        std::string prefix = program_name.empty() ? name() : program_name;
        if (auto path = get_full_command_path(); !path.empty()) {
            prefix += " " + path;
        }

        std::print("Usage: {}", prefix);
        if (!options.empty()) {
            std::print(" [OPTIONS]");
        }
        if (!subcommands.empty()) {
            std::print(" <SUBCOMMAND>");
        }
        std::println("\n\n{}", description());

        if (!options.empty()) {
            std::println("\nOptions:");
            for (const auto& [option_name, option] : options) {
                std::print("  --{}", option_name);
                if (!option.is_flag) {
                    std::print(" <VALUE>");
                }
                std::println("\t{}", option.description);
            }
        }

        if (!subcommands.empty()) {
            std::println("\nSubcommands:");
            for (const auto& [command_name, command] : subcommands) {
                std::println("  {}\t{}", command_name, command->description());
            }
            std::println("\nUse '{} <subcommand> --help' for more information.", prefix);
        }
    }

    std::string get_full_command_path() const {
        if (!parent) {
            return "";
        }
        if (auto path = parent->get_full_command_path(); !path.empty()) {
            return path + " " + name();
        }
        return name();
    }

    int parse_leading_options(int argc, char* argv[], int start_index) {
        int i = start_index;

        while (i < argc) {
            std::string arg = argv[i];
            if (!arg.starts_with("--")) {
                break;
            }

            auto it = options.find(arg.substr(2));
            if (it == options.end()) {
                break;
            }

            if (it->second.is_flag) {
                it->second.set_flag();
            } else {
                if (i + 1 >= argc) {
                    throw std::invalid_argument("missing value for option: " + it->first);
                }
                it->second.parse(argv[++i]);
            }

            i++;
        }

        return i;
    }

    std::pair<Command*, int> find_target_command(int argc, char* argv[], int start_index = 1) {
        Command* current = this;
        int i = start_index;

        while (i < argc) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                current->show_help(argc > 0 ? argv[0] : "");
                std::exit(0);
            }

            if (arg.starts_with("--")) {
                break;
            }

            if (auto it = current->subcommands.find(arg); it != current->subcommands.end()) {
                current = it->second;
            } else {
                current->arguments.push_back(arg);
            }

            i++;
        }

        return {current, i};
    }

    void parse_options(int argc, char* argv[], int start_index) {
        for (int i = start_index; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                show_help(argc > 0 ? argv[0] : "");
                std::exit(0);
            }

            if (arg.starts_with("--")) {
                auto it = options.find(arg.substr(2));
                if (it == options.end()) {
                    throw std::invalid_argument("unknown option: " + arg.substr(2));
                }

                if (it->second.is_flag) {
                    it->second.set_flag();
                } else {
                    if (i + 1 >= argc) {
                        throw std::invalid_argument("missing value for option: " + it->first);
                    }
                    it->second.parse(argv[++i]);
                }
            } else {
                arguments.push_back(arg);
            }
        }
    }

    virtual void execute() = 0;
};

} // namespace argman
