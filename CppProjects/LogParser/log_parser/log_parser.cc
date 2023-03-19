#include "LogParser/log_parser/log_parser.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <unordered_set>

#include "LogParser/log_parser/convert_value.h"
#include "LogParser/log_parser/float_str.h"
#include "thirdparty/util/log.h"

namespace log_parser {

namespace {
const int64_t kMinTimestamp = 1673503905;
const int64_t kMaxTimestamp = 4102415999;
const std::unordered_set<int64_t> kTelephoneHeaderSet = {130, 131, 132, 134};
const std::unordered_map<std::string, Metrics> kMetricsMap = {{"sum", Metrics::kSum},
                                                              {"max", Metrics::kMax},
                                                              {"min", Metrics::kMin},
                                                              {"mean", Metrics::kMean},
                                                              {"median", Metrics::kMedian}};
const std::unordered_map<DataType, std::string> kDataTypeMap = {
    {DataType::kInt, "Int"},
    {DataType::kFloat, "Float"},
    {DataType::kDate, "Date"},
    {DataType::kString, "String"},
    {DataType::kTelephone, "Telephone"},
    {DataType::kTimestamp, "Timestamp"}};
}  // namespace

LogParser::LogParser(const std::vector<std::vector<std::string>>& raw_content,
                     const std::unordered_map<std::string, size_t>& col_name2col_idx,
                     const std::unordered_map<size_t, DataType>& col_idx2col_data_type,
                     const std::vector<std::string>& col_name_list)
    : raw_content_(raw_content),
      is_from_file_(false),
      col_name_list_(col_name_list),
      col_name2col_idx_(col_name2col_idx),
      col_idx2col_data_type_(col_idx2col_data_type) {
  col_cnt_ = col_name_list_.size();
  if (col_cnt_ != col_name2col_idx.size() || col_cnt_ != col_idx2col_data_type_.size()) {
    raw_content_.clear();
    LOG_ERROR("invalid initial data for LogParser, clear all raw content to avoid crash");
  }
}

LogParser::LogParser(const std::string& log_file_path)
    : is_from_file_(true), file_path_(log_file_path) {
}

bool LogParser::Parse() {
  if (is_from_file_) {
    if (!this->ParseRawContentFromFile()) {
      return false;
    }
  } else {
    if (!this->ParseRawContent()) {
      return false;
    }
  }

  LOG_INFO("ParseRawContent done");

  if (!this->SortContent()) {
    return false;
  }
  LOG_INFO("SortContent done");

  if (!this->StoreMetrics()) {
    return false;
  }
  LOG_INFO("StoreMetrics done");
  return true;
}

bool LogParser::Calculate(const std::string& col_name, const std::string& metric_name,
                          std::string* const result) {
  auto col_iter = col_name2col_idx_.find(col_name);
  if (col_iter == col_name2col_idx_.end()) {
    LOG_ERROR("[Calculate]invalid column name [%s], please retry", col_name.c_str());
    return false;
  }
  size_t col_idx = col_iter->second;
  DataType data_type = col_idx2col_data_type_[col_idx];
  LOG_INFO("[Calculate]column name [%s], column index [%zu], metric name [%s]", col_name.c_str(),
           col_idx, metric_name.c_str());

  auto metric_iter = kMetricsMap.find(metric_name);
  if (metric_iter == kMetricsMap.end()) {
    LOG_ERROR("[Calculate]invalid metrics name [%s], please retry", col_name.c_str());
    return false;
  }
  Metrics metric = metric_iter->second;

  auto iter = metrics_data_by_col_[col_idx].find(metric);
  if (iter == metrics_data_by_col_[col_idx].end()) {
    LOG_ERROR("[Calculate]invalid aggregate metric [%s] for column [%s] that has data type[%s]",
              metric_name.c_str(), col_name.c_str(), kDataTypeMap.at(data_type).c_str());
    return false;
  }

  *result = iter->second;

  return true;
}

bool LogParser::Sort(const std::string& col_name, const int32_t limit, const bool is_asc,
                     const bool is_show_header,
                     std::vector<std::vector<std::string>>* const result) {
  auto iter = col_name2col_idx_.find(col_name);
  if (iter == col_name2col_idx_.end()) {
    LOG_ERROR("[Sort]invalid column name [%s], please retry", col_name.c_str());
    return false;
  }

  size_t col_idx = iter->second;
  LOG_INFO("[Sort]column name [%s], column index [%zu]", col_name.c_str(), col_idx);

  size_t actual_limit = limit;
  if (actual_limit >= row_cnt_) {
    LOG_INFO("[Sort]read max row count [%zu], list all the rows by order", row_cnt_);
    actual_limit = row_cnt_;
  }

  std::vector<std::vector<std::string>> temp;
  if (is_show_header) {
    temp.emplace_back(col_name_list_);
  }

  for (size_t row_idx = 0; row_idx < actual_limit; ++row_idx) {
    if (is_asc) {
      temp.emplace_back(raw_content_[sorted_content_by_col_[col_idx][row_idx]]);
    } else {
      temp.emplace_back(raw_content_[sorted_content_by_col_[col_idx][row_cnt_ - 1 - row_idx]]);
    }
  }

  temp.swap(*result);

  return true;
}

bool LogParser::ParseRawContent() {
  if (raw_content_.empty()) {
    LOG_ERROR("empty raw content to parse, quit");
    return false;
  }

  std::vector<std::vector<std::string>> parsed_raw_content;

  for (RowIndex row_idx = 0; row_idx < raw_content_.size(); ++row_idx) {
    bool is_mismatch_data_type = false;
    for (ColIndex col_idx = 0; col_idx < col_cnt_; ++col_idx) {
      if (this->ParseColumnDataType(raw_content_[row_idx][col_idx]) !=
          col_idx2col_data_type_[col_idx]) {
        is_mismatch_data_type = true;
        break;
      }
    }
    if (is_mismatch_data_type) {
      continue;
    }
    parsed_raw_content.emplace_back(raw_content_[row_idx]);
  }
  raw_content_.swap(parsed_raw_content);
  row_cnt_ = raw_content_.size();

  return true;
}

bool LogParser::ParseRawContentFromFile() {
  std::ifstream f(file_path_);
  if (!f.good()) {
    LOG_ERROR("log file [%s] don't exist, quit", file_path_.c_str());
    return false;
  }
  f.close();

  f.open(file_path_, std::ios::in);
  if (!f.is_open()) {
    LOG_ERROR("open log file [%s] fail, quit", file_path_.c_str());
    return false;
  }

  int32_t valid_line_cnt = 0;
  int32_t empty_line_cnt = 0;
  int32_t mismatch_col_cnt = 0;
  int32_t mismatch_data_type_cnt = 0;

  std::string line_content;
  std::string line_buff;
  while (std::getline(f, line_buff)) {
    if (line_buff.empty()) {
      ++empty_line_cnt;
      continue;
    }

    // parse column name from first line
    if (valid_line_cnt == 0) {
      this->ParseLineContent(line_buff, &col_name_list_);
      col_cnt_ = col_name_list_.size();
      if (col_cnt_ == 0) {
        LOG_ERROR("parse column name for log file [%s] fail, quit", file_path_.c_str());
        return false;
      }
      LOG_INFO("parse log file [%s] successfully with column count [%zu]", file_path_.c_str(),
               col_cnt_);
      ++valid_line_cnt;
      continue;
    }

    // parse data type from second line
    if (valid_line_cnt == 1) {
      std::vector<std::string> col_val_list;
      col_val_list.reserve(col_cnt_);
      this->ParseLineContent(line_buff, &col_val_list);
      if (col_val_list.size() != col_cnt_) {
        ++mismatch_col_cnt;
        continue;
      }
      LOG_INFO("parse log column names successfully:");
      for (size_t i = 0; i < col_cnt_; ++i) {
        col_name2col_idx_[col_name_list_[i]] = i;
        col_idx2col_data_type_[i] = this->ParseColumnDataType(col_val_list[i]);
        LOG_INFO("the data type of column [%s] is %d", col_name_list_[i].c_str(),
                 static_cast<int>(col_idx2col_data_type_[i]));
      }
    }

    // parse and store column value
    std::vector<std::string> col_val_list;
    col_val_list.reserve(col_cnt_);
    this->ParseLineContent(line_buff, &col_val_list);
    if (col_val_list.size() != col_cnt_) {
      ++mismatch_col_cnt;
      continue;
    }

    bool is_mismatch_data_type = false;
    for (size_t i = 0; i < col_val_list.size(); ++i) {
      if (this->ParseColumnDataType(col_val_list[i]) != col_idx2col_data_type_[i]) {
        is_mismatch_data_type = true;
        break;
      }
    }
    if (is_mismatch_data_type) {
      ++mismatch_data_type_cnt;
      continue;
    }

    valid_line_cnt++;
    raw_content_.emplace_back(col_val_list);
  }

  if (raw_content_.empty()) {
    LOG_ERROR("empty log file [%s], quit", file_path_.c_str());
    return false;
  }
  row_cnt_ = raw_content_.size();

  LOG_INFO(
      "parse log file done, valid_line:[%d] empty_line_cnt:[%d] mismatch_col_cnt:[%d] "
      "mismatch_data_type_cnt:[%d]",
      valid_line_cnt, empty_line_cnt, mismatch_col_cnt, mismatch_data_type_cnt);
  return true;
}

bool LogParser::SortContent() {
  sorted_content_by_col_.resize(col_cnt_);
  for (size_t col_idx = 0; col_idx < col_cnt_; ++col_idx) {
    sorted_content_by_col_[col_idx].resize(row_cnt_);
    for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
      sorted_content_by_col_[col_idx][row_idx] = row_idx;
    }
  }

  for (size_t col_idx = 0; col_idx < col_cnt_; ++col_idx) {
    std::vector<RowIndex>& col_data = sorted_content_by_col_[col_idx];

    switch (col_idx2col_data_type_[col_idx]) {
      case DataType::kInt: {
        std::vector<int64_t> col_int64_data(row_cnt_);
        for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
          col_int64_data[row_idx] = ConvertValue<int64_t>(raw_content_[row_idx][col_idx]);
        }
        std::sort(col_data.begin(), col_data.end(), [&col_int64_data](RowIndex a, RowIndex b) {
          return col_int64_data[a] < col_int64_data[b];
        });
        break;
      }
      case DataType::kTelephone:
      case DataType::kTimestamp:
      case DataType::kDate:
      case DataType::kString: {
        std::vector<std::string> col_str_data(row_cnt_);
        for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
          col_str_data[row_idx] = raw_content_[row_idx][col_idx];
        }
        std::sort(col_data.begin(), col_data.end(), [&col_str_data](RowIndex a, RowIndex b) {
          return col_str_data[a] < col_str_data[b];
        });
        break;
      }
      case DataType::kFloat: {
        std::vector<FloatStr> col_float_str_data;
        col_float_str_data.reserve(row_cnt_);
        for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
          col_float_str_data.emplace_back(raw_content_[row_idx][col_idx]);
        }

        std::sort(col_data.begin(), col_data.end(), [&col_float_str_data](RowIndex a, RowIndex b) {
          return col_float_str_data[a] < col_float_str_data[b];
        });
        break;
      }
    }
  }

  return true;
}

bool LogParser::StoreMetrics() {
  metrics_data_by_col_.resize(col_cnt_);
  for (size_t col_idx = 0; col_idx < col_cnt_; ++col_idx) {
    Metrics2Value& metrics2value = metrics_data_by_col_[col_idx];
    RowIndex min_row_index = sorted_content_by_col_[col_idx][0];
    RowIndex max_row_index = sorted_content_by_col_[col_idx][row_cnt_ - 1];
    RowIndex median_row_index = sorted_content_by_col_[col_idx][row_cnt_ / 2];
    metrics2value[Metrics::kMin] = raw_content_[min_row_index][col_idx];
    metrics2value[Metrics::kMax] = raw_content_[max_row_index][col_idx];
    metrics2value[Metrics::kMedian] = raw_content_[median_row_index][col_idx];

    switch (col_idx2col_data_type_[col_idx]) {
      case DataType::kInt: {
        std::vector<int64_t> int_val_list(row_cnt_);
        for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
          int_val_list[row_idx] = ConvertValue<int64_t>(raw_content_[row_idx][col_idx]);
        }
        int64_t sum = std::accumulate(int_val_list.begin(), int_val_list.end(), 0);
        metrics2value[Metrics::kSum] = std::to_string(sum);
        metrics2value[Metrics::kMean] = std::to_string(sum * 1.0 / row_cnt_);
        break;
      }
      case DataType::kFloat: {
        std::vector<float> float_val_list(row_cnt_);
        for (size_t row_idx = 0; row_idx < row_cnt_; ++row_idx) {
          float_val_list[row_idx] = ConvertValue<float>(raw_content_[row_idx][col_idx]);
        }
        float sum = std::accumulate(float_val_list.begin(), float_val_list.end(), 0.0);
        metrics2value[Metrics::kSum] = std::to_string(sum);
        metrics2value[Metrics::kMean] = std::to_string(sum / row_cnt_);
        break;
      }
      default:
        break;
    }
  }
  return true;
}

void LogParser::ParseLineContent(const std::string& content, std::vector<std::string>* const res) {
  std::stringstream ss(content);
  std::string col_val;
  while (std::getline(ss, col_val, '\t')) {
    res->emplace_back(col_val);
  }
}

DataType LogParser::ParseColumnDataType(const std::string& col_val) {
  char* p = nullptr;
  int64_t i = std::strtol(col_val.data(), &p, 10);

  if (p == col_val.data() + col_val.size()) {
    if (i >= kMinTimestamp && i <= kMaxTimestamp) {
      return DataType::kTimestamp;
    }
    if (col_val.size() == 11) {
      int32_t telephone_head = i / 1e8;
      if (kTelephoneHeaderSet.count(telephone_head) == 1) {
        return DataType::kTelephone;
      }
    }
    return DataType::kInt;
  }

  std::strtof(col_val.data(), &p);
  if (p == col_val.data() + col_val.size()) {
    return DataType::kFloat;
  }

  if (col_val.size() == 10 && col_val[4] == '-' && col_val[7] == '-') {
    return DataType::kDate;
  }

  return DataType::kString;
}

size_t LogParser::row_cnt() const {
  return row_cnt_;
}

size_t LogParser::col_cnt() const {
  return col_cnt_;
}

const std::vector<std::vector<std::string>>& LogParser::raw_content() const {
  return raw_content_;
}

const std::vector<std::string>& LogParser::col_name_list() const {
  return col_name_list_;
}

const std::unordered_map<std::string, size_t>& LogParser::col_name2col_idx() const {
  return col_name2col_idx_;
}

const std::unordered_map<size_t, DataType>& LogParser::col_idx2col_data_type() const {
  return col_idx2col_data_type_;
}

}  // namespace log_parser