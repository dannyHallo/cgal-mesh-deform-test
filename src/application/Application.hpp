#pragma once

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
  ReturnCode _remeshKernal();
  ReturnCode _simplifyKernal();
};
