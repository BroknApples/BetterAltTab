#ifndef IMGUI_UI_HPP
#define IMGUI_UI_HPP

#ifndef NOMINMAX
  #define NOMINMAX
#endif // NOMINMAX

#include <iomanip>
#include <cstdio>
#include <sstream>
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


/**
 * @brief Predefined tab groups that will ALWAYS exist
 */
struct StaticTabGroups {
  static inline const std::string OPEN_TABS = "Open Tabs"; // Special instance of a tab group
  static inline const std::string HOTKEYS = "Hotkeys"; // Special instance of a tab group
};


// Types
using TabGroup = std::vector<std::shared_ptr<WindowInfo>>;
using TabGroupMap =
  std::unordered_map<
    std::string,
    TabGroup
  >;
using TabGroupOrderList = std::vector<std::string>;
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

    // vars
    static bool _window_just_focused;
    static int _redraw_moving_frames_count;
    static bool _needs_io_redraw;

    static bool _tab_groups_visible;
    static bool _hotkey_panel_visible;
    static bool _settings_panel_visible;

    static double _fps_display_accumulator;
    static bool _request_saved_config_reset;
    static std::string _last_clicked_tab_group;


    // Render Helpers

    /**
     * @brief Outputs right-aligned for an ImGui call
     * @param fmt formatted text
     * @param ... formatted text vars
     */
    static void _ImGuiRightAlignedText(const char* fmt, ...);


    /**
     * @brief Fits a string to a given width
     * @param str: String to fit
     * @param max_width: Maximum width allowed for the string
     * @param ellipses: Should ellipses be appended to the string if it is truncated?
     * @returns std::string: New string made to fit the given size
     */
    static std::string _fitStringToWidth(const std::string& str, const float max_width, const bool ellipses = false);


    /**
     * @brief Renders a tab's cell in a tab group
     * @param tabs: List of tab groups. NOTE: Only used for context-menu actions.
     * @param group_title: Title of the tab group its rendering in
     * @param info: Window info to draw
     * @param layout: Layout to render with
     * @param cell_size: Size of the cell to render
     */
    static void _renderTabCell(TabGroupMap& tabs, const std::string& group_title, const std::shared_ptr<WindowInfo>& info, const TabGroupLayout layout, const ImVec2 cell_size);


    /**
     * @brief Renders a tab group with the proper layout
     * @param tab_groups: Map of tab groups.
     * @param title: Title to give the tab group
     * @param tabs: Tabs to render
     * @param layout: Layout to render with
     */
    static void _renderTabGroup(TabGroupMap& tab_groups, const std::string& title, const TabGroup tabs, const TabGroupLayout layout);


    /**
     * @brief Render each tab group on the screen
     * @param tab_groups: Map of tab groups to render
     * @param tab_groups_order: List of the ordering of tab groups
     * @param tab_groups_layouts: Map of the layout for each tab group
     */
    static void _renderTabGroupsUI(TabGroupMap& tab_groups, TabGroupOrderList& tab_groups_order, const TabGroupLayoutList& tab_group_layouts);


    /**
     * @brief Render the hotkey UI onto the screen
     * @param hotkeys: Hotkey windows to render
     * @param hotkey_layout: Layout to render with
     */
    static void _renderHotkeyUI(const TabGroup& hotkeys, const TabGroupLayout hotkey_layout);


    /**
     * @brief Render the settings panel UI onto the screen
     * @param fps: FPS of the app
     * @param delta: Time since last frame
     */
    static void _renderSettingsUI(const double fps, const double delta);


    /**
     * @brief Syncs a temporary variable to its saved var
     */
    template<typename T>
    static void _syncTemp(T& tmp, const T& source) {
      if (!ImGui::IsAnyItemActive()) tmp = source;
    }

  public:
    /**
     * @brief Enforce Static-Only class
     */
    ImGuiUI() = delete;


    /**
     * @brief Setup im gui styles
     */
    static void setupImGuiStyles();


    /**
     * @brief Draws all UI elements that should be drawn (must be visible)
     * @param tab_groups: Tab groups to render
     * @param tab_groups_order: List of the order to render tab groups
     * @param tab_groups_layouts: Layout to render the tab groups with
     */
    static void drawUI(const double fps, const double delta,
      TabGroupMap& tab_groups, TabGroupOrderList& tab_groups_order, const TabGroupLayoutList& tab_groups_layouts
    );


    // Inline funcs

    static void setWindowJustFocused(const bool v) { _window_just_focused = v; }
    static const bool wasWindowJustFocused() { return _window_just_focused; }

    static void setNeedsMovingRedraw(const bool v) { if (v) _redraw_moving_frames_count = 5; }
    static const bool needsMovingRedraw() { return _redraw_moving_frames_count > 0; }
    static void decrementMovingRedraw(const int f) { _redraw_moving_frames_count -= f; }

    static void setNeedsIoRedraw(const bool v) { _needs_io_redraw = v; }
    static const bool needsIoRedraw() { return _needs_io_redraw; }


    static void setTabGroupsVisibility(const bool v) { _tab_groups_visible = v; }
    static const bool isTabGroupsVisible() { return _tab_groups_visible; }
    static void setHotkeyPanelVisibility(const bool v) { _hotkey_panel_visible = v; }
    static const bool isHotkeyPanelVisible() { return _hotkey_panel_visible; }
    static void setSettingsPanelVisibility(const bool v) { _settings_panel_visible = v; }
    static const bool isSettingsPanelVisible() { return _settings_panel_visible; }
};


#endif // IMGUI_UI_HPP
