#pragma once

#include <list>
#include <string>

namespace util {

bool Split(const std::string& content, const char delim, std::list<std::string>* const results);

}