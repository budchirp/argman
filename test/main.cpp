#include "argman.hpp"

#include <iostream>

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
  build_command->add_option(
      make_unique<Option<string>>("ldflags", "Linker flags", ""));
  build_command->add_option(
      make_unique<Option<bool>>("debug", "Debug mode", false));

  command_line_parser->add_command(std::move(build_command));

  try {
    command_line_parser->parse(argc, argv);
  } catch (const exception &e) {
    cerr << command_line_parser->app_name << ": error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
