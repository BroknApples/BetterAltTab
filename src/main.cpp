#include <windows.h>
#include "core/application.hpp"


// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  Application::createApplication(hInstance);
  Application::runApplication();
  Application::destroyApplication();
  return 0;
}
