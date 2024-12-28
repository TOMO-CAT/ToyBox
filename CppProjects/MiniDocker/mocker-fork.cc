/**
 * @file mocker-fork.cc
 * @author k
 * @brief
 * @version 0.1
 * @date 2024-08-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/wait.h>
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

static void run_child(int /*argc*/, char** argv) {
  if (::execvp(argv[0], argv)) {
    std::cerr << "Fail to exec" << std::endl;
  }
}

static void run(int argc, char** argv) {
  std::cout << "Running " << cmd(argc, argv) << std::endl;

  pid_t child_pid = ::fork();

  if (child_pid < 0) {
    std::cerr << "Fail to fork" << std::endl;
    return;
  }

  if (child_pid) {
    if (::waitpid(child_pid, nullptr, 0) < 0) {
      std::cerr << "Fail to wait for child" << std::endl;
    } else {
      std::cout << "Child terminated" << std::endl;
    }
  } else {
    run_child(argc, argv);
  }
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