module;

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module argman.command;

import argman.option;

namespace argman {

export class Command {
  public:
    class Info {
      public:
        std::string name;
        std::string description;
        std::vector<Option> options = {};
        std::vector<Command*> commands = {};
    };

  public:
    std::unordered_map<std::string, Option> options;

    Command() = default;
    virtual ~Command() = default;

    virtual Info info() = 0;
    virtual void execute() = 0;

    template <typename T>
    const T& get(const std::string& name) const {
        auto it = options.find(name);
        if (it == options.end()) {
            throw std::invalid_argument("unknown option: " + name);
        }
        return it->second.template get<T>();
    }
};

} // namespace argman
