module;

#include <stdexcept>
#include <string>

export module argman.parser;

import argman.command;

namespace argman {

export class CommandLineParser {
  private:
    Command* root;

  public:
    CommandLineParser(Command& root) : root(&root) { this->root->initialize(); }

    void parse(int argc, char* argv[]) {
        if (argc < 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
            root->show_help(argc > 0 ? argv[0] : "");
            return;
        }

        int options_end = root->parse_leading_options(argc, argv, 1);
        if (options_end >= argc) {
            root->show_help(argc > 0 ? argv[0] : "");
            return;
        }

        auto* target = root->get_subcommand(argv[options_end]);
        if (!target) {
            throw std::invalid_argument("unknown command: " + std::string(argv[options_end]));
        }

        auto [command, start] = target->find_target_command(argc, argv, options_end + 1);
        command->parse_options(argc, argv, start);
        command->execute();
    }
};

} // namespace argman
