#include "widget_layer.hpp"


WidgetLayer::WidgetLayer(const char* layer_name, const float layer_transparency, bool* p_open, const ImGuiWindowFlags flags)
: _visible(true)
, _layer_name(layer_name)
, _layer_transparency(layer_transparency)
, _p_open(p_open)
, _flags(flags) {
  _layer_transparency = std::clamp(_layer_transparency, 0.0f, 1.0f);
}


WidgetLayer::~WidgetLayer() {}


// ------------------------ Widget functions ------------------------
void WidgetLayer::addText(const std::string& label) {
  _widgets.push_back(TextData{label});
}

void WidgetLayer::addButton(const std::string& label, std::function<void(bool)> callback) {
  _widgets.push_back(ButtonData{label, false, callback});
}


void WidgetLayer::addSlider(const std::string& label, float min, float max, float value, std::function<void(float)> callback) {
  _widgets.push_back(SliderData{label, value, min, max, callback});
}


void WidgetLayer::addCheckbox(const std::string& label, bool checked, std::function<void(bool)> callback) {
  _widgets.push_back(CheckboxData{label, checked, callback});
}


// ----------------------- Render/Helper functions -----------------------
void WidgetLayer::render() {
  if (!_visible) return;

  if (_layer_transparency < 1.0f) {
    ImGui::SetNextWindowBgAlpha(_layer_transparency);
  }

  ImGui::Begin(_layer_name, _p_open, _flags);

  for (auto& w : _widgets) {
    std::visit([](auto& widget){
      using T = std::decay_t<decltype(widget)>;
      if constexpr (std::is_same_v<T, TextData>) {
        ImGui::Text(widget.label.c_str());
      }
      else if constexpr (std::is_same_v<T, ButtonData>) {
        if (ImGui::Button(widget.label.c_str()) && widget.callback) {
          widget.clicked = !widget.clicked;
          widget.callback(widget.clicked);
        }
      }
      else if constexpr (std::is_same_v<T, SliderData>) {
        if (ImGui::SliderFloat(widget.label.c_str(), &widget.value, widget.minimum, widget.maximum) && widget.callback) {
          widget.callback(widget.value);
        }
      }
      else if constexpr (std::is_same_v<T, CheckboxData>) {
        if (ImGui::Checkbox(widget.label.c_str(), &widget.checked) && widget.callback) {
          widget.callback(widget.checked);
        }
      }
    }, w);
  }

  ImGui::End();
}


void WidgetLayer::toggleVisibility(bool visible) {
  _visible = visible;
}