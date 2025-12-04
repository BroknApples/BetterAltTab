#include <iostream>
#include <windows.h>
#include "core/application.hpp"
#include "core/config.hpp"
#include "core/windows_helpers.hpp"


// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  // Ensure only one instance is running before proceeding.
  if (isInstanceUnique()) {
    // auto windows = getAllAltTabWindows();

    // for (WindowInfo info : windows) {
    //   std::cout << "Window: \"" << info.title << "\" (HWND: 0x" << info.hwnd << ")\n";
    // }

    // printf("\nTotal: %zu windows\n", windows.size());

    Config::init();
  
    if (Application::createApplication(hInstance)) {
      Application::runApplication();
      Application::destroyApplication();
    }
  }
  
  return EXIT_SUCCESS;
}
