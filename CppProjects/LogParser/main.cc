#include <cstdlib>
#include <iostream>

#include "LogParser/log_parser/log_parser_mgr.h"
#include "thirdparty/util/log.h"

int main() {
  log_parser::LogParserMgr log_parser_manager(10, "./test_log");
  if (!log_parser_manager.Parse()) {
    LOG_INFO("parse fail, quit");
    exit(EXIT_FAILURE);
  }

  // calculate merged metrics
  std::string merged_cal_res;
  std::vector<std::string> metrics({"sum", "min", "max", "mean", "median"});
  for (auto&& metric : metrics) {
    log_parser_manager.CalculateAllFiles("FLOAT", metric, &merged_cal_res);
    std::cout << "calculate merged result of " << metric << " : " << merged_cal_res << std::endl;
  }

  // merged sort
  std::vector<std::vector<std::string>> merged_sort_result;
  log_parser_manager.SortAllFiles("DATE", 10, true, true, &merged_sort_result);
  std::cout << "Sort Result: " << std::endl;
  for (auto&& row : merged_sort_result) {
    for (auto&& item : row) {
      std::cout << "\t" << item;
    }
    std::cout << std::endl;
  }

  // calculate metrics for single file
  std::string single_cal_res;
  for (auto&& metric : metrics) {
    log_parser_manager.CalculateSingleFile("./test_log/1679222052326.log", "FLOAT", metric,
                                           &single_cal_res);
    std::cout << "calculate merged result of " << metric << " : " << single_cal_res << std::endl;
  }

  // sort for single file
  std::vector<std::vector<std::string>> single_sort_result;
  log_parser_manager.SortSingleFile("./test_log/1679222053239.log", "DATE", 10, true, true,
                                    &single_sort_result);
  std::cout << "Sort Result: " << std::endl;
  for (auto&& row : single_sort_result) {
    for (auto&& item : row) {
      std::cout << "\t" << item;
    }
    std::cout << std::endl;
  }
}
