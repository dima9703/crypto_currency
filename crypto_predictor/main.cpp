#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "model.h"
#include "json.hpp"
using json = nlohmann::json;

int main() {
  json j = ReadValidateInputConfig();
  CryptoData a;
  a.Initialize(j);
  a.train();
  a.predict();
  return 0;
}
