#ifndef GUI_HPP
#define GUI_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <algorithm>
#include <memory>
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
    // -------------- ImGui window vars --------------
    const char* _layer_name;
    float _layer_transparency;
    bool* _p_open;
    ImGuiWindowFlags _window_flags;

    // -------------- Widgets --------------
    ToolbarSide _toolbar_side; // Defaults to the top
    std::shared_ptr<WidgetContainer> _toolbar;
    WidgetContainer _layout;
    std::function<void()> _render_pipeline; // Function used within the { ImGui::Begin() -> ImGui::End() } block.

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


    /**
     * @brief Function for rendering which INCLUDES a toolbar rendering pipeline
     */
    void _renderInternalWithToolbar();

    /**
     * @brief Function for rendering which DOES NOT INCLUDE a toolbar rendering pipeline
     */
    void _renderInternalNoToolbar();


  public:
    /**
     * @brief Actual constructor
     * @param layer_name: Name of the widget layer.
     * @param layer_transparency: Transparency of the layer
     * @param p_open: X box to close the window
     * @param window_flags: ImGui window flags
     * @param toolbar: Should there be a toolbar on this WidgetLayer?
     */
    WidgetLayer(const char* layer_name, float transparency = 1.0f, bool* p_open = nullptr, ImGuiWindowFlags window_flags = 0, const bool toolbar = false);


    /**
     * @brief Adds a widget to the layout
     * @param widget: Widget to add
     * @param position: Position in the layout to add the widget
     * @param toolbar: Should it be added to the toolbar instead of the layer?
     */
    ImGuiWidget& addWidget(const WidgetVariant& widget, const int position = -1, const bool toolbar = false);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the text
     * @param position: Position in the layout to add the widget
     * @param toolbar: Should it be added to the toolbar instead of the layer?
     */
    ImGuiWidget& addText(const std::string& name, const std::string& label, const int position = -1, const bool toolbar = false);


    /**
     * @brief Adds a button widget
     * @param name: Name of the ImGui element
     * @param label: Label of the button
     * @param initial_state: Intial state of the button
     * @param callback: Function called on each frame.
     * @param position: Position in the layout to add the widget
     * @param toolbar: Should it be added to the toolbar instead of the layer?
     */
    ImGuiWidget& addButton(const std::string& name, const std::string& label, const bool initial_state, const std::function<void(bool)> callback = nullptr, const int position = -1, const bool toolbar = false);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the slider
     * @param min: Minimum percentage of the slider
     * @param max: Maximium percentage of the slider
     * @param value: Starting value of the slider
     * @param callback: Function called on each frame.
     * @param position: Position in the layout to add the widget
     * @param toolbar: Should it be added to the toolbar instead of the layer?
     */
    ImGuiWidget& addSlider(const std::string& name, const std::string& label, const float min, const float max, const float value, const std::function<void(float)> callback = nullptr, const int position = -1, const bool toolbar = false);


    /**
     * @brief Adds a Slider widget
     * @param name: Name of the ImGui element
     * @param label: Label of the checkbox
     * @param checked: Initial check state
     * @param callback: Function called on each frame.
     * @param position: Position in the layout to add the widget
     * @param toolbar: Should it be added to the toolbar instead of the layer?
     */
    ImGuiWidget& addCheckbox(const std::string& name, const std::string& label, const bool checked, const std::function<void(bool)> callback = nullptr, const int position = -1, const bool toolbar = false);


    /**
     * @brief Renders all widgets on this layer
     */
    void render();


    // --------------- Inline functions ---------------

    /**
     * @brief Gets the list of widgets in this layer
     * @returns std::vector<ImGuiWidget>&: List of wiget
     */
    std::vector<ImGuiWidget>& getWidgets() {
      return _layout.getItems();
    }


    /**
     * @brief Gets the list of widgets in this layer
     * @returns std::vector<ImGuiWidget>&: List of wiget
     */
    std::vector<ImGuiWidget>& getToolbarWidgets() {
      return _toolbar->getItems();
    }

    /**
     * @brief Changes the visibiltiy of the toolbar when rendering
     * @param value: New value
     */
    void setToolbarVisibility(const bool visible) {
      if (visible) {
        if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
        _render_pipeline = [this]() { _renderInternalWithToolbar(); };
      } else {
        _render_pipeline = [this]() { _renderInternalNoToolbar(); };
      }
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

      if (_toolbar == nullptr) return;
      switch (side) {
        case ToolbarSide::Top:
        case ToolbarSide::Bottom:
          _toolbar->setGridSize(10, 1);
          break;
        case ToolbarSide::Left:
        case ToolbarSide::Right:
          _toolbar->setGridSize(1, 10);
          break;
      }
    }
};


#endif // GUI_HPP
