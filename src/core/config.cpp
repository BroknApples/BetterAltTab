#include "config.hpp"
 

// ---------------- Static declarations ----------------

bool Config::initialized = false;
ImVec2 Config::monitor_size = ImVec2(1920, 1080); // TODO: Read from save file the saved monitor size for default value
float Config::settings_panel_width_percent = 20.0f;  // 20% of the screen
float Config::settings_panel_height_percent = 100.0f; // Full screen height
int Config::vsync = 1;

// Keybinds
std::array<Keybind, 10> Config::keybinds = {
  Keybind{ImGuiKey_1},
  Keybind{ImGuiKey_2},
  Keybind{ImGuiKey_3},
  Keybind{ImGuiKey_4},
  Keybind{ImGuiKey_5},
  Keybind{ImGuiKey_6},
  Keybind{ImGuiKey_7},
  Keybind{ImGuiKey_8},
  Keybind{ImGuiKey_9},
  Keybind{ImGuiKey_0}
};

// ---------------- init ----------------

void Config::init() {
  monitor_size = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  initialized = true;
}