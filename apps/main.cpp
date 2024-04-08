#include "application/Application.hpp"

#include <iostream>

int main() {
  Application app{};

  std::cout << "running application...\n";
  app.run();
  std::cout << "application finished.\n";

  return 0;
}