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
#include <stdio.h>

matrix PredictARMAs(const matrix& X, const std::vector<AR>& ars,
                    size_t future_count) {
  matrix prediction;
  for (size_t row = 0; row < X.size(); ++row) {
    std::vector<double> time_series = GetRow(X, row);
    prediction.push_back(ars[row].predict(time_series, future_count));
  }
  return prediction;
}

void CryptoData::predict() {
  size_t future_predictions = 30;
  matrix X_predicted = PredictARMAs(X, CryptoData::ARs, future_predictions);
  matrix prediction = Multiplication(CryptoData::F, X_predicted);
  WriteMatrixToCsv(prediction, "prediction.csv");
  std::cout << "Predicted and saved to file \n";
}
