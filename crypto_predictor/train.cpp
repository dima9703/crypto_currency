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

matrix Zeros(size_t n, size_t m) {
  matrix Y;
  for (size_t iter = 0; iter < n; ++iter) {
    Y.emplace_back();
    for (size_t jiter = 0; jiter < m; ++jiter) {
      Y.back().push_back(0.);
    }
  }
  return Y;
}

matrix SubtractMatrix(const matrix& A, const matrix& B) {
  matrix C;
  size_t n = A.size();
  size_t m = A.back().size();
  size_t r = B.back().size();
  if ((A.size() == B.size()) && (A.back().size() == B.back().size())) {
    for (size_t x = 0; x < A.size(); ++x) {
      C.emplace_back();
      for (size_t y = 0; y < A.back().size(); ++y) {
        C.back().push_back(A[x][y] - B[x][y]);
      }
    }
    return C;
  } else {
    std::cout << "\nError while adding " << A.size() << " x " << A.back().size()
              << " matrix to " << B.size() << " x " << B.back().size()
              << " matrix";
    return C;
  }
}

std::vector<double> GetCol(const matrix& X, size_t col_number) {
  std::vector<double> col;
  for (size_t iter = 0; iter < X.size(); ++iter) {
    col.push_back(X[iter][col_number]);
  }
  return col;
}

matrix InvertTriangular(const matrix& a) {
  size_t n = a.size();
  double t;
  matrix b = Zeros(n, n);
  for (size_t i = 0; i < n; ++i) {
    b[i][i] = 1. / a[i][i];
    for (int j = i - 1; j >= 0; j--) {
      t = 0;
      for (size_t k = 1 + j; k < n; k++) {
        t += a[j][k] * b[k][i];
      }
      b[j][i] = -t / a[j][j];
    }
  }
  return b;
}

void QRMatrix(matrix* a, matrix* q) {
  size_t i, j, k;
  double t, s, norma1, norma2;
  size_t n = (*a).size();
  std::vector<double> sinus(n);
  std::vector<double> cosinus(n);
  for (i = 0; i < n; i++) {
    norma1 = (*a)[i][i] * (*a)[i][i];
    norma2 = norma1;
    for (j = i + 1; j < n; ++j) {
      if (abs((*a)[j][i]) < 1e-15) {
        cosinus[j] = 1.;
        sinus[j] = 0.;
      } else {
        norma2 = norma1;
        norma1 += (*a)[j][i] * (*a)[j][i];
        cosinus[j] = sqrt(norma2) / sqrt(norma1);
        if ((j == i + 1) & ((*a)[i][i] < 0)) cosinus[j] = -cosinus[j];
        sinus[j] = -((*a)[j][i]) / sqrt(norma1);
        norma2 = norma1;
      }
    }
    for (j = 0; j < n; ++j) {
      for (k = i + 1; k < n; ++k) {
        if (j >= i) {
          t = (*a)[i][j];
          s = (*a)[k][j];
          (*a)[i][j] = cosinus[k] * t - s * sinus[k];
          (*a)[k][j] = t * sinus[k] + s * cosinus[k];
        }

        t = (*q)[i][j];
        s = (*q)[k][j];
        (*q)[i][j] = cosinus[k] * t - sinus[k] * s;
        (*q)[k][j] = cosinus[k] * s + t * sinus[k];
      }
    }
  }
}

matrix EyeMatrix(size_t n, double value = 1.) {
  matrix Y;
  for (size_t iter = 0; iter < n; ++iter) {
    Y.emplace_back();
    for (size_t jiter = 0; jiter < n; ++jiter) {
      if (iter == jiter) {
        Y.back().push_back(value);
      } else {
        Y.back().push_back(0.);
      }
    }
  }
  return Y;
}

std::vector<double> VectorByMatrix(const matrix& A,
                                   const std::vector<double>& x) {
  std::vector<double> y;
  if (A.back().size() == x.size()) {
    for (size_t iter = 0; iter < A.size(); ++iter) {
      double tmp = 0;
      for (size_t jter = 0; jter < x.size(); ++jter) {
        tmp += A[iter][jter] * x[jter];
      }
      y.push_back(tmp);
    }
    return y;
  } else {
    std::cout << "\n Error while calculating " << x.size() << " vector on "
              << A.size() << " x " << A.back().size() << " matrix";
    return y;
  }
}

matrix AddMatrix(const matrix& A, const matrix& B) {
  matrix C;
  size_t n = A.size();
  size_t m = A.back().size();
  size_t r = B.back().size();
  if ((A.size() == B.size()) && (A.back().size() == B.back().size())) {
    for (size_t x = 0; x < A.size(); ++x) {
      C.emplace_back();
      for (size_t y = 0; y < A.back().size(); ++y) {
        C.back().push_back(A[x][y] + B[x][y]);
      }
    }
    return C;
  } else {
    std::cout << "\nError while adding " << A.size() << " x " << A.back().size()
              << " matrix to " << B.size() << " x " << B.back().size()
              << " matrix";
    return C;
  }
}

double ScalarProduct(std::vector<double> x, std::vector<double> y) {
  double product = 0;
  if (x.size() == y.size()) {
    for (size_t iter = 0; iter < x.size(); ++iter) {
      product += x[iter] * y[iter];
    }
  } else {
    std::cout << "ERROR: Vectors of different shape\n";
  }
  return product;
}

matrix ConstructG(const AR& ARmodel, const std::vector<double>& time_series) {
  size_t t = time_series.size();
  matrix G = Zeros(t, t);
  size_t lags = ARmodel.coefs().size();
  std::vector<double> weights;
  weights.push_back(-1.);
  for (const auto& coef : ARmodel.coefs()) {
    weights.push_back(coef);
  }
  for (size_t iter = 0; iter < t; ++iter) {
    for (size_t jter = iter; jter < t; ++jter) {
      int d = jter - iter;
      for (size_t l = d; l < weights.size(); ++l) {
        if ((lags <= l + iter) && (l + iter < t)) {
          G[iter][jter] -= weights[iter] * weights[iter - d];
        }
      }
    }
  }
  return Zeros(t, t);
}

std::vector<double> ConstructD(const AR& ARmodel,
                               const std::vector<double>& time_series) {
  double common = -1.;
  for (const auto& coef : ARmodel.coefs()) {
    common += coef;
  }
  size_t lags = ARmodel.coefs().size();
  size_t time_count = time_series.size();
  std::vector<double> result;
  for (size_t iter = 0; iter < time_count; ++iter) {
    result.push_back(0.);
    for (size_t jter = 0; jter < ARmodel.coefs().size(); ++jter) {
      if ((iter + jter >= ARmodel.coefs().size()) &&
          (iter + jter <= time_count)) {
        result.back() += ARmodel.coefs()[jter];
      }
    }
    result.back() *= common;
  }
  return result;
}

matrix Laplacian(const matrix& G) {
  if (G.size() != G.back().size()) {
    std::cout << "Matrix G is not square\n";
    return Zeros(1, 1);
  } else {
    size_t time_count = G.size();
    matrix LaplacianG = Zeros(time_count, time_count);
    for (size_t iter = 0; iter < G.size(); ++iter) {
      for (size_t jter = 0; jter < G.size(); ++jter) {
        if (iter == jter) {
          LaplacianG[iter][jter] = -G[iter][jter];
        } else {
          for (size_t tmp; tmp < time_count; ++tmp) {
            LaplacianG[iter][jter] += G[iter][tmp];
          }
        }
      }
    }
    return LaplacianG;
  }
}

matrix Invert(const matrix& A) {
  matrix R = A;
  size_t n = A.size();
  matrix Q = EyeMatrix(n);
  QRMatrix(&R, &Q);
  return Multiplication(InvertTriangular(R), Q);
  ;
}
std::vector<double> RidgeRegression(const matrix& Z,
                                    const std::vector<double>& Y,
                                    double lambda) {
  size_t n = Z.back().size();
  return VectorByMatrix(
      Invert(AddMatrix(Multiplication(Transpose(Z), Z), EyeMatrix(n, lambda))),
      VectorByMatrix(Transpose(Z), Y));
}

void UpdateF(matrix* F, const matrix X, const matrix Y, const mask init_mask,
             double lambda) {
  size_t items_count = Y.size();
  size_t time_count = Y.back().size();
  // std::cout<< time_count << " " << items_count << "\n";
  size_t latent_dimension = X.size();
  for (size_t row_F = 0; row_F < items_count; ++row_F) {
    std::vector<double> target;
    matrix features;
    for (size_t row_X = 0; row_X < time_count; ++row_X) {
      if (init_mask[row_F][row_X]) {
        target.push_back(Y[row_F][row_X]);
        features.emplace_back();
        for (size_t jter = 0; jter < latent_dimension; ++jter) {
          features.back().push_back(X[jter][row_X]);
        }
      }
    }
  }
}

void UpdateX(const matrix& F, matrix* X, const matrix& Y,
             const std::vector<AR>& ARs, const mask& init_mask, double lambda) {
  size_t items_count = Y.size();
  size_t time_count = Y.back().size();
  size_t latent_dimension = (*X).size();
  for (size_t row_X = 0; row_X < time_count; ++row_X) {
    std::vector<double> target;
    matrix features;
    for (size_t row_F = 0; row_F < items_count; ++row_F) {
      if (init_mask[row_F][row_X]) {
        target.push_back(Y[row_F][row_X]);
        features.emplace_back();
        for (size_t jter = 0; jter < latent_dimension; ++jter) {
          features.back().push_back(F[row_F][jter]);
        }
      }
    }
    if (features.size() > 0) {
      std::vector<double> new_row = RidgeRegression(features, target, lambda);
      for (size_t iter = 0; iter < latent_dimension; ++iter) {
        (*X)[iter][row_X] = new_row[iter];
      }
    }
  }
}

void UpdateARMAs(const matrix& X, std::vector<AR>* ars, size_t lags,
                 double lambda = 0, bool fit_intercept = false) {
  for (size_t row = 0; row < X.size(); ++row) {
    std::vector<double> time_series = GetRow(X, row);
    (*ars)[row].fit(time_series, lags, lambda, fit_intercept);
  }
}

double CalculateNorma(const matrix& A, const mask& init_mask) {
  size_t n = A.size();
  size_t m = A.back().size();
  double norma = 0;
  for (size_t iter = 0; iter < n; ++iter) {
    for (size_t jter = 0; jter < m; ++jter) {
      if (init_mask[iter][jter]) {
        norma += A[iter][jter] * A[iter][jter];
      }
    }
  }
  return norma;
}

double CalculateFullNorma(const matrix& A) {
  size_t n = A.size();
  size_t m = A.back().size();
  double norma = 0;
  for (size_t iter = 0; iter < n; ++iter) {
    for (size_t jter = 0; jter < m; ++jter) {
      norma += A[iter][jter] * A[iter][jter];
    }
  }
  return norma;
}

double CalculateRegularizedNorma(const matrix& Y, const matrix& F,
                                 const matrix& X, const mask& init_mask,
                                 double lambda_F = 0, double lambda_x = 0) {
  return CalculateNorma(SubtractMatrix(Y, Multiplication(F, X)), init_mask) +
         lambda_x * CalculateFullNorma(X) + lambda_F * CalculateFullNorma(F);
}

double trmf_norm(const matrix& Y, const matrix& F, const matrix& X,
                 const mask& init_mask, const std::vector<AR>& ARs,
                 double lambda_F = 0, double lambda_x = 0,
                 double lambda_w = 0) {
  double trmf =
      CalculateNorma(SubtractMatrix(Y, Multiplication(F, X)), init_mask) +
      lambda_F * CalculateFullNorma(F);
  matrix R;
  for (const auto& ARmodel : ARs) {
    R.push_back(ARmodel.coefs());
  }
  trmf += lambda_w * CalculateFullNorma(R);
  for (size_t iter = 0; iter < X.size(); ++iter) {
    std::vector<double> time_series = GetRow(X, iter);
    std::vector<double> D = ConstructD(ARs[iter], time_series);
    matrix LapG = Laplacian(ConstructG(ARs[iter], time_series));
    trmf += 0.5 * ScalarProduct(time_series, VectorByMatrix(LapG, time_series));
    for (size_t iter = 0; iter < D.size(); ++iter) {
      trmf += 0.5 * D[iter] * time_series[iter] * time_series[iter];
    }
  }
  return trmf;
}

void CryptoData::train() {
  ARs.resize(X.size());
  for (size_t iteration = 0; iteration < max_iter_; ++iteration) {
    std::cout << "Started " << iteration << " iteration ...\n";
    UpdateF(&F, X, Y_, init_mask_, lambda_F_);
    UpdateX(F, &X, Y_, ARs, init_mask_, lambda_x_);
    UpdateARMAs(X, &ARs, lags_, lambda_w_ / lambda_x_, fit_intercept_);
   }

  WriteMatrixToCsv(F, "F.csv");
  std::cout << "Train finished.\n";
}
