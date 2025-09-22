#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>

using namespace std;

namespace ArgMan {

template <typename T> T from_string(const string &str);

template <> inline string from_string<string>(const string &str) { return str; }

template <> inline int from_string<int>(const string &str) {
  try {
    return stoi(str);
  } catch (const exception &) {
    throw invalid_argument("invalid integer value: " + str);
  }
}

template <> inline long from_string<long>(const string &str) {
  try {
    return stol(str);
  } catch (const exception &) {
    throw invalid_argument("invalid long value: " + str);
  }
}

template <> inline float from_string<float>(const string &str) {
  try {
    return stof(str);
  } catch (const exception &) {
    throw invalid_argument("invalid float value: " + str);
  }
}

template <> inline double from_string<double>(const string &str) {
  try {
    return stod(str);
  } catch (const exception &) {
    throw invalid_argument("invalid double value: " + str);
  }
}

template <> inline bool from_string<bool>(const string &str) {
  string lower_str = str;
  transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);

  if (lower_str == "true" || lower_str == "1" || lower_str == "yes" ||
      lower_str == "on") {
    return true;
  }
  if (lower_str == "false" || lower_str == "0" || lower_str == "no" ||
      lower_str == "off") {
    return false;
  }

  throw invalid_argument("invalid boolean value: " + str);
}

} // namespace ArgMan