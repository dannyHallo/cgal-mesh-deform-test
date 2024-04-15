#pragma once

#include <string>

namespace Remesh {

void isoRemesh(std::string const &filename, double targetEdgeLength, unsigned int nbIter);

} // namespace Remesh