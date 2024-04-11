#include "Application.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/angle_and_area_smoothing.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/detect_features.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Surface_mesh.h>
#include <boost/iterator/function_output_iterator.hpp>
#include <cmath> // for std::acos

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef boost::graph_traits<Mesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor edge_descriptor;
typedef K::Vector_3 Vector_3;
typedef Mesh::Face_index Face_index;
typedef Mesh::Edge_index Edge_index;
typedef Mesh::Halfedge_index Halfedge_index;

namespace PMP = CGAL::Polygon_mesh_processing;

double constexpr kPi       = 3.14159265358979323846;
double constexpr kDegToRad = kPi / 180.0;

struct halfedge2edge {
  halfedge2edge(const Mesh &m, std::vector<edge_descriptor> &edges) : mMesh(m), mEdges(edges) {}
  void operator()(const halfedge_descriptor &h) const { mEdges.push_back(edge(h, mMesh)); }
  const Mesh &mMesh;
  std::vector<edge_descriptor> &mEdges;
};

namespace {
std::string const kResourceFolderPrefix = "C:/Users/danny/Desktop/cgal-mesh-deform-test/resources/";
std::string const kInputPrefix          = kResourceFolderPrefix + "input-models/out/";
std::string const kOutputPrefix         = kResourceFolderPrefix + "output-models/";

std::string _makeFullInputPath(std::string const &filename) { return kInputPrefix + filename; }
std::string _makeFullOutputPath(std::string const &filename) { return kOutputPrefix + filename; }

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

// Helper function to calculate the normal of a face
Vector_3 calculate_face_normal(const Mesh &mesh, Face_index f) {
  return CGAL::Polygon_mesh_processing::compute_face_normal(f, mesh);
}

// Function to calculate the dihedral angle between two vectors
double calculate_dihedral_angle(const Vector_3 &normal1, const Vector_3 &normal2) {
  double dot   = normal1 * normal2;
  double angle = std::acos(
      dot / (CGAL::sqrt(normal1.squared_length()) * CGAL::sqrt(normal2.squared_length())));
  return angle;
}

std::vector<Edge_index> find_edges_by_angle_threshold(Mesh &mesh, double angle_threshold_rad) {
  std::vector<Edge_index> edges_over_threshold;

  for (Edge_index e : mesh.edges()) {
    Halfedge_index h          = mesh.halfedge(e, 0);
    Halfedge_index h_opposite = mesh.halfedge(e, 1);

    if (!mesh.is_border(h) && !mesh.is_border(h_opposite)) {
      Face_index f1 = mesh.face(h);
      Face_index f2 = mesh.face(h_opposite);

      Vector_3 normal1 = calculate_face_normal(mesh, f1);
      Vector_3 normal2 = calculate_face_normal(mesh, f2);

      double angle = calculate_dihedral_angle(normal1, normal2);

      if (angle > angle_threshold_rad) {
        // This edge 'e' has adjacent faces forming an angle greater than the threshold
        edges_over_threshold.push_back(e);
      }
    }
  }

  return edges_over_threshold;
}

void _remesh(std::string const &filename, double angleLimDeg, double targetEdgeLength,
             unsigned int nbIter) {
  std::string const inputFilePath  = _makeFullInputPath(filename);
  std::string const outputFilePath = _makeFullOutputPath(filename);

  auto meshOpt = _readMesh(inputFilePath);
  assert(meshOpt.has_value());
  Mesh &mesh = meshOpt.value();

  std::vector<edge_descriptor> border;
  PMP::border_halfedges(faces(mesh), mesh,
                        boost::make_function_output_iterator(halfedge2edge(mesh, border)));
  // PMP::split_long_edges(border, targetEdgeLength, mesh);

  auto edgesFound = find_edges_by_angle_threshold(mesh, angleLimDeg * kDegToRad);
  std::cout << "edgesFound size: " << edgesFound.size() << std::endl;

  boost::unordered_map<edge_descriptor, bool> edge_constraints_map;
  for (edge_descriptor ed : edges(mesh)) {
    edge_constraints_map[ed] = false;
  }
  for (edge_descriptor ed : edgesFound) {
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
  auto remeshedMeshOpt = _readMesh(outputFilePath);
}

} // namespace

Application::Application()  = default;
Application::~Application() = default;

void Application::run() {
  for (;;) {
    ReturnCode code = _iteration();
    if (code == ReturnCode::kExit) {
      break;
    }
  }
}

Application::ReturnCode Application::_iteration() {
  static std::string usingFileName = "o_jeep.obj";
  std::string inputLine; // Use to read the whole line

  // filename
  std::cout << "Enter the filename /[" << usingFileName << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    usingFileName = inputLine;
  }

  if (usingFileName == "exit") {
    return ReturnCode::kExit;
  }

  static double usingTargetEdgeLength = 0.04;
  std::cout << "Enter the target edge length /[" << usingTargetEdgeLength << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingTargetEdgeLength; // Convert to double
  }

  static double usingAngleLimDeg = 40.0;
  std::cout << "Enter the angle limit in degrees /[" << usingAngleLimDeg << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingAngleLimDeg; // Convert to double
  }

  static unsigned int usingNbIter = 10;
  std::cout << "Enter the number of iterations /[" << usingNbIter << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingNbIter; // Convert to unsigned int
  }

  _remesh(usingFileName, usingAngleLimDeg, usingTargetEdgeLength, usingNbIter);

  return ReturnCode::kContinue;
}
