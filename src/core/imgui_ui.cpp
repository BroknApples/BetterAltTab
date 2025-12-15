#include "imgui_ui.hpp"


// ----------------- Static Vars -----------------

bool ImGuiUI::_tab_groups_visible = false;
bool ImGuiUI::_hotkey_panel_visible = false;
bool ImGuiUI::_settings_panel_visible = false;

double ImGuiUI::_fps_display_accumulator = 0.0;
bool ImGuiUI::_hotkey_layout_horizontal = false;


// ----------------- Private Functions -----------------

void ImGuiUI::_setupImGuiStyles() {
  ImGuiStyle& style = ImGui::GetStyle();

  // Text
  style.Colors[ImGuiCol_Text]            = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]    = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
  style.Colors[ImGuiCol_TextLink]        = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]  = ImVec4(0.26f, 0.52f, 0.96f, 0.35f);


  // Windows / Backgrounds
  style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);
  style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg]          = ImVec4(0.12f, 0.12f, 0.12f, 0.95f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.55f);


  // Borders & Separators
  style.Colors[ImGuiCol_Border]           = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
  style.Colors[ImGuiCol_BorderShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

  style.Colors[ImGuiCol_Separator]        = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.75f);
  style.Colors[ImGuiCol_SeparatorActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);


  // Frames (Inputs, Sliders, Checkboxes)
  style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
  style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.30f, 0.30f, 0.30f, 0.70f);
  style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);

  style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
  style.Colors[ImGuiCol_InputTextCursor]  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);


  // Sliders
  style.Colors[ImGuiCol_SliderGrab]        = ImVec4(0.40f, 0.40f, 0.40f, 0.70f);
  style.Colors[ImGuiCol_SliderGrabActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);


  // Buttons
  style.Colors[ImGuiCol_Button]            = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
  style.Colors[ImGuiCol_ButtonHovered]     = ImVec4(0.30f, 0.30f, 0.30f, 0.80f);
  style.Colors[ImGuiCol_ButtonActive]      = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);


  // Headers (CollapsingHeader, TreeNode, Selectable)
  style.Colors[ImGuiCol_Header]            = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
  style.Colors[ImGuiCol_HeaderHovered]     = ImVec4(0.30f, 0.30f, 0.30f, 0.70f);
  style.Colors[ImGuiCol_HeaderActive]      = ImVec4(0.35f, 0.35f, 0.35f, 0.90f);


  // Title Bar / Menu Bar
  style.Colors[ImGuiCol_TitleBg]            = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  style.Colors[ImGuiCol_TitleBgActive]      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);

  style.Colors[ImGuiCol_MenuBarBg]          = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);


  // Tabs
  style.Colors[ImGuiCol_Tab]                         = ImVec4(0.15f, 0.15f, 0.15f, 0.80f);
  style.Colors[ImGuiCol_TabHovered]                  = ImVec4(0.30f, 0.30f, 0.30f, 0.90f);

  style.Colors[ImGuiCol_TabSelected]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  style.Colors[ImGuiCol_TabSelectedOverline]         = ImVec4(0.40f, 0.60f, 0.90f, 1.00f);

  style.Colors[ImGuiCol_TabDimmed]                   = ImVec4(0.10f, 0.10f, 0.10f, 0.80f);
  style.Colors[ImGuiCol_TabDimmedSelected]           = ImVec4(0.15f, 0.15f, 0.15f, 0.90f);
  style.Colors[ImGuiCol_TabDimmedSelectedOverline]   = ImVec4(0.40f, 0.60f, 0.90f, 0.80f);


  // Plots
  style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);


  // Tables
  style.Colors[ImGuiCol_TableHeaderBg]       = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  style.Colors[ImGuiCol_TableBorderStrong]   = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_TableBorderLight]    = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_TableRowBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_TableRowBgAlt]       = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);


  // Trees / Drag & Drop / Markers
  style.Colors[ImGuiCol_TreeLines]          = ImVec4(0.35f, 0.35f, 0.35f, 0.60f);

  style.Colors[ImGuiCol_DragDropTarget]     = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  style.Colors[ImGuiCol_DragDropTargetBg]   = ImVec4(1.00f, 1.00f, 0.00f, 0.20f);

  style.Colors[ImGuiCol_UnsavedMarker]      = ImVec4(1.00f, 0.50f, 0.00f, 1.00f);


  // Navigation (Keyboard / Gamepad)
  style.Colors[ImGuiCol_NavCursor]             = ImVec4(0.40f, 0.60f, 0.90f, 1.00f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
}


// -------------------------------- Draw UI --------------------------------

void ImGuiUI::drawUI(const double fps, const double delta) {
  if (_tab_groups_visible)      { _renderTabGroupsUI(); }
  if (_hotkey_panel_visible)    { _renderHotkeyUI(); }
  if (_settings_panel_visible)  { _renderSettingsUI(fps, delta); }
}


// -------------------------------- UI Rendering Helpers --------------------------------

/**
 * @brief Outputs right-aligned for an ImGui call
 * @param fmt formatted text
 * @param ... formatted text vars
 */
void ImGuiRightAlignedText(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buf[128];
  vsnprintf(buf, sizeof(buf), fmt, args);

  va_end(args);

  float avail = ImGui::GetContentRegionAvail().x;
  float text_w = ImGui::CalcTextSize(buf).x;

  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - text_w);
  ImGui::TextUnformatted(buf);
}


// -------------------------------- UI Rendering --------------------------------


void ImGuiUI::_renderTabGroupsUI() {
  
}


void ImGuiUI::_renderHotkeyUI() {
  static constexpr ImGuiWindowFlags HOTKEY_PANEL_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

  if (_hotkey_layout_horizontal) { // horizontal Layout
    // Bottom-left corner placement
    ImVec2 size(
      Config::monitor_size.x * _HOTKEY_PANEL_LENGTH_PERCENT / 100.0f,
      Config::monitor_size.x * _HOTKEY_PANEL_HEIGHT_PERCENT / 100.0f
    );
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, Config::monitor_size.y), ImGuiCond_Always, _BOTTOM_LEFT_CORNER_POS);
    
    
    if (ImGui::Begin("Hotkeys", &_hotkey_panel_visible, HOTKEY_PANEL_FLAGS)) {
      // --- Main Content Area ---
      if (ImGui::BeginChild("Main Content Area")) {

      }
      ImGui::EndChild();
    }
    ImGui::End();
  }
  else { // Vertical layout
    // Bottom-left corner placement
    ImVec2 size(
      Config::monitor_size.x * _HOTKEY_PANEL_HEIGHT_PERCENT / 100.0f,
      Config::monitor_size.x * _HOTKEY_PANEL_LENGTH_PERCENT / 100.0f
    );
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, Config::monitor_size.y), ImGuiCond_Always, _BOTTOM_LEFT_CORNER_POS);
    
    if (ImGui::Begin("Hotkeys", &_hotkey_panel_visible, HOTKEY_PANEL_FLAGS)) {
      // --- Main Content Area ---
      if (ImGui::BeginChild("Main Content Area")) {

      }
      ImGui::EndChild();
    }
    ImGui::End();
  }
}


void ImGuiUI::_renderSettingsUI(const double fps, const double delta) {
  // Top-right corner placement
  ImVec2 size(
    Config::monitor_size.x * Config::settings_panel_width_percent / 100.0f,
    Config::monitor_size.y * Config::settings_panel_height_percent / 100.0f
  );

  ImGui::SetNextWindowSize(size, ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImVec2(Config::monitor_size.x, 0.0f), ImGuiCond_Always, _TOP_RIGHT_CORNER_POS);
  
  static constexpr ImGuiWindowFlags SETTINGS_PANEL_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  if (ImGui::Begin("Settings", &_settings_panel_visible, SETTINGS_PANEL_FLAGS)) {
    static const float TEXT_HEIGHT = ImGui::CalcTextSize("Placeholder Text").y;
    const float SETTINGS_PANEL_HEIGHT = ImGui::GetWindowSize().y;
    const float DESCRIPTION_POS = TEXT_HEIGHT * 2.7f * 1.67f; // SIX-SEVEN !!!
    const ImVec2 MAIN_CONTENT_SIZE(0.0f, -DESCRIPTION_POS);

    // --- Toolbar Area ---
    {
      // NOTE: Style/Color changes for the toolbar background can go here

      // Toolbar Buttons
      if (ImGui::Button("📂 New Project")) {
        // Handle 'New Project' action
      }
      ImGui::SameLine(); // Puts the next widget on the same line

      if (ImGui::Button("💾 Save")) {
        // Handle 'Save' action
      }
      ImGui::SameLine();

      if (ImGui::Button("↩️ Undo")) {
        // Handle 'Undo' action
      }
    }

    // Add some spacing between the toolbar and the main content
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    // --- Main Content Area ---
    if (ImGui::BeginChild("Main Content Area", MAIN_CONTENT_SIZE)) {
      if (ImGui::CollapsingHeader("Graphics Options")) {
        static bool vsync_checkbox;
        if (ImGui::Checkbox("VSync (Recommended)", &vsync_checkbox)) {
          Config::vsync = vsync_checkbox;
        }
      }
      
      if (ImGui::CollapsingHeader("Settings Panel Options")) {
        // Panel Width/Height
        constexpr float SETTINGS_PANEL_MIN_WIDTH = 20.0f;
        constexpr float SETTINGS_PANEL_MAX_WIDTH = 100.0f;
        constexpr float SETTINGS_PANEL_MIN_HEIGHT = 20.0f;
        constexpr float SETTINGS_PANEL_MAX_HEIGHT = 100.0f;
        static const float SLIDER_WIDTH = ImGui::GetFontSize() * 0.80f * 4.0f; // Boxes are the size of 4 characters
        static float width_tmp = Config::settings_panel_width_percent;
        static float height_tmp = Config::settings_panel_height_percent;
        ImGui::PushItemWidth(SLIDER_WIDTH);
        if (ImGui::InputFloat("Panel Width (%)", &width_tmp, 0.0f, 0.0f, "%.1f")) {
          width_tmp = std::clamp(width_tmp, SETTINGS_PANEL_MIN_WIDTH, SETTINGS_PANEL_MAX_WIDTH);
        }
        if (!ImGui::IsItemActive() && (width_tmp != Config::settings_panel_width_percent)) {
          Config::settings_panel_width_percent = width_tmp;
        }
        ImGui::SameLine(0.0f, SLIDER_WIDTH);
        if (ImGui::InputFloat("Panel Height (%)", &height_tmp, 0.0f, 0.0f, "%.1f")) {
          height_tmp = std::clamp(height_tmp, SETTINGS_PANEL_MIN_HEIGHT, SETTINGS_PANEL_MAX_HEIGHT);
        }
        if (!ImGui::IsItemActive() && (height_tmp != Config::settings_panel_height_percent)) {
          Config::settings_panel_height_percent = height_tmp;
        }
        ImGui::PopItemWidth();
      }
      
      if (ImGui::CollapsingHeader("Hotkey Panel Options")) {
        ImGui::Checkbox("Horizontal Layout", &_hotkey_layout_horizontal);
      }
    }
    ImGui::EndChild();

    // Add some spacing between the main content and the application data
    //ImGui::SetCursorPosY(DESCRIPTION_POS); // 5% as padding
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    // --- BetterAltTab Data -- (Bottom of the window) ---
    {
      _fps_display_accumulator += delta;
      if (_fps_display_accumulator > _FPS_DISPLAY_UPDATE_DELAY) {
        _fps_display_accumulator -= _FPS_DISPLAY_UPDATE_DELAY;
      }
      const float display_fps = std::min(fps, 999.0);
      ImGui::Text("%4.0f fps    %3.0f ms", fps, (delta * 1000.0));
      ImGui::Text("BetterAltTab %s", Config::VERSION);
    }
  }
  ImGui::End();
}
