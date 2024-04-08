#pragma once

#include <string>

class Application {
public:
  Application();
  ~Application();

  void run();

private:
  void _remesh(std::string const &filename, double target_edge_length = 0.04,
               unsigned int nb_iter = 10);
};