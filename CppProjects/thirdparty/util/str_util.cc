#include "thirdparty/util/str_util.h"

#include <sstream>

namespace util {

bool Split(const std::string& content, const char delim, std::list<std::string>* const results) {
  if (results == nullptr) {
    return false;
  }

  results->clear();
  std::istringstream iss(content);
  std::string value;
  while (!iss.eof()) {
    std::getline(iss, value, delim);
    results->emplace_back(value);
  }
  return true;
}

}  // namespace util