#ifndef IMGUI_UI_HPP
#define IMGUI_UI_HPP

#ifndef NOMINMAX
  #define NOMINMAX
#endif // NOMINMAX

#include <cstdio>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <windows.h>

#include "imgui.h"

#include "config.hpp"
#include "timers.hpp"
#include "win_utils.hpp"


// Types
using TabGroupWindows =
  std::vector<
    std::pair<
      std::string,
      std::vector<std::shared_ptr<WindowInfo>>
    >
  >;
using TabGroupLayout =
  std::vector<
    std::pair<
      std::string,
      WindowItemLayout
    >
  >;
using HotkeyWindows = 
  std::array<
    std::shared_ptr<WindowInfo>,
    10
  >;
using HotkeyLayout = WindowItemLayout;


/**
 * @brief Layout used to define the style of rendering a tab group should render with.
 */
enum WindowItemLayout {
  GRID,
  VERTICAL_LIST
};


/**
 * @brief STATIC-ONLY CLASS
 * 
 * Draws all UI for the program.
 */
class ImGuiUI {
  private:
    // Constants
    static constexpr double _FPS_DISPLAY_UPDATE_DELAY = 0.2;
    static constexpr float _HOTKEY_PANEL_LENGTH_PERCENT = 30.0f;
    static constexpr float _HOTKEY_PANEL_HEIGHT_PERCENT = 10.0f;
    static constexpr ImVec2 _BOTTOM_LEFT_CORNER_POS = ImVec2(0.0f, 1.0f);
    static constexpr ImVec2 _TOP_LEFT_CORNER_POS = ImVec2(0.0f, 0.0f);
    static constexpr ImVec2 _TOP_RIGHT_CORNER_POS = ImVec2(1.0f, 0.0f);
    static constexpr ImVec2 _BOTTOM_RIGHT_CORNER_POS = ImVec2(1.0f, 1.0f);

    // vars
    static bool _tab_groups_visible;
    static bool _hotkey_panel_visible;
    static bool _settings_panel_visible;

    static double _fps_display_accumulator;
    static bool _hotkey_layout_horizontal; // True = horizontal, False = Vertical


    // Render funcs

    static void _renderWindowItem(const WindowItemLayout layout);
    static void _renderTabGroupsUI(const TabGroupWindows& tab_groups, const TabGroupLayout& tab_group_layout);
    static void _renderHotkeyUI(const HotkeyWindows& hotkeys, const HotkeyLayout hotkey_layout);
    static void _renderSettingsUI(const double fps, const double delta);

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
     * @param tab_groups: Tab groups to render
     * @param tab_group_layout: Layout to render the tab groups with
     * @param hotkeys: Hotkey windows
     * @param hotkey_layout: Layout to render the hotkeys with
     */
    static void drawUI(const double fps, const double delta,
      const TabGroupWindows& tab_groups,
      const TabGroupLayout& tab_group_layout,
      const HotkeyWindows& hotkeys,
      const HotkeyLayout hotkey_layout
    );


    // Inline funcs

    static void setTabGroupsVisibility(const bool v) { _tab_groups_visible = v; }
    static const bool isTabGroupsVisible() { return _tab_groups_visible; }
    static void setHotkeyPanelVisibility(const bool v) { _hotkey_panel_visible = v; }
    static const bool isHotkeyPanelVisible() { return _hotkey_panel_visible; }
    static void setSettingsPanelVisibility(const bool v) { _settings_panel_visible = v; }
    static const bool isSettingsPanelVisible() { return _settings_panel_visible; }
};


#endif // IMGUI_UI_HPP
