#ifndef CONFIG_HPP
#define CONFIG_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <string_view>

#include "imgui.h"
#include "windows.h"

#include "../json/json_reader.hpp"


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
};


#endif // CONFIG_HPP