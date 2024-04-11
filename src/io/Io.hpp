#pragma once

#include <string>

namespace Io {
std::string makeFullInputPath(std::string const &filename);
std::string makeFullOutputPath(std::string const &filename);
} // namespace Io