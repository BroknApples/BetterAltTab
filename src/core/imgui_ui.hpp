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


/**
 * @brief Layout used to define the style of rendering a tab group should render with.
 */
enum TabGroupLayout {
  GRID,
  VERTICAL_LIST
};


// Types
using TabGroup = std::vector<std::shared_ptr<WindowInfo>>;
using TabGroupMap =
  std::unordered_map<
    std::string,
    TabGroup
  >;
using TabGroupLayoutList =
  std::unordered_map<
    std::string,
    TabGroupLayout
  >;


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
    static inline const std::string _HOTKEY_TAB_GROUP = "Hotkey TabGroup"; // Special instance of a tab group

    // vars
    static bool _tab_groups_visible;
    static bool _hotkey_panel_visible;
    static bool _settings_panel_visible;

    static double _fps_display_accumulator;
    static bool _hotkey_layout_horizontal; // True = horizontal, False = Vertical


    // Render Helpers

    /**
     * @brief Outputs right-aligned for an ImGui call
     * @param fmt formatted text
     * @param ... formatted text vars
     */
    void _ImGuiRightAlignedText(const char* fmt, ...);

    /**
     * @brief Renders a tab group with the proper layout
     * @param title: Title to give the tab group
     * @param tabs: Tabs to render
     * @param layout: Layout to render with
     */
    static void _renderTabGroup(const std::string& title, const TabGroup tabs, const TabGroupLayout layout);

    /**
     * @brief Renders a tab from a tab group
     * @param layout: Layout to render with
     * @param info: Window info to draw
     */
    static void _renderTabItem(const std::shared_ptr<WindowInfo> info, const TabGroupLayout layout);

    // Render funcs
    static void _renderTabGroupsUI(const TabGroupMap& tab_groups, const TabGroupLayoutList& tab_group_layouts);
    static void _renderHotkeyUI(const TabGroup& hotkeys, const TabGroupLayout hotkey_layout);
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
     * @param tab_group_layouts: Layout to render the tab groups with
     */
    static void drawUI(const double fps, const double delta,
      const TabGroupMap& tab_groups,
      const TabGroupLayoutList& tab_group_layouts
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
