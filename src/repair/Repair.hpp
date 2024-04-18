#pragma once

#include <string>

namespace Repair {

void removeDegenerateFaces(std::string const &filename, float thresholdAngle);

void detectCaps(std::string const &filename, float thresholdAngle);

} // namespace Repair