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

void AR::fit(const std::vector<double>& time_series, size_t lags, double lambda,
             bool fit_intercept) {
  AR::lags_ = lags;
  if (lags > 0) {
    AR::lags_ = lags;
  }

  matrix features;
  std::vector<double> target;
  AR::lambda_ = lambda;
  AR::fit_intercept_ = fit_intercept;
  size_t time_count = time_series.size();

  if (2 * lags_ > time_count) {
    std::cout << "Failed to fit AR model on " << time_count << " elements on "
              << lags_ << " lags\n";
  } else {
    for (size_t iter = lags_; iter < time_count; ++iter) {
      features.emplace_back();
      target.push_back(time_series[iter]);
      if (fit_intercept) {
        features.back().push_back(1.);
      }
      for (size_t jter = iter - lags_; jter < iter; ++jter) {
        features.back().push_back(time_series[jter]);
      }
    }
    AR::coefs_ = RidgeRegression(features, target, lambda_);
  }
}

std::vector<double> AR::predict(const std::vector<double>& time_series,
                                size_t future_count) const {
  std::vector<double> predictions = time_series;
  std::vector<double> new_predictions;
  size_t time_series_length = time_series.size();
  if (time_series_length < lags_) {
    std::cout << "Fail to predict with AR: not enough data\n";
  } else {
    for (size_t day = 0; day < future_count; ++day) {
      double prediction = 0;
      if (fit_intercept_) {
        prediction += coefs_[0];
      }
      for (size_t iter = 0; iter < lags_; ++iter) {
        if (fit_intercept_) {
          prediction += coefs_[iter + 1] *
                        predictions[time_series_length + day - lags_ + iter];
        } else {
          prediction += coefs_[iter] *
                        predictions[time_series_length + day - lags_ + iter];
        }
      }
      predictions.push_back(prediction);
      new_predictions.push_back(prediction);
    }
  }
  return new_predictions;
}
