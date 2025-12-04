#include "widget_layer.hPP"


static uint64_t WIDGET_UNIQUE_ID = 1;


// ------------------- Private functions -------------------
void WidgetLayer::_updateActualSize() {
  if (_autofit) {
    // TODO: HERE
  }
  else {
    float determined_width = std::clamp(_curr_width, _min_width, std::max(_min_width, _max_width));
    float determined_height = std::clamp(_curr_height, _min_height, std::max(_min_height, _max_height));
    _actual_size = ImVec2(determined_width, determined_height);
  }
}


// ------------------- Constructor -------------------
WidgetLayer::WidgetLayer(const char* layer_name, float transparency, bool* p_open, ImGuiWindowFlags window_flags)
: _visible(true)
, _layer_name(layer_name)
, _layer_transparency(transparency)
, _p_open(p_open)
, _window_flags(window_flags)
, _layout(layer_name)
, _min_width(0.0f)
, _min_height(0.0f)
, _max_width(Config::monitor_size.x)
, _max_height(Config::monitor_size.y)
, _curr_width(400.0f)
, _curr_height(300.0f)
, _actual_size({400.0f, 300.0f}) {
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

  ImGui::SetNextWindowSize(_actual_size, ImGuiCond_FirstUseEver);
  //ImGui::SetNextWindowSize(_actual_size, ImGuiCond_Always);
  
  if (ImGui::Begin(_layer_name, _p_open, _window_flags)) {
    _layout.render();
  }
  ImGui::End();
}
