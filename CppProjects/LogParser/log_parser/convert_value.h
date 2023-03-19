#pragma once

#include <string>

namespace log_parser {

template <typename T>
T ConvertValue(const std::string& val) {
  return T();
}

template <>
int64_t ConvertValue(const std::string& val);

template <>
float ConvertValue(const std::string& val);

}  // namespace log_parser