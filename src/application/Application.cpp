#include "Application.hpp"

#include "mesh-simplification/MeshSimplification.hpp"
#include "remesh/Remesh.hpp"

#include <iostream>
#include <sstream>

Application::Application()  = default;
Application::~Application() = default;

void Application::run() {
  static std::string const kRemesh   = "remesh";
  static std::string const kSimplify = "simplify";

  static std::string usingCommand = kRemesh;
  std::string inputLine; // Use to read the whole line

  // command
  std::cout << "Enter the command /[" << usingCommand << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    usingCommand = inputLine;
  }

  for (;;) {
    ReturnCode code = _commandKernal(usingCommand);
    if (code == ReturnCode::kExit) {
      break;
    }
  }
}

Application::ReturnCode Application::_commandKernal(std::string const &command) {
  if (command == "remesh") {
    return _remeshKernal();
  } else if (command == "simplify") {
    return _simplifyKernal();
  } else {
    return ReturnCode::kExit;
  }
}

Application::ReturnCode Application::_remeshKernal() {
  static std::string usingFileName = "o_pig_lo.obj";
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

  static unsigned int usingNbIter = 10;
  std::cout << "Enter the number of iterations /[" << usingNbIter << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingNbIter; // Convert to unsigned int
  }

  Remesh::isoRemesh(usingFileName, usingTargetEdgeLength, usingNbIter);

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_simplifyKernal() {
  static std::string usingFileName = "o_pig_lo_remeshed.obj";
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

  static size_t usingOutputFaceCount = 6050;
  std::cout << "Enter the output face count /[" << usingOutputFaceCount << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingOutputFaceCount; // Convert to size_t
  }

  MeshSimplification::edgeCollapse(usingFileName, usingOutputFaceCount);

  return ReturnCode::kContinue;
}
