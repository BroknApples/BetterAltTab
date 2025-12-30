#include "config.hpp"
 

// ---------------- Static declarations ----------------

// Json
JsonReader Config::_json_reader = JsonReader();


// Constants/States

bool Config::initialized = false;
ImVec2 Config::monitor_size = ImVec2(1920, 1080); // TODO: Read from save file the saved monitor size for default value


// ----------------- Settings -----------------

// Graphics
float Config::tab_groups_tab_width = 640.0f;
float Config::tab_groups_tab_height = 360.0f;

// Hotkey Panel
bool Config::hotkey_panel_horizontal_layout = false;
std::array<Keybind, 10> Config::keybinds = {
  Keybind{ImGuiKey_1},
  Keybind{ImGuiKey_2},
  Keybind{ImGuiKey_3},
  Keybind{ImGuiKey_4},
  Keybind{ImGuiKey_5},
  Keybind{ImGuiKey_6},
  Keybind{ImGuiKey_7},
  Keybind{ImGuiKey_8},
  Keybind{ImGuiKey_9},
  Keybind{ImGuiKey_0}
};

// Settings Panel
float Config::settings_panel_width_percent = 20.0f;  // 20% of the screen
float Config::settings_panel_height_percent = 100.0f; // Full screen height

// Graphics
bool Config::vsync = true;


// ---------------- init & save ----------------

void Config::init() {
  // TODO: Implement a config.json creating system IF there is no existing file.

  resetToSaved();

  // In-program initialized vars
  monitor_size = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  initialized = true;
}


/**
 * @brief Gets the ImGuiKey value from it's charname
 * @param name: Name of the char in string form
 * @returns ImGuiKey: Key code for ImGui
 */
ImGuiKey _ImGuiKeyFromName(const std::string& name) {
  for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
    const char* key_name = ImGui::GetKeyName(static_cast<ImGuiKey>(key));
    if (key_name && strcmp(key_name, name.c_str()) == 0) return (static_cast<ImGuiKey>(key));
  }

  return ImGuiKey_None;
};


bool Config::save() {
  // Load current values into the json reader
  {
    // Tab Groups
    _json_reader.setDouble(_TAB_GROUPS_TAB_WIDTH, tab_groups_tab_width);
    _json_reader.setDouble(_TAB_GROUPS_TAB_HEIGHT, tab_groups_tab_height);

    // Hotkey Panel
    _json_reader.setBool(_HOTKEY_PANEL_HORIZONTAL_LAYOUT, hotkey_panel_horizontal_layout);
    _json_reader.setString(_KEYBIND_SLOT1, ImGui::GetKeyName(keybinds[0].key));
    _json_reader.setString(_KEYBIND_SLOT2, ImGui::GetKeyName(keybinds[1].key));
    _json_reader.setString(_KEYBIND_SLOT3, ImGui::GetKeyName(keybinds[2].key));
    _json_reader.setString(_KEYBIND_SLOT4, ImGui::GetKeyName(keybinds[3].key));
    _json_reader.setString(_KEYBIND_SLOT5, ImGui::GetKeyName(keybinds[4].key));
    _json_reader.setString(_KEYBIND_SLOT6, ImGui::GetKeyName(keybinds[5].key));
    _json_reader.setString(_KEYBIND_SLOT7, ImGui::GetKeyName(keybinds[6].key));
    _json_reader.setString(_KEYBIND_SLOT8, ImGui::GetKeyName(keybinds[7].key));
    _json_reader.setString(_KEYBIND_SLOT9, ImGui::GetKeyName(keybinds[8].key));
    _json_reader.setString(_KEYBIND_SLOT10, ImGui::GetKeyName(keybinds[9].key));

    // Settings Panel
    _json_reader.setDouble(_SETTINGS_PANEL_WIDTH_PERCENT, settings_panel_width_percent);
    _json_reader.setDouble(_SETTINGS_PANEL_HEIGHT_PERCENT, settings_panel_height_percent);

    // Graphics
    _json_reader.setBool(_VSYNC, vsync);
  }

  return _json_reader.saveToFile(CONFIG_SAVE_PATH);
}

void Config::resetToSaved() {
  // Load from file -> into each var
  _json_reader.loadFromFile(CONFIG_SAVE_PATH);

  // Tab Groups
  tab_groups_tab_width  = _json_reader.getDouble(_TAB_GROUPS_TAB_WIDTH, _TAB_GROUPS_TAB_WIDTH_DEFAULT);
  tab_groups_tab_height = _json_reader.getDouble(_TAB_GROUPS_TAB_HEIGHT, _TAB_GROUPS_TAB_HEIGHT_DEFAULT);

  // Hotkey Panel
  hotkey_panel_horizontal_layout = _json_reader.getBool(_HOTKEY_PANEL_HORIZONTAL_LAYOUT, _HOTKEY_PANEL_HORIZONTAL_LAYOUT_DEFAULT);
  keybinds[0].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT1, _KEYBIND_SLOT1_DEFAULT));
  keybinds[1].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT2, _KEYBIND_SLOT2_DEFAULT));
  keybinds[2].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT3, _KEYBIND_SLOT3_DEFAULT));
  keybinds[3].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT4, _KEYBIND_SLOT4_DEFAULT));
  keybinds[4].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT5, _KEYBIND_SLOT5_DEFAULT));
  keybinds[5].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT6, _KEYBIND_SLOT6_DEFAULT));
  keybinds[6].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT7, _KEYBIND_SLOT7_DEFAULT));
  keybinds[7].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT8, _KEYBIND_SLOT8_DEFAULT));
  keybinds[8].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT9, _KEYBIND_SLOT9_DEFAULT));
  keybinds[9].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT10, _KEYBIND_SLOT10_DEFAULT));

  // Settings Panel
  settings_panel_width_percent = _json_reader.getDouble(_SETTINGS_PANEL_WIDTH_PERCENT, _SETTINGS_PANEL_WIDTH_PERCENT_DEFAULT);
  settings_panel_height_percent = _json_reader.getDouble(_SETTINGS_PANEL_HEIGHT_PERCENT, _SETTINGS_PANEL_HEIGHT_PERCENT_DEFAULT);

  // Graphics
  vsync = _json_reader.getBool(_VSYNC);
}


void Config::resetToDefault() {
  // Tab Groups
  tab_groups_tab_width  = _TAB_GROUPS_TAB_WIDTH_DEFAULT;
  tab_groups_tab_height = _TAB_GROUPS_TAB_HEIGHT_DEFAULT;

  // Hotkey Panel
  hotkey_panel_horizontal_layout = _HOTKEY_PANEL_HORIZONTAL_LAYOUT_DEFAULT;
  keybinds[0].key = _ImGuiKeyFromName(_KEYBIND_SLOT1_DEFAULT);
  keybinds[1].key = _ImGuiKeyFromName(_KEYBIND_SLOT2_DEFAULT);
  keybinds[2].key = _ImGuiKeyFromName(_KEYBIND_SLOT3_DEFAULT);
  keybinds[3].key = _ImGuiKeyFromName(_KEYBIND_SLOT4_DEFAULT);
  keybinds[4].key = _ImGuiKeyFromName(_KEYBIND_SLOT5_DEFAULT);
  keybinds[5].key = _ImGuiKeyFromName(_KEYBIND_SLOT6_DEFAULT);
  keybinds[6].key = _ImGuiKeyFromName(_KEYBIND_SLOT7_DEFAULT);
  keybinds[7].key = _ImGuiKeyFromName(_KEYBIND_SLOT8_DEFAULT);
  keybinds[8].key = _ImGuiKeyFromName(_KEYBIND_SLOT9_DEFAULT);
  keybinds[9].key = _ImGuiKeyFromName(_KEYBIND_SLOT10_DEFAULT);

  // Settings Panel
  settings_panel_width_percent = _SETTINGS_PANEL_WIDTH_PERCENT_DEFAULT;
  settings_panel_height_percent = _SETTINGS_PANEL_HEIGHT_PERCENT_DEFAULT;

  // Graphics
  vsync = _VSYNC_DEFAULT;

  // Save default settings
  save();
}
