#include "Benchmark.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <CGAL/Polygon_mesh_processing/shape_predicates.h>
#include <CGAL/Surface_mesh.h>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

typedef boost::graph_traits<Mesh>::face_descriptor face_descriptor;

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

namespace Benchmark {

void benchmark(std::string const &filename, float thresholdAngle) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeMesh = _readMesh(inputFilePath);
  if (maybeMesh == std::nullopt) {
    return;
  }
  Mesh &mesh = maybeMesh.value();

  // bool intersecting = PMP::does_self_intersect<CGAL::Parallel_if_available_tag>(
  //     mesh, CGAL::parameters::vertex_point_map(get(CGAL::vertex_point, mesh)));

  // count intersection
  std::vector<std::pair<face_descriptor, face_descriptor>> intersected_tris;
  PMP::self_intersections<CGAL::Parallel_if_available_tag>(faces(mesh), mesh,
                                                           std::back_inserter(intersected_tris));
  std::cout << intersected_tris.size() << " pairs of triangles intersect." << std::endl;

  // count cap triangles
  float const threshold = getCosVal(thresholdAngle);

  int capCount = 0;
  for (const auto &face : faces(mesh)) {
    auto halfEdge = PMP::is_cap_triangle_face(face, mesh, threshold);
    if (halfEdge == boost::graph_traits<Mesh>::null_halfedge()) {
      continue;
    }
    ++capCount;
  }

  std::cout << capCount << " cap triangles found." << std::endl;
}

void detectCaps(std::string const &filename, float thresholdAngle) {}

} // namespace Benchmark