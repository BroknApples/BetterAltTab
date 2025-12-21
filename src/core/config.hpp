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
class Config {
  private:
    static JsonReader _json_reader;

    // JSON KEY CONSTANTS

    inline static const std::string _GRAPHICS_SETTINGS = "Graphics Settings";
    inline static const std::string _VSYNC = (_GRAPHICS_SETTINGS + "." + "V-Sync");

    inline static const std::string _SETTINGS_PANEL = "Settings Panel";
    inline static const std::string _SETTINGS_PANEL_WIDTH_PERCENT = (_SETTINGS_PANEL + "." + "Settings Panel Width Percent");
    inline static const std::string _SETTINGS_PANEL_HEIGHT_PERCENT = (_SETTINGS_PANEL + "." + "Settings Panel Height Percent");

    inline static const std::string _HOTKEY_PANEL = "Hotkey Panel";
    inline static const std::string _HOTKEY_PANEL_HORIZONTAL_LAYOUT = _HOTKEY_PANEL + "." + "Horizontal Layout";
    inline static const std::string _HOTKEY_KEYBINDS = (_HOTKEY_PANEL + "." + "Keybinds");
    inline static const std::string _KEYBIND_SLOT1  = (_HOTKEY_KEYBINDS + "." + "Slot 1");
    inline static const std::string _KEYBIND_SLOT2  = (_HOTKEY_KEYBINDS + "." + "Slot 2");
    inline static const std::string _KEYBIND_SLOT3  = (_HOTKEY_KEYBINDS + "." + "Slot 3");
    inline static const std::string _KEYBIND_SLOT4  = (_HOTKEY_KEYBINDS + "." + "Slot 4");
    inline static const std::string _KEYBIND_SLOT5  = (_HOTKEY_KEYBINDS + "." + "Slot 5");
    inline static const std::string _KEYBIND_SLOT6  = (_HOTKEY_KEYBINDS + "." + "Slot 6");
    inline static const std::string _KEYBIND_SLOT7  = (_HOTKEY_KEYBINDS + "." + "Slot 7");
    inline static const std::string _KEYBIND_SLOT8  = (_HOTKEY_KEYBINDS + "." + "Slot 8");
    inline static const std::string _KEYBIND_SLOT9  = (_HOTKEY_KEYBINDS + "." + "Slot 9");
    inline static const std::string _KEYBIND_SLOT10 = (_HOTKEY_KEYBINDS + "." + "Slot 10");

    
  public:
    /**
     * @brief Enforce static-only class
     */
    Config() = delete;


    /**
     * @brief Initialize the config
     */
    static void init();

    
    /**
     * @brief Save changes to the disk
     * @returns bool: True/False of success
     */
    static bool save();


    /**
     * @brief Resets the config to the saved version
     */
    static void resetToSaved();


    /**
     * @brief Resets the config to the default version
     * NOTE: Automatically saves
     */
    static void resetToDefault();


    // Constants/States
    inline static const std::string CONFIG_SAVE_PATH = "config.json"; // TODO: Remove '../' when completed
    static constexpr const char* VERSION = "v1.0.0";
    static bool initialized;
    static ImVec2 monitor_size;

    // ----------------- Settings -----------------

    // Graphics
    static bool vsync;

    // Settings Panel
    static float settings_panel_width_percent;  // 20% of the screen
    static float settings_panel_height_percent; // Full screen height

    // Keybinds
    static bool hotkey_panel_horizontal_layout;
    static std::array<Keybind, 10> keybinds;
};


#endif // CONFIG_HPP