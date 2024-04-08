#include "Application.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Surface_mesh.h>
#include <boost/iterator/function_output_iterator.hpp>

#include <iostream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef boost::graph_traits<Mesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor edge_descriptor;
namespace PMP = CGAL::Polygon_mesh_processing;

namespace {
std::string const kResourceFolderPrefix = "C:/Users/danny/Desktop/cgal-mesh-deform-test/resources/";
std::string const kInputPrefix          = kResourceFolderPrefix + "models/input-models/";
std::string const kOutputPrefix         = kResourceFolderPrefix + "models/output-models";

std::string _makeFullInputPath(std::string const &filename) { return kInputPrefix + filename; }
std::string _makeFullOutputPath(std::string const &filename) { return kOutputPrefix + filename; }
} // namespace

Application::Application()  = default;
Application::~Application() = default;

void Application::run() { _remesh("o_cube.obj"); }

struct halfedge2edge {
  halfedge2edge(const Mesh &m, std::vector<edge_descriptor> &edges) : m_mesh(m), m_edges(edges) {}
  void operator()(const halfedge_descriptor &h) const { m_edges.push_back(edge(h, m_mesh)); }
  const Mesh &m_mesh;
  std::vector<edge_descriptor> &m_edges;
};

void Application::_remesh(std::string const &filename, double target_edge_length,
                          unsigned int nb_iter) {
  std::string const filePath = _makeFullInputPath(filename);
  std::cout << "Reading " << filename << " (" << filePath << ") " << "..." << std::endl;

  Mesh mesh;
  if (!PMP::IO::read_polygon_mesh(filePath, mesh)) {
    std::cerr << "Cannot read polygon mesh" << std::endl;
    return;
  }
  if (!CGAL::is_triangle_mesh(mesh)) {
    std::cerr << "Input geometry is not triangulated." << std::endl;
    return;
  }

  std::cout << "Split border...";
  std::vector<edge_descriptor> border;
  PMP::border_halfedges(faces(mesh), mesh,
                        boost::make_function_output_iterator(halfedge2edge(mesh, border)));
  PMP::split_long_edges(border, target_edge_length, mesh);
  std::cout << "done." << std::endl;
  std::cout << "Start remeshing of " << filename << " (" << num_faces(mesh) << " faces)..."
            << std::endl;

  PMP::isotropic_remeshing(faces(mesh), target_edge_length, mesh,
                           CGAL::parameters::number_of_iterations(nb_iter).protect_constraints(
                               true)); // i.e. protect border, here
  CGAL::IO::write_polygon_mesh(_makeFullOutputPath(filename), mesh,
                               CGAL::parameters::stream_precision(17));
  std::cout << "Remeshing done." << std::endl;
}