#ifndef CONFIG_HPP
#define CONFIG_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


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
  static ImVec2 monitor_size;
};


#endif // CONFIG_HPP