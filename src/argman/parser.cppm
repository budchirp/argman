module;

#include <print>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module argman.parser;

import argman.option;
import argman.command;

namespace argman {

export class CommandLineParser {
  private:
    Command* root;

    void initialize(Command& command) {
        auto command_info = command.info();

        for (auto& option : command_info.options) {
            for (const auto& alias : option.aliases) {
                command.aliases[alias] = option.name;
            }
            command.options.insert_or_assign(option.name, std::move(option));
        }

        for (auto* sub : command_info.commands) {
            if (sub) {
                initialize(*sub);
            }
        }
    }

    Command* find_subcommand(Command& command, const std::string& name) {
        for (auto* sub : command.info().commands) {
            if (sub && sub->info().name == name) {
                return sub;
            }
        }

        return nullptr;
    }

    void show_help(Command& command, const std::string& prefix) {
        auto command_info = command.info();

        std::print("Usage: {}", prefix);
        if (!command.options.empty()) {
            std::print(" [OPTIONS]");
        }
        if (!command_info.commands.empty()) {
            std::print(" <COMMAND>");
        }
        std::println("\n\n{}", command_info.description);

        if (!command.options.empty()) {
            std::println("\nOptions:");
            for (const auto& [name, option] : command.options) {
                if (!option.aliases.empty()) {
                    for (size_t i = 0; i < option.aliases.size(); ++i) {
                        std::print("  -{}", option.aliases[i]);
                        if (i + 1 < option.aliases.size()) {
                            std::print(",");
                        }
                    }
                    std::print(", ");
                } else {
                    std::print("      ");
                }
                std::print("--{}", name);
                if (!option.is_flag) {
                    if (option.is_list()) {
                        std::print(" <VALUES...>");
                    } else {
                        std::print(" <VALUE>");
                    }
                }
                std::println("\t{}", option.description);
            }
        }

        if (!command_info.commands.empty()) {
            std::println("\nCommands:");
            for (auto* sub : command_info.commands) {
                if (sub) {
                    std::println("  {}\t{}", sub->info().name, sub->info().description);
                }
            }
            std::println("\nUse '{} <command> --help' for more information.", prefix);
        }
    }

    void process_option(Command& current, const std::string& option_name, int& i, int argc,
                        char* argv[]) {
        auto it = current.options.find(option_name);
        if (it == current.options.end()) {
            throw std::invalid_argument("unknown option: " + option_name);
        }

        if (it->second.is_flag) {
            it->second.set_flag();
        } else if (it->second.is_list()) {
            while (i + 1 < argc && !std::string(argv[i + 1]).starts_with("--") &&
                   !std::string(argv[i + 1]).starts_with("-")) {
                it->second.parse(argv[++i]);
            }
        } else {
            if (i + 1 >= argc) {
                throw std::invalid_argument("missing value for option: " + it->first);
            }
            it->second.parse(argv[++i]);
        }
    }

  public:
    CommandLineParser(Command& root) : root(&root) { initialize(root); }

    int parse(int argc, char* argv[]) {
        std::string program_name = argc > 0 ? argv[0] : "";

        if (argc < 2) {
            show_help(*root, program_name);
            return 0;
        }

        Command* current = root;
        std::string command_path = program_name;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                show_help(*current, command_path);
                return 0;
            }

            if (arg.starts_with("--")) {
                process_option(*current, arg.substr(2), i, argc, argv);
            } else if (arg.starts_with("-") && arg.size() >= 2) {
                std::string alias = arg.substr(1);
                auto alias_it = current->aliases.find(alias);
                if (alias_it == current->aliases.end()) {
                    throw std::invalid_argument("unknown option: " + arg);
                }
                process_option(*current, alias_it->second, i, argc, argv);
            } else {
                auto* sub = find_subcommand(*current, arg);
                if (!sub) {
                    throw std::invalid_argument("unknown command: " + arg);
                }
                current = sub;
                command_path += " " + arg;
            }
        }

        return current->execute();
    }
};

} // namespace argman
