#ifndef WIDGET_TYPES_HPP
#define WIDGET_TYPES_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <vector>
#include <string>
#include <variant>
#include <functional>

#include "imgui.h"


struct TextData;
struct ButtonData;
struct SliderData;
struct CheckboxData;

/** Polymorphism variant */
using WidgetVariant = std::variant<
  TextData,
  ButtonData,
  SliderData,
  CheckboxData
>;


/**
 * @brief Base class of all widget data structs
 */
struct WidgetData {
  /**
   * @brief DEFAULT CONSTRUCTOR
   */
  WidgetData() = default;

  /**
   * @brief Constructs a new data object.
   */
  WidgetData(const std::string& name)
  : name(name) {}

  std::string name;
};


/**
 * @brief Data for ImGui::Text widgets
 */
struct TextData : public WidgetData {
  /**
   * @brief DEFAULT CONSTRUCTOR
   */
  TextData() = default;

  /**
   * @brief Constructs a new text data object.
   */
  TextData(const std::string& name, const std::string& label)
  : WidgetData(name)
  , label(label) {}

  std::string label;
};


/**
 * @brief Data for ImGui::Button widgets
 */
struct ButtonData : public WidgetData {
  /**
   * @brief DEFAULT CONSTRUCTOR
   */
  ButtonData() = default;
  
  /**
   * @brief Constructs a new button data object.
   */
  ButtonData(const std::string& name, const std::string& label, const bool clicked, std::function<void(bool)> callback = nullptr)
  : WidgetData(name)
  , label(label)
  , clicked(clicked)
  , callback(callback) {}

  std::string label;
  bool clicked = false;
  std::function<void(bool)> callback;
};


/**
 * @brief Data for ImGui::Slider widgets
 */
struct SliderData : public WidgetData {
  /**
   * @brief DEFAULT CONSTRUCTOR
   */
  SliderData() = default;
  
  /**
   * @brief Constructs a new slider data object.
   */
  SliderData(const std::string& name, const std::string& label, const float value, float minimum, const float maximum, std::function<void(float)> callback = nullptr)
  : WidgetData(name)
  , label(label)
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
struct CheckboxData : public WidgetData {
  /**
   * @brief DEFAULT CONSTRUCTOR
   */
  CheckboxData() = default;
  
  /**
   * @brief Constructs a new checkbox data object.
   */
  CheckboxData(const std::string& name, const std::string& label, const bool checked, std::function<void(bool)> callback = nullptr)
  : WidgetData(name)
  , label(label)
  , checked(checked)
  , callback(callback) {}

  std::string label;
  bool checked;
  std::function<void(bool)> callback;
};


// etc...


#endif // WIDGET_TYPES_HPP
