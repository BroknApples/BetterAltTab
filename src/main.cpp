#include <windows.h>
#include "core/application.hpp"
#include "core/config.hpp"


// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  Config::init();
  
  if (Application::createApplication(hInstance)) {
    Application::runApplication();
    Application::destroyApplication();
  }
  
  return 0;
}
