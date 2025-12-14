#include "config.hpp"
 

// ---------------- Static declarations ----------------

bool Config::initialized = false;
ImVec2 Config::monitor_size = ImVec2(1920, 1080); // TODO: Read from save file the saved monitor size for default value
float Config::settings_panel_width_percent = 20.0f;  // 20% of the screen
float Config::settings_panel_height_percent = 100.0f; // Full screen height
int Config::vsync = 0;

// ---------------- init ----------------

void Config::init() {
  monitor_size = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  initialized = true;
}