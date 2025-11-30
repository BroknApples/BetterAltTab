#ifndef WIDGET_TYPES_HPP
#define WIDGET_TYPES_HPP


#include <vector>
#include <string>
#include <variant>
#include <functional>

#include "imgui.h"


/**
 * @brief Data for ImGui::Text widgets
 */
struct TextData {
  /**
   * @brief Constructs a new text data object.
   */
  TextData(const std::string& label)
  : label(label) {}

  std::string label;
  bool checked;
};


/**
 * @brief Data for ImGui::Button widgets
 */
struct ButtonData {
  /**
   * @brief Constructs a new button data object.
   */
  ButtonData(const std::string& label, const bool clicked, std::function<void(bool)> callback = nullptr)
  : label(label)
  , clicked(clicked)
  , callback(callback) {}

  std::string label;
  bool clicked = false;
  std::function<void(bool)> callback;
};


/**
 * @brief Data for ImGui::Slider widgets
 */
struct SliderData {
  /**
   * @brief Constructs a new slider data object.
   */
  SliderData(const std::string& label, const float value, float minimum, const float maximum, std::function<void(float)> callback = nullptr)
  : label(label)
  , value(value)
  , minimum(minimum)
  , maximum(maximum)
  , callback(callback) {}

  std::string label;
  float value;
  float minimum;
  float maximum;
  std::function<void(float)> callback;
};


/**
 * @brief Data for ImGui::Checkbox widgets
 */
struct CheckboxData {
  /**
   * @brief Constructs a new checkbox data object.
   */
  CheckboxData(const std::string& label, const bool checked, std::function<void(bool)> callback = nullptr)
  : label(label)
  , checked(checked)
  , callback(callback) {}

  std::string label;
  bool checked;
  std::function<void(bool)> callback;
};


// etc...


using ImGuiWidget = std::variant<TextData, ButtonData, SliderData, CheckboxData>;


#endif // WIDGET_TYPES_HPP
