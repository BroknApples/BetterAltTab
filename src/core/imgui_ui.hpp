#ifndef IMGUI_UI_HPP
#define IMGUI_UI_HPP


#include <cstdio>
#include <windows.h>

#include "imgui.h"

#include "config.hpp"
#include "timers.hpp"


/**
 * @brief STATIC-ONLY CLASS
 * 
 * Draws all UI for the program.
 */
class ImGuiUI {
  private:
    // Constants
    static constexpr double _FPS_DISPLAY_UPDATE_DELAY = 0.2;

    // vars
    static bool _tab_groups_visible;
    static bool _hotkey_panel_visible;
    static bool _settings_panel_visible;

    static double _fps_display_accumulator;

  public:
    /**
     * @brief Enforce Static-Only class
     */
    ImGuiUI() = delete;


    /**
     * @brief Setup im gui styles
     */
    static void _setupImGuiStyles();


    /**
     * @brief Draws all UI elements that should be drawn (must be visible)
     */
    static void drawUI(const double fps, const double delta);


    // Inline funcs

    static void setTabGroupsVisibility(const bool v) { _tab_groups_visible = v; }
    static const bool isTabGroupsVisible() { return _tab_groups_visible; }
    static void setHotkeyPanelVisibility(const bool v) { _hotkey_panel_visible = v; }
    static const bool isHotkeyPanelVisible() { return _hotkey_panel_visible; }
    static void setSettingsPanelVisibility(const bool v) { _settings_panel_visible = v; }
    static const bool isSettingsPanelVisible() { return _settings_panel_visible; }


    // Render funcs

    static void _renderTabGroupsUI();
    static void _renderHotkeyUI();
    static void _renderSettingsUI(const double fps, const double delta);
};


#endif // IMGUI_UI_HPP
