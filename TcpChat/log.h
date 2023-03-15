#pragma once

#include <iostream>

#define LOG_ERROR(fmt, args...)                                                                \
  do {                                                                                         \
    fprintf(stderr, "[TcpChat::Error][%s:%d][%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, \
            ##args);                                                                           \
  } while (0)

#define LOG_INFO(fmt, args...)                                                                \
  do {                                                                                        \
    fprintf(stdout, "[TcpChat::Info][%s:%d][%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, \
            ##args);                                                                          \
  } while (0)
