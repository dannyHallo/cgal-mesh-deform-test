#pragma once

#include <string>

namespace MeshSimplification {
void meshSimplification(std::string const &filename, double stopRatio = 0.1);
}