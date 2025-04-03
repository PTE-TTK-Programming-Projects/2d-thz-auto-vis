#pragma once
#include <vector>

class Matrix {
private:
  std::vector<double> data;
  int size_n, size_m;

public:
  Matrix(int n, int m) {
    size_n = n;
    size_m = m;
    data = std::vector<double>(n * m, 0);
  }

  double &at(int n, int m) {
    if (n >= size_n || m >= size_m) {
      throw "Indicies exceeding maximum size!";
    }
    return data.at(size_m * n + m);
  }

  int n() { return size_n; }
  int m() { return size_m; }
};
