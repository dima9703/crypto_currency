#include "model.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <set>
#include <iomanip>
#include <map>
#include <math.h>
#include <unistd.h>
#include <random>
#include "json.hpp"
#include <stdio.h>
using json = nlohmann::json;

void WriteMatrixToCsv(matrix some_matrix, std::string filename) {
  // remove the file if already exists
  remove(filename.c_str());

  std::ofstream out_(filename);

  for (auto &row : some_matrix) {
    for (auto col : row) out_ << std::fixed << col << ',';
    out_ << '\n';
  }
}

matrix RandomMatrixInit(size_t n, size_t m) {
  matrix Y;
  std::mt19937 randomGenerator(std::random_device{}());
  std::normal_distribution<double> normal(0, 1);
  for (size_t iter = 0; iter < n; ++iter) {
    Y.emplace_back();
    for (size_t jiter = 0; jiter < m; ++jiter) {
      Y.back().push_back(normal(randomGenerator));
    }
  }
  return Y;
}

std::string ReplaceString(std::string subject, const std::string &search,
                          const std::string &replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

void SplitString(const std::string &fullstr, std::vector<std::string> &elements,
                 const std::string &delimiter) {
  std::string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
  std::string::size_type pos = fullstr.find_first_of(delimiter, lastpos);

  while ((std::string::npos != pos) || (std::string::npos != lastpos)) {
    elements.push_back(fullstr.substr(lastpos, pos - lastpos));
    lastpos = fullstr.find_first_not_of(delimiter, pos);
    pos = fullstr.find_first_of(delimiter, lastpos);
  }
}

double StringToDouble(const std::string &str) {
  std::istringstream stm;
  double val = 0;
  stm.str(str);
  stm >> val;
  return val;
}

void ReadCsvFile(const std::string &filename, const std::string &csvdelimiter,
                 std::vector<std::vector<double> > &sarr) {
  std::ifstream fin(filename.c_str());

  std::string s;
  std::vector<std::string> selements;
  std::vector<double> delements;

  while (!fin.eof()) {
    getline(fin, s);

    if (!s.empty()) {
      s = ReplaceString(s, ",", ".");
      SplitString(s, selements, csvdelimiter);

      for (size_t i = 0; i < selements.size(); i++) {
        delements.push_back(StringToDouble(selements[i]));
      }
      sarr.push_back(delements);
      selements.clear();
      delements.clear();
    }
  }
  sarr.erase(sarr.begin());
  fin.close();
}

void PrintMatrix(const matrix &mat) {
  for (size_t iter = 0; iter < mat.size(); ++iter) {
    for (size_t jter = 0; jter < mat[iter].size(); ++jter) {
      std::cout << mat[iter][jter] << " ";
    }
    std::cout << "\n";
  }
}

template <class T>
void PrintVector(const std::vector<T> &x) {
  for (size_t iter = 0; iter < x.size(); ++iter) {
    std::cout << x[iter] << " ";
  }
  std::cout << "\n";
}

void PrintMask(const mask &mat) {
  for (size_t iter = 0; iter < mat.size(); ++iter) {
    for (size_t jter = 0; jter < mat[iter].size(); ++jter) {
      std::cout << mat[iter][jter] << " ";
    }
    std::cout << "\n";
  }
}

mask InitMask(size_t n, size_t m) {
  mask init_mask;
  for (size_t iter = 0; iter < n; ++iter) {
    init_mask.emplace_back();
    // Y.emplace_back();
    for (size_t jiter = 0; jiter < m; ++jiter) {
      init_mask.back().push_back(true);
    }
  }
  return init_mask;
}

std::vector<double> GetRow(const matrix &X, size_t row_number) {
  std::vector<double> row;
  for (size_t iter = 0; iter < X[row_number].size(); ++iter) {
    row.push_back(X[row_number][iter]);
  }
  return row;
}

matrix Multiplication(const matrix &A, const matrix &B) {
  matrix C;
  size_t n = A.size();
  size_t m = A.back().size();
  size_t r = B.back().size();
  if (m == B.size()) {
    for (size_t x = 0; x < n; ++x) {
      C.emplace_back();
      for (size_t y = 0; y < r; ++y) {
        double value = 0;
        for (size_t z = 0; z < m; ++z) {
          value += A[x][z] * B[z][y];
        }
        C.back().push_back(value);
      }
    }
    return C;
  } else {
    std::cout << "\nError while multiplication " << A.size() << " x "
              << A.back().size() << " matrix on " << B.size() << " x "
              << B.back().size() << " matrix";
    return C;
  }
}

json ReadValidateInputConfig() {
  std::ifstream ifs("configuration.json");
  json input_config = json::parse(ifs);

  // Проверка допустимых значений частот.
  std::vector<std::string> available_freq = {"day", "fiveMin", "hour", "oneMin", "thirtyMin"};
  if (std::find(available_freq.begin(), available_freq.end(),
                input_config["frequency"]) == available_freq.end()) {
    std::cout << "frequency must be day, fiveMin, hour, oneMin or thirtyMin";
    throw 20;
  }

  //Проверка допустимых значений  процента имеющихся данных для обучения
  if (input_config["percentage_using_data"] < 1 ||
      input_config["percentage_using_data"] > 100) {
    std::cout << "percentage_using_data must be in [1; 100]";
    throw 20;
  }

  //Проверка latent_dimension
  if (input_config["latent_dimension"] < 1) {
    std::cout << "latent_dimension must be > 1";
    throw 20;
  }

  //Проверка bcgjk
  if (input_config["number_input_files"] < 1) {
    std::cout << "number_input_files must be > 1";
    throw 20;
  }

  //Проверка ARLags
  if (input_config["ARLags"] < 1) {
    std::cout << "ARLags must be > 1";
    throw 20;
  }

  //Проверка lambda_F
  if (input_config["lambda_F"] < 0) {
    std::cout << "lambda_F must be > 0";
    throw 20;
  }

  //Проверка lambda_x
  if (input_config["lambda_x"] < 0) {
    std::cout << "lambda_x must be > 0";
    throw 20;
  }

  //Проверка lambda_w
  if (input_config["lambda_w"] < 0) {
    std::cout << "lambda_w must be > 0";
    throw 20;
  }
  return input_config;
}

/*//Дать ссылку на папку и файл в зависимости от частоты использования. 
std::pair<std::string, std::string> GetPathFromFrequency(std::string frequncy) {
    std::pair<std::string, std::string> output;
    switch(frequncy) {
        case "day":
            output = std::make_pair("day", "day_1");
            return output;
        case "fiveMin":
            output = std::make_pair("fiveMin", "fiveMin_1");
            return output;
        case "hour":
            output = std::make_pair("hour", "hour_1");
            return output;
        case "oneMin":
            output = std::make_pair("oneMin", "oneMin_1");
            return output;
        case "thirtyMin":
            output = std::make_pair("thirtyMin", "thirtyMin_1");
            return output;
    }
}*/