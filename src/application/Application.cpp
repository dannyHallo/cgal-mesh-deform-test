#include "Application.hpp"

#include "mesh-simplification/MeshSimplification.hpp"
#include "remesh/Remesh.hpp"

#include <iostream>
#include <sstream>

Application::Application()  = default;
Application::~Application() = default;

static std::string const kRemeshCmd   = "rem";
static std::string const kSimplifyCmd = "sim";

void Application::run() {
  for (;;) {
    static std::string usingCommand = kRemeshCmd;
    std::string inputLine; // Use to read the whole line

    std::cout << "Enter the command /[" << usingCommand << "]: ";
    std::getline(std::cin, inputLine); // Read the whole line
    if (!inputLine.empty()) {
      usingCommand = inputLine;
    }

    ReturnCode code = _commandKernal(usingCommand);
    if (code == ReturnCode::kExit) {
      break;
    }
  }
}

Application::ReturnCode Application::_commandKernal(std::string const &command) {
  if (command == kRemeshCmd) {
    return _remeshKernal();
  } else if (command == kSimplifyCmd) {
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

  static std::string usingPolicy = "cp";
  std::cout << "Enter the policy /[" << usingPolicy << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    usingPolicy = inputLine;
  }

  static size_t usingOutputFaceCount = 6050;
  std::cout << "Enter the output face count /[" << usingOutputFaceCount << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingOutputFaceCount; // Convert to size_t
  }

  // select policy here
  MeshSimplification::GarlandHeckbertPolicy policy =
      MeshSimplification::GarlandHeckbertPolicy::kClassicPlane;
  if (usingPolicy == "cp") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kClassicPlane;
  } else if (usingPolicy == "pp") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kProbabilisticPlane;
  } else if (usingPolicy == "ct") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kClassicTriangle;
  } else if (usingPolicy == "pt") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kProbabilisticTriangle;
  } else {
    std::cerr << "Invalid policy" << std::endl;
    return ReturnCode::kFailure;
  }

  MeshSimplification::edgeCollapse(usingFileName, usingOutputFaceCount, policy);

  return ReturnCode::kContinue;
}
