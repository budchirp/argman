#include <exception>
#include <print>
#include <string>
#include <vector>

import argman;

class BuildCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "build",
                .description = "Build the project",
                .options = {
                    argman::Option("ldflags", "Linker flags", std::string("")),
                    argman::Option("debug", "Enable debug mode", false, true, std::vector<std::string>{"d"}),
                    argman::Option("jobs", "Number of parallel jobs", 1, false, std::vector<std::string>{"j"}),
                    argman::Option("files", "Source files to build", std::vector<std::string>{}),
                }};
    }

    int execute() override {
        std::println("Building project...");
        std::println("Debug: {}", get<bool>("debug") ? "enabled" : "disabled");
        std::println("LDFLAGS: {}", get<std::string>("ldflags"));
        std::println("Jobs: {}", get<int>("jobs"));

        for (const auto& file : get<std::vector<std::string>>("files")) {
            std::println("File: {}", file);
        }
        return 0;
    }
};

class CleanCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "clean",
                .description = "Clean build artifacts",
                .options = {
                    argman::Option("force", "Force clean all files", false, true),
                }};
    }

    int execute() override {
        std::println("Cleaning project...");
        if (get<bool>("force")) {
            std::println("Force clean enabled");
        }
        return 0;
    }
};

class TestRunCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "run",
                .description = "Run unit tests",
                .options = {
                    argman::Option("verbose", "Verbose test output", false, true),
                    argman::Option("filter", "Test filter pattern", std::string("")),
                }};
    }

    int execute() override {
        std::println("Running tests...");
        if (get<bool>("verbose")) {
            std::println("Verbose output enabled");
        }
        if (auto filter = get<std::string>("filter"); !filter.empty()) {
            std::println("Filter: {}", filter);
        }
        return 0;
    }
};

class TestBenchCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "bench",
                .description = "Run benchmarks",
                .options = {
                    argman::Option("iterations", "Number of benchmark iterations", 100),
                }};
    }

    int execute() override {
        std::println("Running benchmarks...");
        std::println("Iterations: {}", get<int>("iterations"));
        return 0;
    }
};

class TestCommand : public argman::Command {
  private:
    TestRunCommand run_command;
    TestBenchCommand bench_command;

  public:
    Info info() override {
        return {.name = "test",
                .description = "Run tests and benchmarks",
                .commands = {&run_command, &bench_command}};
    }

    int execute() override { std::println("Use 'test --help' for available subcommands."); return 0; }
};

class DocGenerateCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "generate",
                .description = "Generate documentation",
                .options = {
                    argman::Option("format", "Output format", std::string("html")),
                    argman::Option("output", "Output directory", std::string("docs/")),
                }};
    }

    int execute() override {
        std::println("Generating documentation...");
        std::println("Format: {}", get<std::string>("format"));
        std::println("Output: {}", get<std::string>("output"));
        return 0;
    }
};

class DocServeCommand : public argman::Command {
  public:
    Info info() override {
        return {.name = "serve",
                .description = "Serve documentation locally",
                .options = {
                    argman::Option("host", "Server host", std::string("localhost")),
                    argman::Option("port", "Server port", 8080),
                }};
    }

    int execute() override {
        std::println("Serving documentation...");
        std::println("Host: {}", get<std::string>("host"));
        std::println("Port: {}", get<int>("port"));
        return 0;
    }
};

class DocCommand : public argman::Command {
  private:
    DocGenerateCommand generate_command;
    DocServeCommand serve_command;

  public:
    Info info() override {
        return {.name = "doc",
                .description = "Documentation utilities",
                .commands = {&generate_command, &serve_command}};
    }

    int execute() override { std::println("Use 'doc --help' for available subcommands."); return 0; }
};

class RootCommand : public argman::Command {
  private:
    BuildCommand build_command;
    CleanCommand clean_command;
    TestCommand test_command;
    DocCommand doc_command;

  public:
    Info info() override {
        return {
            .name = "argman",
            .description = "A modern C++ argument parsing library",
            .options =
                {
                    argman::Option("verbose", "Enable verbose output", false, true, std::vector<std::string>{"v"}),
                    argman::Option("config", "Configuration file path", std::string("config.json"), false, std::vector<std::string>{"c"}),
                },
            .commands = {&build_command, &clean_command, &test_command, &doc_command}};
    }

    int execute() override { std::println("Use --help for available commands."); return 0; }
};

int main(int argc, char* argv[]) {
    RootCommand root;
    argman::CommandLineParser parser(root);

    try {
        return parser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::println(stderr, "{}: error: {}", root.info().name, e.what());
        return 1;
    }
}
