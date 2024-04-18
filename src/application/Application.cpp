#include "Application.hpp"

#include "benchmark/Benchmark.hpp"
#include "mesh-simplification/MeshSimplification.hpp"
#include "remesh/Remesh.hpp"
#include "repair/Repair.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

Application::Application()  = default;
Application::~Application() = default;

static std::string const kRemeshCmd    = "rem";
static std::string const kSimplifyCmd  = "sim";
static std::string const kRepairCmd    = "rep";
static std::string const kBenchmarkCmd = "ben";
static std::string const kCustomCmd    = "cus";

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
  } else if (command == kRepairCmd) {
    return _repairKernal();
  } else if (command == kBenchmarkCmd) {
    return _benchmarkKernal();
  } else if (command == kCustomCmd) {
    return _customKernal();
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

  // record time
  auto start = std::chrono::high_resolution_clock::now();
  Remesh::isoRemesh(usingFileName, usingTargetEdgeLength, usingNbIter);
  auto end                              = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken: " << elapsed.count() << "s" << std::endl;

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_simplifyKernal() {
  static std::string usingFileName = "1.obj";
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
    std::cout << "Using Classic Plane" << std::endl;
  } else if (usingPolicy == "pp") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kProbabilisticPlane;
    std::cout << "Using Probabilistic Plane" << std::endl;
  } else if (usingPolicy == "ct") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kClassicTriangle;
    std::cout << "Using Classic Triangle" << std::endl;
  } else if (usingPolicy == "pt") {
    policy = MeshSimplification::GarlandHeckbertPolicy::kProbabilisticTriangle;
    std::cout << "Using Probabilistic Triangle" << std::endl;
  } else {
    policy = MeshSimplification::GarlandHeckbertPolicy::kNone;
    std::cout << "Using Default" << std::endl;
  }

  // record time
  auto start = std::chrono::high_resolution_clock::now();
  MeshSimplification::edgeCollapse(usingFileName, usingOutputFaceCount, policy);
  auto end                              = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time taken: " << elapsed.count() << "s" << std::endl;

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_repairKernal() {
  static std::string usingFileName = "test.obj";
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

  static float usingThresholdAngle = 170;
  std::cout << "Enter the threshold angle /[" << usingThresholdAngle << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingThresholdAngle; // Convert to float
  }

  Repair::removeDegenerateFaces(usingFileName, usingThresholdAngle);
  // Repair::detectCaps(usingFileName, usingThresholdAngle);

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_benchmarkKernal() {
  static std::string usingFileName = "1.obj";
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

  static float usingThresholdAngle = 170;
  std::cout << "Enter the threshold angle /[" << usingThresholdAngle << "]: ";
  std::getline(std::cin, inputLine); // Read the whole line
  if (!inputLine.empty()) {
    std::stringstream(inputLine) >> usingThresholdAngle; // Convert to float
  }

  Benchmark::benchmark(usingFileName, usingThresholdAngle);

  return ReturnCode::kContinue;
}

Application::ReturnCode Application::_customKernal() {
  for (int i = 1; i <= 10; i++) {
    std::string usingFileName = std::to_string(i) + ".obj";
    std::cout << "Using " << usingFileName << std::endl;

    // record time
    auto start = std::chrono::high_resolution_clock::now();
    MeshSimplification::edgeCollapse(usingFileName, 10000,
                                     MeshSimplification::GarlandHeckbertPolicy::kNone);
    auto end                              = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << "s" << std::endl;
  }

  return ReturnCode::kContinue;
}
