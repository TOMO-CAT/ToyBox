#pragma once

#include <cstdint>
#include <string>

namespace log_parser {

// FloatStr: 浮点数的字符串表示, 用于解决精度损失的问题
class FloatStr final {
 public:
  explicit FloatStr(const std::string& raw_str);
  ~FloatStr();

 public:
  bool is_valid() const;
  int64_t integer() const;
  int64_t fraction() const;
  float float_val() const;
  const std::string& raw() const;

 private:
  std::string raw_;
  bool is_valid_ = false;
  float float_val_ = 0;
  int64_t integer_ = 0;
  int64_t fraction_ = 0;
};

bool operator<(const FloatStr& a, const FloatStr& b);
bool operator>(const FloatStr& a, const FloatStr& b);

}  // namespace log_parser