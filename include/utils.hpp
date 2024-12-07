#pragma once

#include <stdexcept>
#include <string>

using namespace std;

namespace ArgMan {

template <typename T> T from_string(const string &str);

template <> inline string from_string<string>(const string &str) { return str; }

template <> inline int from_string<int>(const string &str) { return stoi(str); }

template <> inline bool from_string<bool>(const string &str) {
  if (str == "true" || str == "1")
    return true;
  if (str == "false" || str == "0")
    return false;

  throw invalid_argument("invalid boolean value: " + str);
}

} // namespace ArgMan
