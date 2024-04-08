#include "Application.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Surface_mesh.h>
#include <boost/iterator/function_output_iterator.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef boost::graph_traits<Mesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor edge_descriptor;
namespace PMP = CGAL::Polygon_mesh_processing;

namespace {
std::string const kResourceFolderPrefix = "C:/Users/danny/Desktop/cgal-mesh-deform-test/resources/";
std::string const kInputPrefix          = kResourceFolderPrefix + "input-models/out/";
std::string const kOutputPrefix         = kResourceFolderPrefix + "output-models/";

std::string _makeFullInputPath(std::string const &filename) { return kInputPrefix + filename; }
std::string _makeFullOutputPath(std::string const &filename) { return kOutputPrefix + filename; }
} // namespace

Application::Application()  = default;
Application::~Application() = default;

void Application::run() {
  for (;;) {
    static std::string usingFileName{};
    std::string inputLine; // Use to read the whole line

    // Filename
    std::cout << "Enter the filename /[" << usingFileName << "]: ";
    std::getline(std::cin, inputLine); // Read the whole line
    if (!inputLine.empty()) {
      usingFileName = inputLine;
    }

    // Exit if filename is "exit"
    if (usingFileName == "exit") {
      break;
    }

    static double usingTargetEdgeLength = 0.04;
    std::cout << "Enter the target edge length /[" << usingTargetEdgeLength << "]: ";
    std::getline(std::cin, inputLine); // Read the whole line
    if (!inputLine.empty()) {
      std::stringstream(inputLine) >> usingTargetEdgeLength; // Convert to double
    }

    static unsigned int usingNbIter = 10;
    std::cout << "Enter the number of iterations /[" << usingNbIter << "]: ";
    std::getline(std::cin, inputLine); // Read the whole line
    if (!inputLine.empty()) {
      std::stringstream(inputLine) >> usingNbIter; // Convert to unsigned int
    }

    _remesh(usingFileName, usingTargetEdgeLength, usingNbIter);
  }
}

struct halfedge2edge {
  halfedge2edge(const Mesh &m, std::vector<edge_descriptor> &edges) : mMesh(m), mEdges(edges) {}
  void operator()(const halfedge_descriptor &h) const { mEdges.push_back(edge(h, mMesh)); }
  const Mesh &mMesh;
  std::vector<edge_descriptor> &mEdges;
};

void Application::_remesh(std::string const &filename, double targetEdgeLength,
                          unsigned int nbIter) {
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
  PMP::split_long_edges(border, targetEdgeLength, mesh);
  std::cout << "done." << std::endl;
  std::cout << "Start remeshing of " << filename << " (" << num_faces(mesh) << " faces)..."
            << std::endl;

  PMP::isotropic_remeshing(faces(mesh), targetEdgeLength, mesh,
                           CGAL::parameters::number_of_iterations(nbIter).protect_constraints(
                               true)); // i.e. protect border, here
  CGAL::IO::write_polygon_mesh(_makeFullOutputPath(filename), mesh,
                               CGAL::parameters::stream_precision(17));
  std::cout << "Remeshing done." << std::endl;

  std::cout << "Output file (" << _makeFullOutputPath(filename) << ") has " << num_faces(mesh)
            << " faces" << std::endl;
}
