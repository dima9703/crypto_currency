#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>

#include <dirent.h>
#include <stdio.h>
#include "json.hpp"
using json = nlohmann::json;

// Используемые структуры для того, чтобы хранить данные и маску данных.
typedef std::vector<std::vector<double> > matrix;
typedef std::vector<std::vector<bool> > mask;

// Заголовки  функций, который используются в разных контекстах
void WriteMatrixToCsv(matrix some_matrix, std::string filename);
matrix RandomMatrixInit(size_t n, size_t m);
void ReadCsvFile(const std::string &filename, const std::string &csvdelimiter,
                 std::vector<std::vector<double> > &sarr);
std::vector<double> GetRow(const matrix &X, size_t row_number);
matrix Multiplication(const matrix &A, const matrix &B);

template <typename T>
T Transpose(const T &q) {
  T A;
  size_t n = q.size();
  size_t m = q.back().size();
  for (size_t i = 0; i < m; i++) {
    A.emplace_back();
    for (size_t j = 0; j < n; j++) {
      A.back().push_back(q[j][i]);
    }
  }
  return A;
}

std::vector<double> RidgeRegression(const matrix &Z,
                                    const std::vector<double> &Y,
                                    double lambda);  // в train.cpp
json ReadValidateInputConfig();

// Класс AR-модели. Подробные функции есть в файле AR.cpp
class AR {
 private:
  size_t lags_;
  std::vector<double> coefs_;
  bool fit_intercept_ = false;
  double lambda_;

 public:
  AR() : lags_(0), lambda_(0) {}
  explicit AR(size_t lags) : lags_(lags), lambda_(0) {}
  explicit AR(size_t lags, double lambda) : lags_(), lambda_(lambda) {}

  size_t lags() const { return lags_; }

  double lambda() const { return lambda_; }

  std::vector<double> coefs() const { return coefs_; }

  void fit(const std::vector<double> &time_series, size_t lags = 0,
           double lambda = 0, bool fit_intercept = false);

  std::vector<double> predict(const std::vector<double> &time_series,
                              size_t future_count) const;
};

// Ключевой класс данной модели. Важные функции - train, init, predict разложены
// в разные файлы.
class CryptoData {
 private:
  int64_t qwer;
  matrix Y_;
  mask init_mask_;
  size_t latent_dimension_;
  size_t max_iter_;
  size_t lags_;

  bool fit_intercept_ = false;
  double lambda_F_;
  double lambda_x_;
  double lambda_w_;

  std::vector<int64_t> used_timestamps;
  matrix F;
  matrix X;

  std::vector<AR> ARs;

 public:
  //CryptoData();
  void Initialize(json input_config);
  void train();
  void save_model();
  void predict();
};
