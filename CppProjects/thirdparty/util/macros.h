#pragma once

namespace util {

#if LANG_CXX11
#define DISALLOW_COPY_AND_ASSIGN(classname) \
  classname(const classname&) = delete;     \
  void operator=(const classname&) = delete
#else
#define DISALLOW_COPY_AND_ASSIGN(classname) \
 private:                                   \
  classname(const classname&);              \
  void operator=(const classname&)
#endif

}  // namespace util