#include "thirdparty/util/threadpool.h"

#include "gtest/gtest.h"

namespace util {

TEST(ThreadPool, Example) {
  ThreadPool threadpool(5);
  std::atomic<int> atomic_int{0};
  std::vector<std::future<void>> task_list;
  for (int32_t i = 0; i < 100; ++i) {
    task_list.push_back(threadpool.Enqueue([&atomic_int]() {
      ++atomic_int;
    }));
  }
  for (auto&& task : task_list) {
    task.wait();
  }
  EXPECT_EQ(atomic_int.operator int(), 100);
}

}  // namespace util