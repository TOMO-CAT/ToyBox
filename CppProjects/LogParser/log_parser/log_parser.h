#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "thirdparty/util/macros.h"

namespace log_parser {

enum class DataType {
  kInt = 1,
  kFloat = 2,
  kString = 3,
  kTelephone = 4,
  kDate = 5,
  kTimestamp = 6,
};

enum class Metrics {
  kSum = 1,
  kMax = 2,
  kMin = 3,
  kMean = 4,
  kMedian = 5,
};

class LogParserMgr;

class LogParser {
 public:
  LogParser(const std::vector<std::vector<std::string>>& raw_content,
            const std::unordered_map<std::string, size_t>& col_name2col_idx,
            const std::unordered_map<size_t, DataType>& col_idx2col_data_type,
            const std::vector<std::string>& col_name_list);
  explicit LogParser(const std::string& log_file_path);

 public:
  /**
   * @brief 解析日志文件, 将结果存储到成员变量
   *
   * @return true
   * @return false
   */
  bool Parse();

  /**
   * @brief 计算给定 column 的指标
   *
   * @param col_name 列名
   * @param metric_name 指标名, 包括 sum、max、min、mean 和 median
   * @param result 指标结果
   * @return true
   * @return false
   */
  bool Calculate(const std::string& col_name, const std::string& metric_name,
                 std::string* const result);

  /**
   * @brief 返回给定 column 的排序结果
   *
   * @param col_name 列名
   * @param limit 返回数据的长度
   * @param is_asc 是否升序排序
   * @param is_show_header 是否显示列名
   * @param result 返回结果
   * @return true
   * @return false
   */
  bool Sort(const std::string& col_name, const int32_t limit, const bool is_asc,
            const bool is_show_header, std::vector<std::vector<std::string>>* const result);

 private:
  bool ParseRawContentFromFile();
  bool ParseRawContent();
  bool SortContent();
  bool StoreMetrics();

 private:
  static void ParseLineContent(const std::string& content, std::vector<std::string>* const res);
  static DataType ParseColumnDataType(const std::string& col_val);

 private:
  using RowIndex = size_t;
  using ColIndex = size_t;
  using Metrics2Value = std::unordered_map<Metrics, std::string>;
  using SortedColumn = std::vector<RowIndex>;

 private:
  std::vector<std::vector<std::string>> raw_content_;
  std::vector<SortedColumn> sorted_content_by_col_;
  std::vector<Metrics2Value> metrics_data_by_col_;

  bool is_from_file_ = false;
  std::string file_path_;
  size_t col_cnt_ = 0;
  size_t row_cnt_ = 0;
  std::vector<std::string> col_name_list_;
  std::unordered_map<std::string, size_t> col_name2col_idx_;
  std::unordered_map<size_t, DataType> col_idx2col_data_type_;

 protected:
  size_t row_cnt() const;
  size_t col_cnt() const;
  const std::vector<std::vector<std::string>>& raw_content() const;
  const std::vector<std::string>& col_name_list() const;
  const std::unordered_map<std::string, size_t>& col_name2col_idx() const;
  const std::unordered_map<size_t, DataType>& col_idx2col_data_type() const;

 private:
  friend class LogParserMgr;

 private:
  DISALLOW_COPY_AND_ASSIGN(LogParser);
};

}  // namespace log_parser