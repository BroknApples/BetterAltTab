#include "config.hpp"
 

// ---------------- Static declarations ----------------
bool Config::initialized = false;
ImVec2 Config::monitor_size = ImVec2(1920, 1080); // TODO: Read from save file the saved monitor size for default value


// ---------------- init ----------------
void Config::init() {
  monitor_size = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  initialized = true;
}