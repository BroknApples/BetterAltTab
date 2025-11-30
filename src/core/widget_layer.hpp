#ifndef GUI_HPP
#define GUI_HPP


#include <vector>
#include <string>
#include <variant>
#include <algorithm>

#include "imgui.h"
#include "widget_types.hpp"


class WidgetLayer {
  private:
    std::vector<ImGuiWidget> _widgets;
    bool _visible;
    const char* _layer_name;
    float _layer_transparency;
    bool* _p_open;
    ImGuiWindowFlags _flags;


  public:
    /**
     * @brief Default Constructor.
     */
    WidgetLayer() = default;

    /**
     * @brief Actual constructor
     * @param layer_name: Name of the widget layer.
     * @param layer_transparency: Transparency of the layer
     * @param p_open: X box to close the window
     * @param flags: ImGui window flags
     */
    WidgetLayer(const char* layer_name, const float layer_transparency = 1.0f, bool* p_open = (bool*)0, const ImGuiWindowFlags flags = 0);


    /**
     * @brief Default Destructor.
     */
    ~WidgetLayer();


    /**
     * @brief Adds a Slider widget
     * @param label: Label of the text
     */
    void addText(const std::string& label);


    /**
     * @brief Adds a button widget
     * @param label: Label of the button
     * @param callback: Function called on each frame.
     */
    void addButton(const std::string& label, std::function<void(bool)> callback = nullptr);


    /**
     * @brief Adds a Slider widget
     * @param label: Label of the slider
     * @param min: Minimum percentage of the slider
     * @param max: Maximium percentage of the slider
     * @param value: Starting value of the slider
     * @param callback: Function called on each frame.
     */
    void addSlider(const std::string& label, float min, float max, float value, std::function<void(float)> callback = nullptr);


    /**
     * @brief Adds a Slider widget
     * @param label: Label of the checkbox
     * @param checked: Initial check state
     * @param callback: Function called on each frame.
     */
    void addCheckbox(const std::string& label, bool checked, std::function<void(bool)> callback = nullptr);


    /**
     * @brief Renders all widgets on this layer
     */
    void render();


    /**
     * @brief Toggles visiblity of the layer
     * @param visible: Should the layer be visible or invisible?
     */
    void toggleVisibility(bool visible);
};


#endif // GUI_HPP
