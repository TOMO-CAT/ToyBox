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

#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "sys/mount.h"

static std::string cmd(int argc, char** argv) {
  std::string cmd;
  for (int i = 0; i < argc; ++i) {
    cmd.append(argv[i] + std::string(""));
  }
  return cmd;
}

static void run_child(int argc, char** argv) {
  std::cout << "Child running " << cmd(argc, argv) << " as " << ::getpid() << std::endl;

  int flags = CLONE_NEWUTS;
  if (::unshare(flags) < 0) {
    std::cerr << "Fail to unshare in child" << std::endl;
    exit(-1);
  }

  if (::chroot("../ubuntu-fs") < 0) {
    std::cerr << "Fail to chroot" << std::endl;
    exit(-1);
  }

  if (chdir("/") < 0) {
    std::cerr << "Fail to chdir to /" << std::endl;
    exit(-1);
  }

  if (mount("proc", "proc", "proc", 0, nullptr) < 0) {
    std::cerr << "Fail to mount /proc" << std::endl;
    exit(-1);
  }

  const char* kChildHostname = "container";
  if (::sethostname(kChildHostname, strlen(kChildHostname)) < 0) {
    std::cerr << "Fail to set child hostname" << std::endl;
    exit(-1);
  }

  if (::execvp(argv[0], argv)) {
    std::cerr << "Fail to exec" << std::endl;
  }
}

static void run(int argc, char** argv) {
  std::cout << "Parent running " << cmd(argc, argv) << " as " << ::getpid() << std::endl;

  if (unshare(CLONE_NEWPID) < 0) {
    std::cerr << "Fail to unshare PID namespace" << std::endl;
    exit(-1);
  }

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
