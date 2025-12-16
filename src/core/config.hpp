#ifndef CONFIG_HPP
#define CONFIG_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <string_view>
#include <array>
#include <vector>

#include "imgui.h"
#include "windows.h"

#include "../json/json_reader.hpp"


/**
 * @brief Struct defines a keybind
 */
struct Keybind {
  Keybind(const ImGuiKey key, const bool ctrl = false, const bool shift = false, const bool alt = false)
  : key(key)
  , ctrl(ctrl)
  , shift(shift)
  , alt(alt) {}

  /**
   * @brief Gets the keybind's formatted name
   * @returns std::string: Formatted string
   */
  std::string getFormattedKeybind() {
    std::string s;
    if (ctrl)  s += "Ctrl+";
    if (shift) s += "Shift+";
    if (alt)   s += "Alt+";
    s += ImGui::GetKeyName(key);
    return s;
  } 

  /**
   * @brief Checks if the current keybind is pressed
   * @returns bool: True/False of being pressed
   */
  bool isKeybindPressed() {
    ImGuiIO& io = ImGui::GetIO();
    if (ctrl  && !io.KeyCtrl)      return false;
    if (shift && !io.KeyShift)     return false;
    if (alt   && !io.KeyAlt)       return false;
    if (!ImGui::IsKeyPressed(key)) return false;
    return true;
  }

  ImGuiKey key;
  bool ctrl;
  bool shift;
  bool alt;
};




/**
 * @brief Holds configuration data for the application
 * 
 * NOTE: STATIC-ONLY
 */
struct Config {
  /**
   * @brief Enforce static-only class
   */
  Config() = delete;

  /**
   * @brief Initialize the config
   */
  static void init();

  static bool initialized;
  static constexpr const char* VERSION = "v1.0.0";
  static ImVec2 monitor_size;

  // Settings Panel
  static float settings_panel_width_percent;  // 20% of the screen
  static float settings_panel_height_percent; // Full screen height
  static int vsync;


  // Keybinds
  static std::array<Keybind, 10> keybinds;
};


#endif // CONFIG_HPP