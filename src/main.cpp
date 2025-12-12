#include <iostream>
#include <windows.h>
#include "core/application.hpp"
#include "core/config.hpp"
#include "core/win_utils.hpp"


// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  // Ensure only one instance is running before proceeding.
  if (isInstanceUnique()) {
    Config::init();
  
    if (Application::createApplication(hInstance)) {
      Application::runApplication();
      Application::destroyApplication();
    }
  }
  
  return EXIT_SUCCESS;
}
