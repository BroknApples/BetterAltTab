#include "widget_layer.hPP"


static uint64_t WIDGET_UNIQUE_ID = 1;


// ------------------- Private functions -------------------

void WidgetLayer::_updateActualSize() {
  if (_autofit) {
    // TODO: HERE
  }
  else {
    float determined_width = std::clamp(_curr_size.x, _min_size.x, std::max(_min_size.x, _max_size.x));
    float determined_height = std::clamp(_curr_size.y, _min_size.y, std::max(_min_size.y, _max_size.y));
    _actual_size = ImVec2(determined_width, determined_height);
  }
}


void WidgetLayer::_renderToolbar() {
  if (_toolbar_side == Top || _toolbar_side == Bottom) {
    // Horizontal toolbar
    if (ImGui::Button("A")) {}
    ImGui::SameLine();
    if (ImGui::Button("B")) {}
    ImGui::SameLine();
    if (ImGui::Button("C")) {}
  }
  else {
    // Vertical toolbar
    if (ImGui::Button("A")) {}
    if (ImGui::Button("B")) {}
    if (ImGui::Button("C")) {}
  }
}


// ------------------- Constructor -------------------

WidgetLayer::WidgetLayer(const char* layer_name, float transparency, bool* p_open, ImGuiWindowFlags window_flags)
: _toolbar_side(ToolbarSide::Top)
, _visible(true)
, _layer_name(layer_name)
, _layer_transparency(transparency)
, _p_open(p_open)
, _window_flags(window_flags)
, _layout(layer_name)
, _min_size(0.0f, 0.0f)
, _max_size(Config::monitor_size.x, Config::monitor_size.y)
, _curr_size(400.0f, 400.0f)
, _actual_size(400.0f, 300.0f) {
  _layout = WidgetContainer(layer_name);
  _layout.setLayout(LayoutType::Grid);  // Default layout
  _layout.setGridSize(2, 2);
}


// ------------------- Public functions -------------------

void WidgetLayer::addText(const std::string& name, const std::string& label) {
  _layout.addItem(ImGuiWidget{
    WIDGET_UNIQUE_ID++,
    TextData{ name, label }
  });
}


void WidgetLayer::addButton(const std::string& name, const std::string& label, const bool initial_state, std::function<void(bool)> callback) {
  _layout.addItem(ImGuiWidget{
    WIDGET_UNIQUE_ID++,
    ButtonData{ name, label, initial_state, callback }
  });
}


void WidgetLayer::addSlider(const std::string& name, const std::string& label, float min, float max, float value, std::function<void(float)> callback) {
  _layout.addItem(ImGuiWidget{
    WIDGET_UNIQUE_ID++,
    SliderData{ name, label, value, min, max, callback }
  });
}


void WidgetLayer::addCheckbox(const std::string& name, const std::string& label, bool checked, std::function<void(bool)> callback) {
  _layout.addItem(ImGuiWidget{
    WIDGET_UNIQUE_ID++,
    CheckboxData{ name, label, checked, callback }
  });
}


void WidgetLayer::render() {
  if (!_visible) return;

  if (_layer_transparency < 1.0f) {
    ImGui::SetNextWindowBgAlpha(_layer_transparency);
  }
  
  // Window Settings
  //ImGui::SetNextWindowSize(_actual_size, ImGuiCond_Once);
  ImGui::SetNextWindowSize(_actual_size, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(_min_size, _max_size);
  

  // Render
  if (ImGui::Begin(_layer_name, _p_open, _window_flags)) {
    const float thickness = 30.0f;
    ImVec2 avail = ImGui::GetContentRegionAvail();

    // --- TOP ---
    if (_toolbar_side == ToolbarSide::Top) {
      ImGui::BeginChild("Toolbar", ImVec2(avail.x, thickness), false);
      _renderToolbar();
      ImGui::EndChild();
    }

    // --- LEFT ---
    if (_toolbar_side == ToolbarSide::Left) {
      ImGui::BeginChild("Toolbar", ImVec2(thickness, avail.y), false);
      _renderToolbar();
      ImGui::EndChild();

      ImGui::SameLine();
    }

    // --- CONTENT ---
    ImVec2 content_size = avail;

    if (_toolbar_side == ToolbarSide::Top ||
        _toolbar_side == ToolbarSide::Bottom) {
      content_size.y -= thickness;
    }
    else if (_toolbar_side == ToolbarSide::Left ||
             _toolbar_side == ToolbarSide::Right) {
      content_size.x -= thickness;
    }

    ImGui::BeginChild("Content", content_size, false);
    _layout.render();
    ImGui::EndChild();

    // --- RIGHT ---
    if (_toolbar_side == ToolbarSide::Right) {
      ImGui::SameLine();
      ImGui::BeginChild("Toolbar", ImVec2(thickness, avail.y), false);
      _renderToolbar();
      ImGui::EndChild();
    }

    // --- BOTTOM ---
    if (_toolbar_side == ToolbarSide::Bottom) {
      ImGui::BeginChild("Toolbar", ImVec2(avail.x, thickness), false);
      _renderToolbar();
      ImGui::EndChild();
    }
  }
  ImGui::End();
}
