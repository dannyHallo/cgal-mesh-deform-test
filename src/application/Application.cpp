#include "Application.hpp"

#include "mesh-simplification/MeshSimplification.hpp"
#include "remesh/Remesh.hpp"

#include <iostream>
#include <sstream>

Application::Application()  = default;
Application::~Application() = default;

void Application::run() {
  for (;;) {
    // ReturnCode code = _remeshKernal();
    ReturnCode code = _simplifyKernal();
    if (code == ReturnCode::kExit) {
      break;
    }
  }
}

Application::ReturnCode Application::_remeshKernal() {
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

  Remesh::remesh(usingFileName, usingAngleLimDeg, usingTargetEdgeLength, usingNbIter);

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_simplifyKernal() {
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

  static double usingStopRatio = 0.1;
  std::cout << "Enter the stop ratio /[" << usingStopRatio << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingStopRatio; // Convert to double
  }

  MeshSimplification::meshSimplification(usingFileName, usingStopRatio);

  return ReturnCode::kContinue;
}