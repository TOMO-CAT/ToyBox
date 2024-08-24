/**
 * @file mocker.cc
 * @author k
 * @brief
 * @version 0.1
 * @date 2024-08-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

static std::string cmd(int argc, char** argv) {
  std::string cmd;
  for (int i = 0; i < argc; ++i) {
    cmd.append(argv[i] + std::string(""));
  }
  return cmd;
}

static void run(int argc, char** argv) {
  std::cout << "Running " << cmd(argc, argv) << std::endl;
  ::execvp(argv[0], argv);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "too few arguments" << std::endl;
    exit(-1);
  }
  if (!::strcmp(argv[1], "run")) {
    run(argc - 2, &argv[2]);
  }
}
