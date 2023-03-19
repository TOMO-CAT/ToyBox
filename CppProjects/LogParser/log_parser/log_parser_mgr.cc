#include "LogParser/log_parser/log_parser_mgr.h"

#include <dirent.h>

#include <utility>

#include "thirdparty/util/log.h"

namespace log_parser {

namespace {
const char kCurrentDirectory[] = ".";
const char kParentDirectory[] = "..";
}  // namespace

LogParserMgr::LogParserMgr(size_t parse_thread_cnt,
                           const std::vector<std::string>& log_file_path_list)
    : parse_threadpool_(parse_thread_cnt), log_file_path_list_(log_file_path_list) {
}

LogParserMgr::LogParserMgr(size_t parse_thread_cnt, const std::string& log_dir)
    : parse_threadpool_(parse_thread_cnt) {
  auto list_files = [](const std::string& log_dir, std::vector<std::string>* const file_list) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = ::opendir(log_dir.c_str())) != nullptr) {
      while ((ent = ::readdir(dir)) != nullptr) {
        std::string file_path = ent->d_name;
        if (file_path == kCurrentDirectory || file_path == kParentDirectory) {
          continue;
        }
        file_list->emplace_back(log_dir + "/" + file_path);
        LOG_INFO("add log [%s] to LogParserMgr", file_list->back().c_str());
      }
      ::closedir(dir);
    } else {
      LOG_ERROR("open log directory [%s] fail", log_dir.c_str());
    }
  };

  list_files(log_dir, &log_file_path_list_);
}

LogParserMgr::~LogParserMgr() = default;

bool LogParserMgr::Parse() {
  // generate all log parser
  std::vector<std::string> temp_log_file_path_list;
  std::vector<std::shared_ptr<LogParser>> temp_log_parser_list;
  std::vector<std::vector<std::string>> temp_all_raw_content;

  std::vector<std::future<bool>> parse_future_list;
  std::vector<bool> parse_res_list;
  for (const auto& file_path : log_file_path_list_) {
    auto log_parser_sptr = std::make_shared<LogParser>(file_path);
    log_parser_list_.emplace_back(log_parser_sptr);
    parse_future_list.emplace_back(parse_threadpool_.Enqueue([log_parser_sptr]() -> bool {
      if (log_parser_sptr->Parse() && log_parser_sptr->row_cnt() > 0) {
        return true;
      }
      return false;
    }));
  }

  for (auto& future : parse_future_list) {
    parse_res_list.emplace_back(future.get());
  }

  for (size_t i = 0; i < log_file_path_list_.size(); ++i) {
    const std::string& file_path = log_file_path_list_[i];
    auto log_parser_sptr = log_parser_list_[i];

    if (parse_res_list[i]) {
      temp_log_file_path_list.emplace_back(file_path);
      temp_log_parser_list.emplace_back(log_parser_sptr);
      log_name2log_parser_[file_path] = log_parser_sptr;
      temp_all_raw_content.insert(temp_all_raw_content.end(),
                                  log_parser_sptr->raw_content().begin(),
                                  log_parser_sptr->raw_content().end());
    } else {
      LOG_ERROR("parse log file [%s] fail", file_path.c_str());
    }
  }

  log_file_path_list_.swap(temp_log_file_path_list);
  log_parser_list_.swap(temp_log_parser_list);

  // merge all log parser
  if (log_parser_list_.empty()) {
    LOG_ERROR("the number of log parser is zero, quit");
    return false;
  }
  if (temp_all_raw_content.empty()) {
    LOG_ERROR("empty content to parse, quit");
    return false;
  }
  std::shared_ptr<LogParser> first_parser = log_parser_list_[0];
  merged_log_parser_ = std::make_shared<LogParser>(
      std::move(temp_all_raw_content), first_parser->col_name2col_idx(),
      first_parser->col_idx2col_data_type(), first_parser->col_name_list());
  if (!merged_log_parser_->Parse()) {
    LOG_ERROR("merge all log parser fail, quit");
    return false;
  }
  LOG_INFO("generate [%zu] log parser and merge them successfully, total line count is [%zu]",
           log_parser_list_.size(), merged_log_parser_->row_cnt());

  return true;
}

bool LogParserMgr::CalculateAllFiles(const std::string& col_name, const std::string& metric_name,
                                     std::string* const result) {
  if (merged_log_parser_ == nullptr) {
    LOG_ERROR("merged_log_parser_ is nullptr");
    return false;
  }
  return merged_log_parser_->Calculate(col_name, metric_name, result);
}

bool LogParserMgr::SortAllFiles(const std::string& col_name, const int32_t limit, const bool is_asc,
                                const bool is_show_header,
                                std::vector<std::vector<std::string>>* const result) {
  if (merged_log_parser_ == nullptr) {
    LOG_ERROR("merged_log_parser_ is nullptr");
    return false;
  }
  return merged_log_parser_->Sort(col_name, limit, is_asc, is_show_header, result);
}

bool LogParserMgr::CalculateSingleFile(const std::string& file_path, const std::string& col_name,
                                       const std::string& metric_name, std::string* const result) {
  auto iter = log_name2log_parser_.find(file_path);
  if (iter == log_name2log_parser_.end() || iter->second == nullptr) {
    LOG_ERROR("invalid file path, please retry");
    return false;
  }
  return iter->second->Calculate(col_name, metric_name, result);
}
bool LogParserMgr::SortSingleFile(const std::string& file_path, const std::string& col_name,
                                  const int32_t limit, const bool is_asc, const bool is_show_header,
                                  std::vector<std::vector<std::string>>* const result) {
  auto iter = log_name2log_parser_.find(file_path);
  if (iter == log_name2log_parser_.end() || iter->second == nullptr) {
    LOG_ERROR("invalid file path, please retry");
    return false;
  }
  return iter->second->Sort(col_name, limit, is_asc, is_show_header, result);
}

}  // namespace log_parser