#include "argman.hpp"

using namespace ArgMan;

class BuildCommand : public Command {
public:
  string name() const override { return "build"; }
  string description() const override { return "Build the project"; }

  vector<Option> options() const override {
    return {{"ldflags", "Linker flags", "string", false, ""},
            {"debug", "Enable debug mode", "bool", true, "false"},
            {"jobs", "Number of parallel jobs", "int", false, "1"}};
  }

  void execute() override {
    cout << "Building project...\n";
    cout << "Debug: "
         << (get_option_value<bool>("debug") ? "enabled" : "disabled") << "\n";
    cout << "LDFLAGS: " << get_option_value<string>("ldflags") << "\n";
    cout << "Jobs: " << get_option_value<int>("jobs") << "\n";

    if (!positional_args.empty()) {
      cout << "Targets: ";
      for (const auto &target : positional_args) {
        cout << target << " ";
      }
      cout << "\n";
    }
  }
};

class CleanCommand : public Command {
public:
  string name() const override { return "clean"; }
  string description() const override { return "Clean build artifacts"; }

  vector<Option> options() const override {
    return {{"force", "Force clean all files", "bool", true, "false"}};
  }

  void execute() override {
    cout << "Cleaning project...\n";
    if (get_option_value<bool>("force")) {
      cout << "Force clean enabled\n";
    }
  }
};

class TestRunCommand : public Command {
public:
  string name() const override { return "run"; }
  string description() const override { return "Run unit tests"; }

  vector<Option> options() const override {
    return {{"verbose", "Verbose test output", "bool", true, "false"},
            {"filter", "Test filter pattern", "string", false, ""}};
  }

  void execute() override {
    cout << "Running tests...\n";
    if (get_option_value<bool>("verbose")) {
      cout << "Verbose output enabled\n";
    }
    if (auto filter = get_option_value<string>("filter"); !filter.empty()) {
      cout << "Filter: " << filter << "\n";
    }
  }
};

class TestBenchCommand : public Command {
public:
  string name() const override { return "bench"; }
  string description() const override { return "Run benchmarks"; }

  vector<Option> options() const override {
    return {
        {"iterations", "Number of benchmark iterations", "int", false, "100"}};
  }

  void execute() override {
    cout << "Running benchmarks...\n";
    cout << "Iterations: " << get_option_value<int>("iterations") << "\n";
  }
};

class TestCommand : public Command {
public:
  string name() const override { return "test"; }
  string description() const override { return "Run tests and benchmarks"; }

  void init() override {
    add_command(make_shared<TestRunCommand>());
    add_command(make_shared<TestBenchCommand>());
  }

  void execute() override {
    cout << "Test command requires a subcommand. Use --help for available "
            "options.\n";
  }
};

class DocGenerateCommand : public Command {
public:
  string name() const override { return "generate"; }
  string description() const override { return "Generate documentation"; }

  vector<Option> options() const override {
    return {{"format", "Output format", "string", false, "html"},
            {"output", "Output directory", "string", false, "docs/"}};
  }

  void execute() override {
    cout << "Generating documentation...\n";
    cout << "Format: " << get_option_value<string>("format") << "\n";
    cout << "Output: " << get_option_value<string>("output") << "\n";
  }
};

class DocServeCommand : public Command {
public:
  string name() const override { return "serve"; }
  string description() const override { return "Serve documentation locally"; }

  vector<Option> options() const override {
    return {{"host", "Server host", "string", false, "localhost"},
            {"port", "Server port", "int", false, "8080"}};
  }

  void execute() override {
    cout << "Serving documentation...\n";
    cout << "Host: " << get_option_value<string>("host") << "\n";
    cout << "Port: " << get_option_value<int>("port") << "\n";
  }
};

class DocCommand : public Command {
public:
  string name() const override { return "doc"; }
  string description() const override { return "Documentation utilities"; }

  void init() override {
    add_command(make_shared<DocGenerateCommand>());
    add_command(make_shared<DocServeCommand>());
  }

  void execute() override {
    cout << "Documentation command requires a subcommand. Use --help for "
            "available options.\n";
  }
};

int main(int argc, char *argv[]) {
  auto parser = CommandLineParser(
      "argman", "A modern C++ argument parsing library",
      {{"verbose", "Enable verbose output", "bool", true, "false"},
       {"config", "Configuration file path", "string", false, "config.json"}});

  parser.add_command(make_shared<BuildCommand>());
  parser.add_command(make_shared<CleanCommand>());
  parser.add_command(make_shared<TestCommand>());
  parser.add_command(make_shared<DocCommand>());

  try {
    parser.parse(argc, argv);
  } catch (const exception &e) {
    cerr << parser.app_name << ": error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}