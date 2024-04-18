#include "Repair.hpp"

#include <CGAL/Polygon_mesh_processing/repair_degeneracies.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/shape_predicates.h>
#include <CGAL/Surface_mesh.h>
#include <boost/iterator/function_output_iterator.hpp>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <iostream>
#include <optional>
#include <string>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

namespace PMP = CGAL::Polygon_mesh_processing;

namespace {
std::optional<Mesh> _readMesh(std::string const &filePath) {
  std::cout << "Loading mesh from path (" << filePath << ")..." << std::endl;

  Mesh mesh;
  if (!PMP::IO::read_polygon_mesh(filePath, mesh)) {
    std::cerr << "Cannot read polygon mesh" << std::endl;
    return std::nullopt;
  }
  if (!CGAL::is_triangle_mesh(mesh)) {
    std::cerr << "Input geometry is not triangulated." << std::endl;
    return std::nullopt;
  }

  std::cout << "Faces: " << num_faces(mesh) << std::endl;
  return std::optional<Mesh>(mesh);
}

float getCosVal(float const angle) { return std::cos(angle * CGAL_PI / 180.0); }

} // namespace

namespace Repair {

void removeDegenerateFaces(std::string const &filename, float thresholdAngle) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeMesh = _readMesh(inputFilePath);
  if (maybeMesh == std::nullopt) {
    return;
  }
  Mesh &mesh = maybeMesh.value();

  float const threshold = getCosVal(thresholdAngle);

  bool success =
      PMP::remove_almost_degenerate_faces(mesh, CGAL::parameters::cap_threshold(threshold));

  CGAL::IO::write_polygon_mesh(outputFilePath, mesh, CGAL::parameters::stream_precision(17));

  std::cout << "Mesh repair state: " << (success ? "success" : "failed") << std::endl;
  std::cout << "Mesh repaired and is written to path (" << outputFilePath << ")" << std::endl;
  auto remeshedMeshOpt = _readMesh(outputFilePath);
}

} // namespace Repair
