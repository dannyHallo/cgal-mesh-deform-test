#pragma once

#include <string>

namespace MeshSimplification {

enum class GarlandHeckbertPolicy {
  kNone,
  kClassicPlane,
  kProbabilisticPlane,
  kClassicTriangle,
  kProbabilisticTriangle,
};

void edgeCollapse(std::string const &filename, size_t outputFaceCount,
                  GarlandHeckbertPolicy policy);


} // namespace MeshSimplification