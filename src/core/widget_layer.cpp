#include "widget_layer.hPP"


static uint64_t WIDGET_UNIQUE_ID = 1;


WidgetLayer::WidgetLayer(const char* layer_name, float transparency, bool* p_open, ImGuiWindowFlags flags)
: _visible(true)
, _layer_name(layer_name)
, _layer_transparency(transparency)
, _p_open(p_open)
, _flags(flags)
, _layout(layer_name) {
  _layout = WidgetContainer(layer_name);
  _layout.setLayout(LayoutType::Grid);  // default layout
  _layout.setGridSize(2, 2);
}


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

  ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
  
  if (ImGui::Begin(_layer_name, _p_open, _flags)) {
    _layout.render();
    ImGui::End();
  }
}
