#pragma once

#include <string>

namespace Remesh {
void remesh(std::string const &filename, double angleLimDeg, double targetEdgeLength,
            unsigned int nbIter);
}