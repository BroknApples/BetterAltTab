#include "config.hpp"
 

// ---------------- Static declarations ----------------

// Json
JsonReader Config::_json_reader = JsonReader();


// Constants/States

bool Config::initialized = false;
ImVec2 Config::monitor_size = ImVec2(1920, 1080); // TODO: Read from save file the saved monitor size for default value


// ----------------- Settings -----------------

// Graphics
bool Config::vsync = true;

// Settings Panel
float Config::settings_panel_width_percent = 20.0f;  // 20% of the screen
float Config::settings_panel_height_percent = 100.0f; // Full screen height

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


// ---------------- init & save ----------------

void Config::init() {
  resetToSaved();

  // In-program initialized vars
  monitor_size = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  initialized = true;
}


bool Config::save() {
  // Load current values into the json reader
  {
    // Graphics
    _json_reader.setBool(_VSYNC, vsync);

    // Settings Panel
    _json_reader.setDouble(_SETTINGS_PANEL_WIDTH_PERCENT, static_cast<double>(settings_panel_width_percent));
    _json_reader.setDouble(_SETTINGS_PANEL_HEIGHT_PERCENT, static_cast<double>(settings_panel_height_percent));

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
  }

  return _json_reader.saveToFile(CONFIG_SAVE_PATH);
}

void Config::resetToSaved() {
  /**
   * @brief Gets the ImGuiKey value from it's charname
   */
  static auto _ImGuiKeyFromName = [](const std::string& name) {
    for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
      const char* key_name = ImGui::GetKeyName(static_cast<ImGuiKey>(key));
      if (key_name && strcmp(key_name, name.c_str()) == 0) return (static_cast<ImGuiKey>(key));
    }
    return ImGuiKey_None;
  };

  // Load from file -> into each var
  _json_reader.loadFromFile(CONFIG_SAVE_PATH);

  // Graphics
  vsync = _json_reader.getBool(_VSYNC);

  // Settings Panel
  settings_panel_width_percent = static_cast<float>(_json_reader.getDouble(_SETTINGS_PANEL_WIDTH_PERCENT));
  settings_panel_height_percent = static_cast<float>(_json_reader.getDouble(_SETTINGS_PANEL_HEIGHT_PERCENT));

  // Hotkey Panel
  hotkey_panel_horizontal_layout = _json_reader.getBool(_HOTKEY_PANEL_HORIZONTAL_LAYOUT);
  keybinds[0].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT1));
  keybinds[1].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT2));
  keybinds[2].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT3));
  keybinds[3].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT4));
  keybinds[4].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT5));
  keybinds[5].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT6));
  keybinds[6].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT7));
  keybinds[7].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT8));
  keybinds[8].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT9));
  keybinds[9].key = _ImGuiKeyFromName(_json_reader.getString(_KEYBIND_SLOT10));
}


void Config::resetToDefault() {
  TODO:
  return;
}
