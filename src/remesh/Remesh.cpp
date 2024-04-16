#include "Remesh.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Surface_mesh.h>
#include <boost/iterator/function_output_iterator.hpp>

#include "common/defines.hpp"
#include "io/Io.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

typedef boost::graph_traits<Mesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor edge_descriptor;
typedef Kernel::Vector_3 Vector_3;
typedef Mesh::Face_index Face_index;
typedef Mesh::Edge_index Edge_index;
typedef Mesh::Halfedge_index Halfedge_index;

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
} // namespace

namespace Remesh {

struct halfedge2edge {
  halfedge2edge(const Mesh &m, std::vector<edge_descriptor> &edges) : mMesh(m), mEdges(edges) {}
  void operator()(const halfedge_descriptor &h) const { mEdges.push_back(edge(h, mMesh)); }
  const Mesh &mMesh;
  std::vector<edge_descriptor> &mEdges;
};

void isoRemesh(std::string const &filename, double targetEdgeLength, unsigned int nbIter) {
  std::string const inputFilePath  = Io::makeFullInputPath(filename);
  std::string const outputFilePath = Io::makeFullOutputPath(filename);

  auto maybeMesh = _readMesh(inputFilePath);
  if (maybeMesh == std::nullopt) {
    return;
  }
  Mesh &mesh = maybeMesh.value();

  std::vector<edge_descriptor> border;
  PMP::border_halfedges(faces(mesh), mesh,
                        boost::make_function_output_iterator(halfedge2edge(mesh, border)));

  boost::unordered_map<edge_descriptor, bool> edge_constraints_map;
  for (edge_descriptor ed : edges(mesh)) {
    edge_constraints_map[ed] = false;
  }
  // add border as well, as the default code suggests
  for (edge_descriptor ed : border) {
    edge_constraints_map[ed] = true;
  }

  auto edge_constraints_property_map = boost::make_assoc_property_map(edge_constraints_map);

  PMP::isotropic_remeshing(
      faces(mesh), targetEdgeLength, mesh,
      CGAL::parameters::number_of_iterations(nbIter)
          .collapse_constraints(true)
          .edge_is_constrained_map(edge_constraints_property_map)); // i.e. protect border, here

  CGAL::IO::write_polygon_mesh(outputFilePath, mesh, CGAL::parameters::stream_precision(17));

  std::cout << "Remeshed mesh written to path (" << outputFilePath << ")" << std::endl;
  // auto remeshedMeshOpt = _readMesh(outputFilePath);
}

} // namespace Remesh
