#include "MeshSimplification.hpp"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Bounded_normal_change_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Face_count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/GarlandHeckbert_policies.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <cassert>
#include <iostream>
#include <optional>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

namespace SMS = CGAL::Surface_mesh_simplification;

typedef SMS::GarlandHeckbert_plane_policies<Mesh, Kernel> Classic_plane;
typedef SMS::GarlandHeckbert_probabilistic_plane_policies<Mesh, Kernel> Prob_plane;
typedef SMS::GarlandHeckbert_triangle_policies<Mesh, Kernel> Classic_tri;
typedef SMS::GarlandHeckbert_probabilistic_triangle_policies<Mesh, Kernel> Prob_tri;

template <typename GHPolicies> void collapse_gh(Mesh &mesh, size_t outputFaceCount) {}

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

void _processMesh(Mesh &mesh, size_t outputFaceCount,
                  MeshSimplification::GarlandHeckbertPolicy policy) {
  SMS::Face_count_stop_predicate<Mesh> stop(outputFaceCount);

  typedef typename Classic_plane::Get_cost GH_cost;
  typedef typename Classic_plane::Get_placement GH_placement;
  typedef SMS::Bounded_normal_change_placement<GH_placement> Bounded_GH_placement;
  Classic_plane gh_policies(mesh);
  const GH_cost &gh_cost           = gh_policies.get_cost();
  const GH_placement &gh_placement = gh_policies.get_placement();
  Bounded_GH_placement placement(gh_placement);
  SMS::edge_collapse(mesh, stop, CGAL::parameters::get_cost(gh_cost).get_placement(placement));
}

void edgeCollapseDefault(std::string const &filename, size_t outputFaceCount) {
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
  // auto remeshedMeshOpt = _readMesh(outputFilePath);
}

void edgeCollapseGarlandHeckbert(std::string const &filename, size_t outputFaceCount,
                                 MeshSimplification::GarlandHeckbertPolicy policy) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeMesh = _readMesh(inputFilePath);
  if (maybeMesh == std::nullopt) {
    return;
  }
  auto &surface_mesh = maybeMesh.value();

  // SMS::edge_collapse(surface_mesh, stop);

  _processMesh(surface_mesh, outputFaceCount, policy);

  CGAL::IO::write_polygon_mesh(outputFilePath, surface_mesh,
                               CGAL::parameters::stream_precision(17));

  std::cout << "Remeshed mesh written to path (" << outputFilePath << ")" << std::endl;
  // auto remeshedMeshOpt = _readMesh(outputFilePath);
}

} // namespace

namespace MeshSimplification {

void edgeCollapse(std::string const &filename, size_t outputFaceCount,
                  GarlandHeckbertPolicy policy) {
  if (policy == GarlandHeckbertPolicy::kNone) {
    edgeCollapseDefault(filename, outputFaceCount);
  } else {
    edgeCollapseGarlandHeckbert(filename, outputFaceCount, policy);
  }
}

} // namespace MeshSimplification
