#include "LogParser/log_parser/convert_value.h"

namespace log_parser {

template <>
int64_t ConvertValue(const std::string& val) {
  return std::atoll(val.data());
}

template <>
float ConvertValue(const std::string& val) {
  return std::strtof(val.data(), nullptr);
}

}  // namespace log_parser