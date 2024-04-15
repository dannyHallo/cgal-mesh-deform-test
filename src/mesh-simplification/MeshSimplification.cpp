#include "MeshSimplification.hpp"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Face_count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <cassert>
#include <iostream>
#include <optional>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

namespace SMS = CGAL::Surface_mesh_simplification;

namespace {

std::optional<Mesh> _readMesh(const std::string &filePath) {
  std::cout << "Loading mesh from path (" << filePath << ")..." << std::endl;

  Mesh mesh;
  if (!CGAL::IO::read_polygon_mesh(filePath, mesh)) {
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

} // namespace

namespace MeshSimplification {

void edgeCollapse(std::string const &filename, size_t outputFaceCount) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeSurfaceMesh = _readMesh(inputFilePath);
  assert(maybeSurfaceMesh.has_value() && "Failed to read input mesh.");
  auto &surface_mesh = maybeSurfaceMesh.value();

  SMS::Face_count_stop_predicate<Mesh> stop(outputFaceCount);

  int r = SMS::edge_collapse(surface_mesh, stop);

  CGAL::IO::write_polygon_mesh(outputFilePath, surface_mesh,
                               CGAL::parameters::stream_precision(17));

  std::cout << "Remeshed mesh written to path (" << outputFilePath << ")" << std::endl;
  auto remeshedMeshOpt = _readMesh(outputFilePath);
}

} // namespace MeshSimplification
