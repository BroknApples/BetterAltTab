#ifndef GUI_HPP
#define GUI_HPP


#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <cstdint>

#include "imgui.h"
#include "widget_types.hpp"
#include "widget_container.hpp"

class WidgetLayer {
  private:
    WidgetContainer _layout;
    bool _visible;
    const char* _layer_name;
    float _layer_transparency;
    bool* _p_open;
    ImGuiWindowFlags _flags;

  public:
    /**
     * @brief Actual constructor
     * @param layer_name: Name of the widget layer.
     * @param layer_transparency: Transparency of the layer
     * @param p_open: X box to close the window
     * @param flags: ImGui window flags
     */
     WidgetLayer(const char* layer_name, float transparency = 1.0f, bool* p_open = nullptr, ImGuiWindowFlags flags = 0);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the text
     */
    void addText(const std::string& name, const std::string& label);


    /**
     * @brief Adds a button widget
     * @param name: Name of the ImGui element
     * @param label: Label of the button
     * @param initial_state: Intial state of the button
     * @param callback: Function called on each frame.
     */
    void addButton(const std::string& name, const std::string& label, const bool initial_state, std::function<void(bool)> callback = nullptr);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the slider
     * @param min: Minimum percentage of the slider
     * @param max: Maximium percentage of the slider
     * @param value: Starting value of the slider
     * @param callback: Function called on each frame.
     */
    void addSlider(const std::string& name, const std::string& label, float min, float max, float value, std::function<void(float)> callback = nullptr);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the checkbox
     * @param checked: Initial check state
     * @param callback: Function called on each frame.
     */
    void addCheckbox(const std::string& name, const std::string& label, bool checked, std::function<void(bool)> callback = nullptr);


    /**
     * @brief Renders all widgets on this layer
     */
    void render();


    /**
     * @brief Toggles visiblity of the layer
     * @param visible: Should the layer be visible or invisible?
     */
    void toggleVisibility(bool visible) {
      _visible = visible;
    }


    /**
     * @brief Apply a new layout to the layer
     * @param type: Layout type
     */
    void setLayout(LayoutType type) {
      _layout.setLayout(type);
    }


    /**
     * @brief Set the new sorting method of a layer
     * @param mode: New sorting mode
     */
    void setSortMode(SortMode mode) {
      _layout.setSortMode(mode);
    }


    /**
     * @brief Set a new grid size for a grid-style layout
     * @param r: Rows
     * @param c: Columns
     */
    void setGridSize(int r, int c) {
      _layout.setGridSize(r, c);
    }
};


#endif // GUI_HPP
