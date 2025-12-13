#ifndef WIDGET_CONTAINER_HPP
#define WIDGET_CONTAINER_HPP


#include <vector>
#include <string>
#include <algorithm>
#include <variant>
#include <cstdint>
#include <functional>

#include "imgui.h"

#include "widget_types.hpp"
#include "config.hpp"

/**
 * @brief Struct which defines a widget's data.
 */
struct ImGuiWidget {
  ImGuiWidget() = default;
  ImGuiWidget(const uint64_t i, WidgetVariant dat) : id(i), data(dat) {}

  uint64_t id;
  WidgetVariant data;

  /**
   * @brief Render an ImGui Widget
   */
  void render();

  /**
   * @brief Applies a new update function to the stored data.
   * @param uf: New update function
   */
  void setUpdateFunction(const std::function<void()> uf) {
    std::visit([&uf](auto& w){
      w.update = uf;
    }, data);
  }
};

/**
 * @brief Enum class which defines the layout style of a LayoutContainer
 */
enum class LayoutType {
  Grid,
  VerticalList,
  HorizontalList,
  RectStackVertical
};


/**
 * @brief Enum class which defines the sorting method of the widget container's layout
 */
enum class SortMode {
  None,
  Alphabetical,
  ByType,
  ByCustomKey
};


/**
 * @brief Class which defines a group of widgets to be rendered
 */
class WidgetContainer {
  private:
    std::string _id;
    LayoutType _layout;
    SortMode _sort_mode;
    int _rows;
    int _cols;
    bool _needs_sorting;

    std::vector<ImGuiWidget> _items;

    
    /**
     * @brief Sort a layout
     */
    void _applySorting();


    /**
     * @brief Render a grid-style layout
     */
    void _renderGrid();


    /**
     * @brief Render a Vertical layout
     */
    void _renderVertical();


    /**
     * @brief Render a horizontal layout
     */
    void _renderHorizontal();


    /**
     * @brief Render a stack of vertical rects
     */
    void _renderRectStackVertical();


    /**
     * @brief Render a single cell
     * @param index: Index to render
     */
    void _renderCell(int index);


    /**
     * @brief Render a dragging object
     * @param item: Item to drag
     */
    void _renderDragSource(ImGuiWidget& item);


    /**
     * @brief Render a dragging object to its new location
     * @param index: Index of the item
     */
    void _renderDropTarget(int index);


    // --- static registry of all containers ---
    static std::vector<WidgetContainer*> _registry;


    /**
     * @brief Finds source container
     * STATIC FUNCTION
     * 
     * @param item_id: ID of the item to move
     * @param target: Target container to move item to
     * @param tatget_index: index of the position to move the item to in the target container
     */
    static void _moveItemBetweenLayouts(int item_id, WidgetContainer& target, int targetIndex);


  public:
    /**
     * @brief Default Constructor.
     */
    WidgetContainer() = default;

    /**
     * CONSTRUCTOR
     * @brief Create a new layer
     * @param id: ID name of the new layer
     */
    WidgetContainer(const std::string& id);


    /**
     * DESTRUCTOR
     * @brief Remove a layer from the registry
     */
    ~WidgetContainer();


    /**
     * @brief Sort layer then render its layout
     */
    void render();


    /**
     * @brief Apply a new layout to the layer
     * @param type: Layout type
     */
    void setLayout(LayoutType type) {
      _layout = type;
    }


    /**
     * @brief Set the new sorting method of a layer
     * @param mode: New sorting mode
     */
    void setSortMode(SortMode mode) {
      if (_sort_mode != mode) {
        _sort_mode = mode;
        _needs_sorting = true;
      }
    }


    /**
     * @brief Set a new grid size for a grid-style layout
     * @param r: Rows
     * @param c: Columns
     */
    void setGridSize(int r, int c) {
      _rows = r;
      _cols = c;
    }


    /**
     * @brief Add a new item to the layout
     * @param item: Item to add
     * @param position: Position to add the item at | DEFAULT = END
     */
    ImGuiWidget& addItem(ImGuiWidget item, const int position = -1) {
      if (position > -1 && position < _items.size()) {
        _items.insert(_items.begin() + position, std::move(item));
        return _items[position];
      }
      else {
        _items.push_back(std::move(item));
        return _items.back();
      }
      
      _needs_sorting = true;
    }

    /**
     * @brief Returns a modifiable list of items
     * @returns std::vector<ImGuiWidget>&: Widget items
     */
    std::vector<ImGuiWidget>& getItems() {
      return _items;
    }
};


#endif // WIDGET_CONTAINER_HPP
