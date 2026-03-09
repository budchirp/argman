module;

#include <algorithm>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

export module argman.option;

namespace argman {

template <typename T>
T from_string(const std::string& str);

template <>
inline std::string from_string<std::string>(const std::string& str) {
    return str;
}

template <>
inline int from_string<int>(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("invalid integer value: " + str);
    }
}

template <>
inline long from_string<long>(const std::string& str) {
    try {
        return std::stol(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("invalid long value: " + str);
    }
}

template <>
inline float from_string<float>(const std::string& str) {
    try {
        return std::stof(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("invalid float value: " + str);
    }
}

template <>
inline double from_string<double>(const std::string& str) {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("invalid double value: " + str);
    }
}

template <>
inline bool from_string<bool>(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    }
    if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }

    throw std::invalid_argument("invalid boolean value: " + str);
}

export using OptionValue = std::variant<std::string, int, bool, float, double>;

export class Option {
  public:
    std::string name;
    std::string description;
    bool is_flag;
    OptionValue value;

    Option(std::string name, std::string description, OptionValue value, bool is_flag = false)
        : name(std::move(name)), description(std::move(description)), is_flag(is_flag),
          value(std::move(value)) {}

    void parse(const std::string& str) {
        std::visit(
            [&str](auto& out) {
                using T = std::decay_t<decltype(out)>;
                out = from_string<T>(str);
            },
            value);
    }

    void set_flag() {
        if (std::holds_alternative<bool>(value)) {
            value = true;
        }
    }

    template <typename T>
    const T& get() const {
        return std::get<T>(value);
    }
};

} // namespace argman
