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

using json = nlohmann::json;

std::vector<std::string> GetFilesInDir(std::string dir) {
  std::vector<std::string> files;
  DIR* dp;
  struct dirent* dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    std::cout << "Error(" << errno << ") opening " << dir << "\n";
    throw 20;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (std::string(dirp->d_name).length() > 3) {
      files.push_back(std::string(dirp->d_name));
    }
  }
  closedir(dp);
  return files;
}

// void CryptoData::Initialize(std::string begin_date, std::string end_date,
//         std::string frequency, uint64_t ar_lags, int64_t max_iter,
//        uint64_t latent_dimension) {
//   int a = 6;
// }

std::vector<int64_t> ab() {
  std::vector<int64_t> av;
  av.push_back(10);
  av.push_back(10);
  return av;
}

void CryptoData::Initialize(json input_config) {
  CryptoData::max_iter_ = (size_t)input_config["max_iterations"];
  int64_t percentage_data = (int64_t)input_config["percentage_using_data"];
  size_t max_files_number = (size_t)input_config["number_input_files"];
  CryptoData::lags_ = (size_t)input_config["ARLags"];
  CryptoData::latent_dimension_ = (size_t)input_config["latent_dimension"];
  CryptoData::lambda_F_ = (double)input_config["lambda_F"];
  CryptoData::lambda_x_ = (double)input_config["lambda_x"];
  CryptoData::lambda_w_ = (double)input_config["lambda_w"];
  std::string frequency = input_config["frequency"];

  std::vector<int> necessary_columns = input_config["necessary_columns"];

  std::string dir = std::string("../data");
  std::vector<std::string> files = GetFilesInDir(dir);
  std::vector<std::vector<std::vector<double>>> all_data;
  for (size_t i = 0; i < max_files_number; i++) {
    std::string path = "../data/" + files[i] + "/" + frequency + "/" +
                       files[i] + "_" + frequency + "_1.csv";
    std::vector<std::vector<double>> sarr;
    ReadCsvFile(path, ";", sarr);
    all_data.push_back(sarr);
  }

  std::set<double> timestamps;
  for (uint64_t i = 0; i < all_data.size(); i++) {
    for (uint64_t j = 0; j < all_data[i].size(); j++) {
      if (all_data[i][j][0] > 1) {
        timestamps.insert((int64_t)all_data[i][j][0]);
      }
    }
  }

  std::vector<int64_t> timestamps_vec(timestamps.begin(), timestamps.end());
  std::sort(timestamps_vec.begin(), timestamps_vec.end());

  // До какого элемента добавляем в таблицу
  size_t last_element_index =
      (size_t)timestamps_vec.size() * percentage_data / 100;
  std::cout << "last element index " << last_element_index << "\n";
  int64_t last_timestamp = timestamps_vec[last_element_index];

  // Запишем все имеющиеся timestamp'ы в CSV файл первой строкой
  std::cout << "time length " << timestamps_vec.size() << "\n";
  matrix timestamps_vec_to_csv;
  timestamps_vec_to_csv.emplace_back();
  for (size_t iter = 0; iter < last_element_index; ++iter) {
    timestamps_vec_to_csv.back().push_back((double)timestamps_vec[iter]);
  }
  WriteMatrixToCsv(timestamps_vec_to_csv, "timestamps.csv");

  std::vector<std::multimap<int64_t, std::vector<double>>> map_tables;
  for (uint64_t i = 0; i < all_data.size(); i++) {
    std::multimap<int64_t, std::vector<double>> ab;
    for (uint64_t j = 0; j < all_data[i].size(); j++) {
      std::vector<double> input_necessary_columns;
      for (size_t index = 0; index < necessary_columns.size(); index++) {
        int64_t column_id = necessary_columns[index] + 1;
        input_necessary_columns.push_back(all_data[i][j][column_id]);
        // std::cout << necessary_columns.size() << "\n";
      }
      ab.insert(std::pair<int64_t, std::vector<double>>(
          all_data[i][j][0], input_necessary_columns));
      /* ab.insert(std::pair<int64_t, std::vector<double>>(
           all_data[i][j][0],
       {all_data[i][j][2], all_data[i][3], all_data[i][4], all_data[i][j][5],
       all_data[i][j][6], all_data[i][j][7]}));*/
    }
    map_tables.push_back(ab);
  }

  matrix final_matrix;
  mask exists_element;

  for (uint64_t iter = 0; iter < timestamps_vec.size(); iter++) {
    std::vector<double> current_array;
    std::vector<bool> current_array_exists;
    for (uint64_t jiter = 0; jiter < map_tables.size(); jiter++) {
      std::vector<double> sub_current_array;
      std::vector<int> sub_current_array_exists;

      auto search = map_tables[jiter].find(timestamps_vec[iter]);
      if (search != map_tables[jiter].end()) {
        sub_current_array = search->second;
        std::vector<int> temporary_vector(necessary_columns.size(), 1);
        sub_current_array_exists = temporary_vector;
      } else {
        std::vector<double> temporary_vector(necessary_columns.size(), 0);
        sub_current_array = temporary_vector;

        std::vector<int> temporary_vector_mask(necessary_columns.size(), 0);
        sub_current_array_exists = temporary_vector_mask;
      }
      current_array.insert(current_array.end(), sub_current_array.begin(),
                           sub_current_array.end());

      current_array_exists.insert(current_array_exists.end(),
                                  sub_current_array_exists.begin(),
                                  sub_current_array_exists.end());
    }

    final_matrix.push_back(current_array);
    exists_element.push_back(current_array_exists);
  }

  matrix final_matrix_a(final_matrix.begin(),
                        final_matrix.begin() + last_element_index);
  mask exists_element_a(exists_element.begin(),
                        exists_element.begin() + last_element_index);

  /*final_matrix = Transpose(final_matrix_a);
  exists_element = Transpose(exists_element_a);*/

  Y_ = Transpose(final_matrix_a);
  init_mask_ = Transpose(exists_element_a);
  // Y_ = Transpose(final_matrix);
  // init_mask_ = Transpose(exists_element);

  // запись матрицы с данными в файл

  std::ofstream out("matrix.csv");
  for (auto& row : Y_) {
    for (auto col : row) out << std::setprecision(51) << col << ';';
    out << '\n';
  }

  // запись маски наличия данных в файл
  std::ofstream out_("mask.csv");

  for (auto& row : init_mask_) {
    for (auto col : row) out_ << std::setprecision(51) << col << ';';
    out_ << '\n';
  }

  std::cout << "Initialized: nrows " << final_matrix.size() << "\tncols "
            << final_matrix[0].size() << "\n";
  std::cout << "Y dimension nrows: " << Y_.size() << "\t";
  std::cout << "ncols " << Y_[0].size() << "\n";

  F = RandomMatrixInit(Y_.size(), latent_dimension_);
  X = RandomMatrixInit(latent_dimension_, Y_[0].size());
}


