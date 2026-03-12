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
                std::print("  --{}", name);
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

  public:
    CommandLineParser(Command& root) : root(&root) { initialize(root); }

    void parse(int argc, char* argv[]) {
        std::string program_name = argc > 0 ? argv[0] : "";

        if (argc < 2) {
            show_help(*root, program_name);
            return;
        }

        Command* current = root;
        std::string command_path = program_name;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                show_help(*current, command_path);
                return;
            }

            if (arg.starts_with("--")) {
                auto it = current->options.find(arg.substr(2));
                if (it == current->options.end()) {
                    throw std::invalid_argument("unknown option: " + arg.substr(2));
                }

                if (it->second.is_flag) {
                    it->second.set_flag();
                } else if (it->second.is_list()) {
                    while (i + 1 < argc && !std::string(argv[i + 1]).starts_with("--")) {
                        it->second.parse(argv[++i]);
                    }
                } else {
                    if (i + 1 >= argc) {
                        throw std::invalid_argument("missing value for option: " + it->first);
                    }
                    it->second.parse(argv[++i]);
                }
            } else {
                auto* sub = find_subcommand(*current, arg);
                if (!sub) {
                    throw std::invalid_argument("unknown command: " + arg);
                }
                current = sub;
                command_path += " " + arg;
            }
        }

        current->execute();
    }
};

} // namespace argman
