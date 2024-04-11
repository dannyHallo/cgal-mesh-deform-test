#include "MeshSimplification.hpp"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <cassert>
#include <iostream>
#include <optional>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> Surface_mesh;

namespace SMS = CGAL::Surface_mesh_simplification;

namespace {
std::optional<Surface_mesh> _readMesh(const std::string &filePath) {
  std::cout << "Loading mesh from path (" << filePath << ")..." << std::endl;

  Surface_mesh mesh;
  if (!CGAL::IO::read_polygon_mesh(filePath, mesh)) {
    std::cerr << "Cannot read polygon mesh" << std::endl;
    return std::nullopt;
  }
  if (!CGAL::is_triangle_mesh(mesh)) {
    std::cerr << "Input geometry is not triangulated." << std::endl;
    return std::nullopt;
  }

  std::cout << "Faces: " << num_faces(mesh) << std::endl;
  return std::optional<Surface_mesh>(mesh);
}

// std::optional<Surface_mesh> _readMesh(std::string const &filePath) {
//   Surface_mesh surface_mesh;

//   std::ifstream is(filePath);
//   if (!is || !(is >> surface_mesh)) {
//     std::cerr << "Failed to read input mesh: " << filePath << std::endl;
//     return std::nullopt;
//   }
//   if (!CGAL::is_triangle_mesh(surface_mesh)) {
//     std::cerr << "Input geometry is not triangulated." << std::endl;
//     return std::nullopt;
//   }
//   return std::optional<Surface_mesh>(surface_mesh);
// }
} // namespace

namespace MeshSimplification {

void meshSimplification(std::string const &filename, double stopRatio) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeSurfaceMesh = _readMesh(inputFilePath);
  assert(maybeSurfaceMesh.has_value() && "Failed to read input mesh.");
  auto &surface_mesh = maybeSurfaceMesh.value();

  SMS::Edge_count_ratio_stop_predicate<Surface_mesh> stop(stopRatio);
  int r = SMS::edge_collapse(surface_mesh, stop);

  std::cout << "Finished!\n"
            << r << " edges removed.\n"
            << surface_mesh.number_of_edges() << " final edges.\n";

  CGAL::IO::write_polygon_mesh(outputFilePath, surface_mesh,
                               CGAL::parameters::stream_precision(17));

  std::cout << "Remeshed mesh written to path (" << outputFilePath << ")" << std::endl;
  auto remeshedMeshOpt = _readMesh(outputFilePath);
}
} // namespace MeshSimplification