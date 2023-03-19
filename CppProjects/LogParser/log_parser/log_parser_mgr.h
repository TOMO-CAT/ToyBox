#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "LogParser/log_parser/log_parser.h"
#include "thirdparty/util/macros.h"
#include "thirdparty/util/threadpool.h"

namespace log_parser {

// LogParserMgr: LogParser 管理器, 支持并发解析多份日志文件
class LogParserMgr final {
 public:
  LogParserMgr(size_t parse_thread_cnt, const std::string& log_dir);
  LogParserMgr(size_t parse_thread_cnt, const std::vector<std::string>& log_file_path_list);
  ~LogParserMgr();

 public:
  bool Parse();
  bool CalculateAllFiles(const std::string& col_name, const std::string& metric_name,
                         std::string* const result);
  bool SortAllFiles(const std::string& col_name, const int32_t limit, const bool is_asc,
                    const bool is_show_header, std::vector<std::vector<std::string>>* const result);
  bool CalculateSingleFile(const std::string& file_path, const std::string& col_name,
                           const std::string& metric_name, std::string* const result);
  bool SortSingleFile(const std::string& file_path, const std::string& col_name,
                      const int32_t limit, const bool is_asc, const bool is_show_header,
                      std::vector<std::vector<std::string>>* const result);

 private:
  util::ThreadPool parse_threadpool_;
  std::shared_ptr<LogParser> merged_log_parser_ = nullptr;

  std::vector<std::string> log_file_path_list_;
  std::vector<std::shared_ptr<LogParser>> log_parser_list_;
  std::unordered_map<std::string, std::shared_ptr<LogParser>> log_name2log_parser_;
  std::vector<std::string> col_name_list_;

 private:
  DISALLOW_COPY_AND_ASSIGN(LogParserMgr);
};

}  // namespace log_parser