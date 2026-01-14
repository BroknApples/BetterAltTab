#include "imgui_ui.hpp"


// ----------------- Static Vars -----------------

bool ImGuiUI::_window_just_focused       = false;
int ImGuiUI::_redraw_moving_frames_count = 0;
bool ImGuiUI::_needs_io_redraw           = false;

bool ImGuiUI::_tab_groups_visible     = false;
bool ImGuiUI::_hotkey_panel_visible   = false;
bool ImGuiUI::_settings_panel_visible = false;

double ImGuiUI::_fps_display_accumulator = 0.0;
bool ImGuiUI::_request_saved_config_reset = false;
int ImGuiUI::_tab_marker_pos = 0;


// ----------------- Private Functions -----------------

// -------------------------------- UI Rendering Helpers --------------------------------


void ImGuiUI::_ImGuiRightAlignedText(const char* fmt, ...) {
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


std::string ImGuiUI::_fitStringToWidth(const std::string& str, const float max_width, const bool ellipses) {
  // Don't check empty strings
  if (str.empty()) return "";

  // Already fits
  if (ImGui::CalcTextSize(str.c_str()).x <= max_width) {
    return str;
  }

  const int LEN = static_cast<int>(str.size());
  int left = 0;
  int right = LEN;
  int fit = 0; // end idx

  float available_width = max_width;
  if (ellipses) {
    available_width -= ImGui::CalcTextSize("...").x;
  }

  // Binary search method
  while (left <= right) {
    const int mid = left + (left + right) / 2;
    const std::string sub = str.substr(0, mid);
    const ImVec2 size = ImGui::CalcTextSize(sub.c_str());
    //std::cout << "str: " << sub << " | len: " << size.x << "\n";

    if (size.x <= available_width) {
      fit = mid;       // this length fits
      left = mid + 1;  // try longer
    }
    else {
      right = mid - 1; // try shorter
    }
  }

  // Build string
  std::string result = str.substr(0, fit);

  // Add ellipsis if truncated
  if (ellipses) {
    result += "...";
  }

  return result;
}


// -------------------------------- UI Rendering --------------------------------


void ImGuiUI::_renderTabCell(TabGroupMap& tabs, const std::string& group_title, const std::shared_ptr<WindowInfo>& info, const TabGroupLayout layout, const ImVec2 cell_size, const int cell_idx) {
  // Total size: image + text
  ImGuiStyle& style = ImGui::GetStyle();
  const float LINE_HEIGHT = ImGui::GetTextLineHeight();
  const ImVec2 CELL_POS = ImGui::GetCursorScreenPos();
  const ImVec2 PADDING = style.FramePadding;
  const ImVec2 TOTAL_SIZE = ImVec2((cell_size.x + PADDING.x), (cell_size.y + (LINE_HEIGHT * 2.0f) + PADDING.y));
  const std::string LABEL_ID = info->title + "_" + group_title;

  // Get text substr
  const std::string TEXT_SUBSTR = _fitStringToWidth(info->title, cell_size.x, true);
  const ImVec2 TEXT_SIZE = ImGui::CalcTextSize(TEXT_SUBSTR.c_str());
  
  // Push ID
  ImGui::PushID(info.get());
  
  // Create selectable area --> Acts as a button.
  bool selected = false;
  const bool activated = ImGui::Selectable(
    "##Cell",
    selected,
    ImGuiSelectableFlags_AllowDoubleClick,
    TOTAL_SIZE
  );

  // Context-menu
  if (ImGui::BeginPopupContextItem("MyButtonContext")) {
    /*
    What goes here?
    
    - Pin tab (for the current tab group, add sorting option to keep at the top of
               the list) (also adds a pin icon on the top right when rendering)
    - Add to tab group
    - Remove from current tab group (except for "Open Tabs" | it must always exist there)
    - Add hotkeys
      * slot 1
      * slot 2
      * ...
      * slot 10
    - Open in file explorer
    
    */

    if (ImGui::MenuItem("Pin Tab")) {
      // TODO: Implement.
    }
    if (ImGui::BeginMenu("Add to tab group")) {
      // TODO: Disable menu items if the current value already exists in the group
      if (ImGui::MenuItem("Group 1")) { std::cout << "Added to Group 1\n"; }
      if (ImGui::MenuItem("Group 2")) { std::cout << "Added to Group 2\n"; }
      if (ImGui::MenuItem("Group 3")) { std::cout << "Added to Group 3\n"; }
      ImGui::EndMenu();
    }

    // Remove from the current tab group
    const bool remove_allowed = (info->title == StaticTabGroups::OPEN_TABS);
    if (ImGui::MenuItem("Remove from tab group", nullptr, false, remove_allowed)) {
      // TODO: Implement
    }

    // Add hotkey for item
    if (ImGui::BeginMenu("Add hotkey")) {
      TabGroup& hotkeys = tabs.at(StaticTabGroups::HOTKEYS);
      if (ImGui::MenuItem("Slot 1"))  { hotkeys[0] = info; }
      if (ImGui::MenuItem("Slot 2"))  { hotkeys[1] = info; }
      if (ImGui::MenuItem("Slot 3"))  { hotkeys[2] = info; }
      if (ImGui::MenuItem("Slot 4"))  { hotkeys[3] = info; }
      if (ImGui::MenuItem("Slot 5"))  { hotkeys[4] = info; }
      if (ImGui::MenuItem("Slot 6"))  { hotkeys[5] = info; }
      if (ImGui::MenuItem("Slot 7"))  { hotkeys[6] = info; }
      if (ImGui::MenuItem("Slot 8"))  { hotkeys[7] = info; }
      if (ImGui::MenuItem("Slot 9"))  { hotkeys[8] = info; }
      if (ImGui::MenuItem("Slot 10")) { hotkeys[9] = info; }
      ImGui::EndMenu();
    }

    // Open in file explorer
    if (ImGui::MenuItem("Open in file explorer")) {
      std::wstring path;
      getWindowExecutablePath(info->hwnd, path);
      openWindowsExplorerAtPath(path);
    }
    ImGui::EndPopup();
  }


  const ImVec2 TEXT_POS = ImVec2(
    CELL_POS.x + (cell_size.x - TEXT_SIZE.x) * 0.5f,
    CELL_POS.y + 5.0f // small top padding
  );

  const ImVec2 IMAGE_POS_0 = ImVec2(
    CELL_POS.x + (cell_size.x - cell_size.x) * 0.5f, // center horizontally (just cell_pos.x)
    CELL_POS.y + TEXT_SIZE.y + 5.0f                  // below text
  );
  
  const ImVec2 IMAGE_POS_1 = ImVec2(
    IMAGE_POS_0.x + cell_size.x,
    IMAGE_POS_0.y + cell_size.y
  );
  
  // Draw
  ImDrawList* dl = ImGui::GetWindowDrawList();
  dl->AddText(TEXT_POS, IM_COL32_WHITE, TEXT_SUBSTR.c_str());
  dl->AddImage(reinterpret_cast<ImTextureID>(info->tex), IMAGE_POS_0, IMAGE_POS_1);

  if (cell_idx == _tab_marker_pos) {
    // Sizes for the outline (include both text and image)
    const float BORDER_SIZE = 2.0f;
    ImVec2 border_min = ImVec2(
      CELL_POS.x, 
      CELL_POS.y
    );

    ImVec2 border_max = ImVec2(
      CELL_POS.x + TOTAL_SIZE.x, 
      CELL_POS.y + TOTAL_SIZE.y
    );

    dl->AddRect(
      border_min,
      border_max,
      IM_COL32(255, 255, 255, 255),
      0.0f,   // no rounding
      0,      // no specific flags
      BORDER_SIZE
    );
  }
  
  if (activated) {
    focusWindow(info->hwnd);
    setWindowJustFocused(true);
  }

  // Remove unique ID for this cell
  ImGui::PopID();
}


void ImGuiUI::_renderTabGroup(TabGroupMap& tab_groups, const std::string& title, const TabGroup tabs, const TabGroupLayout layout) {
  // Constants
  static constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_NoPadOuterX | ImGuiWindowFlags_AlwaysVerticalScrollbar;
  const ImVec2 CELL_SIZE = ImVec2(Config::tab_groups_tab_width, Config::tab_groups_tab_height);
  const float PADDING = ImGui::GetStyle().ItemSpacing.x;
  const float AVAIL_X = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize;
  const int COLUMNS = std::max(static_cast<int>(AVAIL_X / (CELL_SIZE.x + PADDING)), 1); // Must have AT LEAST 1 column
  const std::string TABLE_NAME = std::string(title + " - Tab Grid");

  // Draw table
  if (ImGui::BeginTable(TABLE_NAME.c_str(), COLUMNS, TABLE_FLAGS)) {
    int cell_idx = 0;
    for (const auto& tab : tabs) {
      // Skip broken tabs
      if (tab == nullptr) {
        std::cout << "NULL tab detected in: '" << title << "'\n";
        continue;
      };

      // Render cell
      ImGui::TableNextColumn();

      // Add manual padding to the first column
      // TODO: Find a way to fix the cell rendering so this isn't needed
      if (ImGui::TableGetColumnIndex() == 0) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().WindowPadding.x);
      }

      _renderTabCell(tab_groups, title, tab, layout, CELL_SIZE, cell_idx);
      cell_idx++;
    }
    ImGui::EndTable();
  }
}


void ImGuiUI::_renderTabGroupsUI(TabGroupMap& tab_groups, TabGroupOrderList& tab_groups_order, const TabGroupLayoutList& tab_groups_layouts) {
  static constexpr ImGuiWindowFlags WINDOW_FLAGS = ImGuiCond_None;

  // Static maps to store last position and size per window
  static std::unordered_map<std::string, ImVec2> last_pos;
  static std::unordered_map<std::string, ImVec2> last_size;

  TabGroupOrderList new_order = tab_groups_order;
  
  // NOTE: Render starting at index 1 since HOTKEYS will always be index 1.
  const int TOTAL_TAB_GROUPS = tab_groups_order.size();
  for (int i = 1; i < TOTAL_TAB_GROUPS; i++) {
    const std::string& title = tab_groups_order[i];
    auto& tabs = tab_groups[title];

    // Sort by last focused time
    std::sort(tabs.begin(), tabs.end(),
      [](const std::shared_ptr<WindowInfo>& a, const std::shared_ptr<WindowInfo>& b) {
        if (a == nullptr) return false;
        if (b == nullptr) return true;
        return a->last_focused > b->last_focused;
      }
    );

    // Create window
    const TabGroupLayout LAYOUT = tab_groups_layouts.at(title);
    if (ImGui::Begin(title.c_str(), nullptr, WINDOW_FLAGS)) {
      // Render the tab group
      _renderTabGroup(tab_groups, title, tabs, LAYOUT);

      // Window was clicked, set to index 1 in the order list
      if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const auto it = std::find(new_order.begin(), new_order.end(), title);
        if (it != new_order.end() && it != new_order.begin() + 1) {
          new_order.erase(it);
          new_order.insert(new_order.begin() + 1, title);
        }
      }

      // --- Check if it needs a redraw ---

      const ImVec2 pos = ImGui::GetWindowPos();
      const ImVec2 size = ImGui::GetWindowSize();
      auto& lp = last_pos[title]; // default-constructed if first frame
      auto& ls = last_size[title];

      // Compare with previous frame, mark redraw if changed
      if (lp.x != pos.x || lp.y != pos.y || ls.x != size.x || ls.y != size.y) {
        setNeedsMovingRedraw(true);
      }
      lp = pos;
      ls = size;
    }
    ImGui::End();
  }

  tab_groups_order = std::move(new_order);;
}


void ImGuiUI::_renderHotkeyUI(const TabGroup& hotkeys, const TabGroupLayout hotkey_layout) {
  static constexpr ImGuiWindowFlags HOTKEY_PANEL_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

  if (Config::hotkey_panel_horizontal_layout) { // horizontal Layout
    // Bottom-left corner placement
    const ImVec2 hotkey_window_size(
      Config::monitor_size.x * _HOTKEY_PANEL_LENGTH_PERCENT / 100.0f,
      Config::monitor_size.x * _HOTKEY_PANEL_HEIGHT_PERCENT / 100.0f
    );
    ImGui::SetNextWindowSize(hotkey_window_size, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, Config::monitor_size.y), ImGuiCond_Always, _BOTTOM_LEFT_CORNER_POS);
    
    
    if (ImGui::Begin("Hotkeys", &_hotkey_panel_visible, HOTKEY_PANEL_FLAGS)) {
      // --- Main Content Area ---
      if (ImGui::BeginChild("Main Content Area")) {
        for (int i = 0; i < 10; i++) {
          if (hotkeys[i] == nullptr) continue;
          ImGui::Text("[%d] %s", i + 1, hotkeys[i]->title.c_str());
        }
      }
      ImGui::EndChild();


      // Check if it needs a redraw
      static ImVec2 last_pos = ImGui::GetWindowPos();
      static ImVec2 last_size = ImGui::GetWindowSize();
      ImVec2 pos = ImGui::GetWindowPos();
      ImVec2 size = ImGui::GetWindowSize();
      if (pos.x != last_pos.x || pos.y != last_pos.y ||
          size.x != last_size.x || size.y != last_size.y) {
        setNeedsMovingRedraw(true);
      }
      last_pos = pos;
      last_size = size;
    }
    ImGui::End();
  }
  else { // Vertical layout
    // Bottom-left corner placement
    const ImVec2 size(
      Config::monitor_size.x * _HOTKEY_PANEL_HEIGHT_PERCENT / 100.0f,
      Config::monitor_size.x * _HOTKEY_PANEL_LENGTH_PERCENT / 100.0f
    );
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, Config::monitor_size.y), ImGuiCond_Always, _BOTTOM_LEFT_CORNER_POS);
    
    if (ImGui::Begin("Hotkeys", &_hotkey_panel_visible, HOTKEY_PANEL_FLAGS)) {
      // --- Main Content Area ---
      if (ImGui::BeginChild("Main Content Area")) {
        for (int i = 0; i < 10; i++) {
          if (hotkeys[i] == nullptr) continue;
          ImGui::Text("[%d] %s", i + 1, hotkeys[i]->title.c_str());
        }
      }
      ImGui::EndChild();

      // Check if it needs a redraw
      static ImVec2 last_pos = ImGui::GetWindowPos();
      static ImVec2 last_size = ImGui::GetWindowSize();
      const ImVec2 pos = ImGui::GetWindowPos();
      const ImVec2 size = ImGui::GetWindowSize();
      if (pos.x != last_pos.x || pos.y != last_pos.y ||
          size.x != last_size.x || size.y != last_size.y) {
        setNeedsMovingRedraw(true);
      }
      last_pos = pos;
      last_size = size;
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
    static const float DESCRIPTION_POS = TEXT_HEIGHT * 1.67f * 1.67f; // SIX-SEVEN !!!
    const ImVec2 MAIN_CONTENT_SIZE(0.0f, -DESCRIPTION_POS);

    // --- Toolbar Area ---
    {
      // NOTE: Style/Color changes for the toolbar background can go here

      if (ImGui::Button("Save")) {
        // Handle 'Save' action
        if (Config::save()) {
          std::cout << "Saved Config as '" << Config::CONFIG_SAVE_PATH << "'" << std::endl;
        }
        else {
          std::cout << "Error saving config." << std::endl;
        }
      }
      ImGui::SameLine();

      if (ImGui::Button("↩ Undo")) {
        // Handle 'Undo' action
        _request_saved_config_reset = true;
        std::cout << "Removing staged changes from the config." << std::endl;
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
      if (ImGui::CollapsingHeader("Tab Groups Options")) {
        // Tab Item Width/Height
        {
          constexpr float TAB_GROUPS_TAB_ITEM_MIN_WIDTH = 360.0f;
          constexpr float TAB_GROUPS_TAB_ITEM_MAX_WIDTH = 1920.0f;
          constexpr float TAB_GROUPS_TAB_ITEM_MIN_HEIGHT = 203.0f;
          constexpr float TAB_GROUPS_TAB_ITEM_MAX_HEIGHT = 1080.0f;
          static const float SLIDER_WIDTH = ImGui::GetFontSize() * 0.80f * 4.0f; // Boxes are the size of 4 characters
          static float width_tmp = Config::tab_groups_tab_width;
          static float height_tmp = Config::tab_groups_tab_height;

          _syncTemp(width_tmp, Config::tab_groups_tab_width);
          _syncTemp(height_tmp,Config::tab_groups_tab_height);

          ImGui::PushItemWidth(SLIDER_WIDTH);
          if (ImGui::InputFloat("Tab Width (px)", &width_tmp, 0.0f, 0.0f, "%.1f")) {
            width_tmp = std::clamp(width_tmp, TAB_GROUPS_TAB_ITEM_MIN_WIDTH, TAB_GROUPS_TAB_ITEM_MAX_WIDTH);
          }
          if (!ImGui::IsItemActive() && (width_tmp != Config::tab_groups_tab_width)) {
            Config::tab_groups_tab_width = width_tmp;
          }
          ImGui::SameLine(0.0f, SLIDER_WIDTH);
          if (ImGui::InputFloat("Tab Height (px)", &height_tmp, 0.0f, 0.0f, "%.1f")) {
            height_tmp = std::clamp(height_tmp, TAB_GROUPS_TAB_ITEM_MIN_HEIGHT, TAB_GROUPS_TAB_ITEM_MAX_HEIGHT);
          }
          if (!ImGui::IsItemActive() && (height_tmp != Config::tab_groups_tab_height)) {
            Config::tab_groups_tab_height = height_tmp;
          }
          ImGui::PopItemWidth();
        }
      }
      
      if (ImGui::CollapsingHeader("Hotkey Panel Options")) {
        ImGui::Indent();
        if (ImGui::CollapsingHeader("Keybinds")) {
          static constexpr const char* PRESS_KEY_PLACEHOLDER = "...";
          static int active_bind = -1;

          // TODO: Fix
          if (active_bind != -1) {
            ImGuiIO& io = ImGui::GetIO();
            bool ctrl = io.KeyCtrl;
            bool shift = io.KeyShift;
            bool alt = io.KeyAlt;
            // bool super = io.KeySuper;

            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
              active_bind = -1;
            }

            for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
              ImGuiKey k = static_cast<ImGuiKey>(key);

              // Skip modifiers
              if (k == ImGuiMod_Ctrl || k == ImGuiMod_Shift ||
                  k == ImGuiMod_Alt  || k == ImGuiMod_Super) {
                continue;
              }

              if (ImGui::IsKeyPressed(k)) {
                Config::keybinds[active_bind] = Keybind{k, ctrl, shift, alt};
                active_bind = -1;
                break;
              }
            }
          }

          // Render each keybind slot's saved key combination.
          for (int i = 0; i < 10; i++) {
            // Text label
            std::stringstream ss;
            ss << "Slot #" << std::left << std::setw(2) << (i + 1); 
            const std::string text = ss.str();

            // Button text
            const char* button = (
              (active_bind == i) ? PRESS_KEY_PLACEHOLDER : ImGui::GetKeyName(Config::keybinds[i].key)
            );

            ImGui::Text(text.c_str());
            ImGui::SameLine();
            if (ImGui::Button(button)) {
              active_bind = i;
            }
          }
        }
        ImGui::Unindent();


        // Other Settings

        ImGui::Checkbox("Horizontal Layout", &Config::hotkey_panel_horizontal_layout);
      }

      if (ImGui::CollapsingHeader("Settings Panel Options")) {
        // Panel Width/Height
        {
          constexpr float SETTINGS_PANEL_MIN_WIDTH = 20.0f;
          constexpr float SETTINGS_PANEL_MAX_WIDTH = 100.0f;
          constexpr float SETTINGS_PANEL_MIN_HEIGHT = 20.0f;
          constexpr float SETTINGS_PANEL_MAX_HEIGHT = 100.0f;
          static const float SLIDER_WIDTH = ImGui::GetFontSize() * 0.80f * 4.0f; // Boxes are the size of 4 characters
          static float width_tmp = Config::settings_panel_width_percent;
          static float height_tmp = Config::settings_panel_height_percent;

          _syncTemp(width_tmp, Config::settings_panel_width_percent);
          _syncTemp(height_tmp,Config::settings_panel_height_percent);

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
      }

      if (ImGui::CollapsingHeader("Graphics Options")) {
        (ImGui::Checkbox("VSync (Recommended)", &Config::vsync));
      }
    }
    ImGui::EndChild();


    // Add some spacing between the main content and the application data
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
      const float TEXT_Y_POS = ImGui::GetCursorPosY();
      ImGui::Text("%4.0f fps    %3.0f ms    %s", std::min(fps, 999.0), (delta * 1000.0), getFormattedRamUsage().c_str());
      ImGui::SetCursorPosY(TEXT_Y_POS);
      ImGuiUI::_ImGuiRightAlignedText("BetterAltTab %s", Config::VERSION);
    }




    // ------------ Check if it needs a redraw ------------

    static ImVec2 last_pos = ImGui::GetWindowPos();
    static ImVec2 last_size = ImGui::GetWindowSize();
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();
    if (pos.x != last_pos.x || pos.y != last_pos.y ||
        size.x != last_size.x || size.y != last_size.y) {
      setNeedsMovingRedraw(true);
    }
    last_pos = pos;
    last_size = size;
  }
  ImGui::End();
}


// --------------------- Styles ---------------------

void ImGuiUI::setupImGuiStyles() {
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

void ImGuiUI::drawUI(const double fps, const double delta,
    TabGroupMap& tab_groups, TabGroupOrderList& tab_groups_order, const TabGroupLayoutList& tab_groups_layouts) {
  // Apply settings resets
  if (_request_saved_config_reset) {
    Config::resetToSaved();
    _request_saved_config_reset = false;
  }

  // User input = needs redraw
  ImGuiIO& io = ImGui::GetIO();
  bool userInteracted = io.WantCaptureMouse || io.WantCaptureKeyboard;
  if (userInteracted) {setNeedsIoRedraw(true); }

  if (_tab_groups_visible)      { _renderTabGroupsUI(tab_groups, tab_groups_order, tab_groups_layouts); }
  if (_hotkey_panel_visible)    { _renderHotkeyUI(tab_groups.at(StaticTabGroups::HOTKEYS), tab_groups_layouts.at(StaticTabGroups::HOTKEYS)); }
  if (_settings_panel_visible)  { _renderSettingsUI(fps, delta); }
}