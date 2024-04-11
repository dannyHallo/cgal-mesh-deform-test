#include "Io.hpp"

namespace Io {
std::string const kResourceFolderPrefix = "C:/Users/danny/Desktop/cgal-mesh-deform-test/resources/";
std::string const kInputPrefix          = kResourceFolderPrefix + "input-models/out/";
std::string const kOutputPrefix         = kResourceFolderPrefix + "output-models/";

std::string makeFullInputPath(std::string const &filename) { return kInputPrefix + filename; }
std::string makeFullOutputPath(std::string const &filename) { return kOutputPrefix + filename; }
} // namespace Io
