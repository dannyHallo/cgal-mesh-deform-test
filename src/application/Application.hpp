#pragma once

#include <string>

class Application {
public:
  Application();
  ~Application();

  void run();

private:
  enum class ReturnCode {
    kContinue,
    kExit,
    kFailure,
  };

  // return 1 if the user wants to exit
  ReturnCode _commandKernal(std::string const &command);
  ReturnCode _remeshKernal();
  ReturnCode _simplifyKernal();
  ReturnCode _repairKernal();
  ReturnCode _benchmarkKernal();
  ReturnCode _customKernal();
};
