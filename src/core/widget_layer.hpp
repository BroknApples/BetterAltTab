#ifndef GUI_HPP
#define GUI_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <cstdint>

#include "imgui.h"

#include "config.hpp"
#include "widget_types.hpp"
#include "widget_container.hpp"


/**
 * @brief Enum for the side a toolbar should be located at.
 */
enum ToolbarSide {
  Top,
  Bottom,
  Left,
  Right,
  None
};


/**
 * @brief Class which defines a window of widgets grouped together
 */
class WidgetLayer {
  private:
    ToolbarSide _toolbar_side; // Defaults to the top
    WidgetContainer _layout;
    bool _visible;
    const char* _layer_name;
    float _layer_transparency;
    bool* _p_open;
    ImGuiWindowFlags _window_flags;

    // -------------- Sizing --------------
    ImVec2 _min_size;
    ImVec2 _max_size;
    ImVec2 _curr_size;
    ImVec2 _actual_size; // Only to be used internally
    bool _autofit; // TODO: Implement this next


    /**
     * @brief Updates the actual size of the widget
     */
    void _updateActualSize();


    /**
     * @brief Renders the actual toolbar
     */
    void _renderToolbar();

  public:
    /**
     * @brief Actual constructor
     * @param layer_name: Name of the widget layer.
     * @param layer_transparency: Transparency of the layer
     * @param p_open: X box to close the window
     * @param window_flags: ImGui window flags
     */
     WidgetLayer(const char* layer_name, float transparency = 1.0f, bool* p_open = nullptr, ImGuiWindowFlags window_flags = 0);


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


    /**
     * @brief Sets the size of each cell to be rendered
     * @param w: Width
     * @param h: Height
     */
    void setCellSize(const float w, const float h) {
      _curr_size.x = std::clamp(w, 0.0f, Config::monitor_size.x);
      _curr_size.y = std::clamp(h, 0.0f, Config::monitor_size.y);
      _updateActualSize();
    }

    /**
     * @brief Sets the size of each cell to be rendered
     * @param w: Width
     * @param h: Height
     */
    void setMinCellSize(const float w, const float h) {
      _min_size.x = std::clamp(w, 0.0f, Config::monitor_size.x);
      _min_size.y  = std::clamp(h, 0.0f, Config::monitor_size.y);
    }

    /**
     * @brief Sets the size of each cell to be rendered
     * @param w: Width
     * @param h: Height
     */
    void setMaxCellSize(const float w, const float h) {
      _max_size.x = std::clamp(w, 0.0f, Config::monitor_size.x);
      _max_size.y  = std::clamp(h, 0.0f, Config::monitor_size.y);
    }


    /**
     * @brief Dynamically sets the size of each cell to match the largest widget.
     * @param value: Value to set
     */
    void setAutofit(const bool value) {
      _autofit = value;
    }


    /**
     * @brief Toggles the window to be resizeable
     * @param value: Value to set
     */
    void toggleResizable(const bool value) {
      if (value) {
        // allow resizing → remove the NoResize flag
        _window_flags &= ~ImGuiWindowFlags_NoResize;
      } else {
          // disable resizing → add the NoResize flag
          _window_flags |= ImGuiWindowFlags_NoResize;
      }
    }


    /**
     * @brief Is the current window resizable?
     * @returns bool: True/False value
     */
    const bool isResizable() const {
      return (_window_flags & ImGuiWindowFlags_NoResize) == 0;
    }


    /**
     * @brief Sets a new side for the toolbar
     * @param side: New side for the toolbar
     */
    void setToolbarSide(ToolbarSide side) {
      _toolbar_side = side;
    }
};


#endif // GUI_HPP
