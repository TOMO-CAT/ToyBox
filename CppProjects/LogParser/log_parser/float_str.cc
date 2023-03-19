#include "LogParser/log_parser/float_str.h"

#include <list>
#include <string>

#include "thirdparty/util/str_util.h"

namespace log_parser {

FloatStr::FloatStr(const std::string& raw_str) : raw_(raw_str) {
  auto FloatParser = [](const std::string& str, float* const out) -> bool {
    char* p = nullptr;
    float float_val = std::strtof(str.data(), &p);
    if (p != str.data() + str.size()) {
      return false;
    }
    *out = float_val;
    return true;
  };

  auto IntParser = [](const std::string& str, int64_t* const out) -> bool {
    char* p = nullptr;
    int64_t int_val = std::strtol(str.data(), &p, 10);
    if (p != str.data() + str.size()) {
      return false;
    }
    *out = int_val;
    return true;
  };

  if (!FloatParser(raw_str, &float_val_)) {
    is_valid_ = false;
    return;
  }

  std::list<std::string> temp_list;
  util::Split(raw_str, '.', &temp_list);

  switch (temp_list.size()) {
    case 1:
      if (IntParser(*temp_list.begin(), &integer_)) {
        is_valid_ = true;
      } else {
        is_valid_ = false;
      }
      break;
    case 2:
      if (IntParser(*temp_list.begin(), &integer_) &&
          IntParser(*(++temp_list.begin()), &fraction_)) {
        is_valid_ = true;
      } else {
        is_valid_ = false;
      }
      break;
    default:
      is_valid_ = false;
      break;
  }
}

FloatStr::~FloatStr() = default;

bool FloatStr::is_valid() const {
  return is_valid_;
}

int64_t FloatStr::integer() const {
  return integer_;
}

int64_t FloatStr::fraction() const {
  return fraction_;
}

float FloatStr::float_val() const {
  return float_val_;
}

const std::string& FloatStr::raw() const {
  return raw_;
}

bool operator<(const FloatStr& a, const FloatStr& b) {
  if (!a.is_valid() || !b.is_valid()) {
    return false;
  }
  return a.float_val() < b.float_val();
}

bool operator>(const FloatStr& a, const FloatStr& b) {
  if (!a.is_valid() || !b.is_valid()) {
    return false;
  }
  return a.float_val() > b.float_val();
}

}  // namespace log_parser