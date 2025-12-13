#include "widget_container.hpp"


// Static member definition
std::vector<WidgetContainer*> WidgetContainer::_registry;


// --------------------- Widget rendering ---------------------

void ImGuiWidget::render() {
  std::visit([](auto& widget){
    using T = std::decay_t<decltype(widget)>;

    if constexpr (std::is_same_v<T, TextData>) {
      ImGui::Text("%s", widget.label.c_str());
    }
    else if constexpr (std::is_same_v<T, ButtonData>) {
      if (ImGui::Button(widget.label.c_str())) {
        widget.clicked = !widget.clicked;
        if (widget.callback) widget.callback(widget.clicked);
      }
    }
    else if constexpr (std::is_same_v<T, SliderData>) {
      if (ImGui::SliderFloat(widget.label.c_str(),
                             &widget.value,
                             widget.minimum,
                             widget.maximum)) {
        if (widget.callback) widget.callback(widget.value);
      }
    }
    else if constexpr (std::is_same_v<T, CheckboxData>) {
      if (ImGui::Checkbox(widget.label.c_str(), &widget.checked)) {
        if (widget.callback) widget.callback(widget.checked);
      }
    }
  }, data);
}


WidgetContainer::WidgetContainer(const std::string& id)
: _id(id)
, _layout(LayoutType::Grid)
, _sort_mode(SortMode::None)
, _rows(1)
, _cols(1)
, _needs_sorting(false) {
  _registry.push_back(this);
}


WidgetContainer::~WidgetContainer() {
  // remove from registry
  auto it = std::find(_registry.begin(), _registry.end(), this);
  if (it != _registry.end()) _registry.erase(it);
}


void WidgetContainer::render() {
  if (_needs_sorting) {
    _applySorting();
    _needs_sorting = false;
  }

  switch (_layout) {
    case LayoutType::Grid:               _renderGrid(); break;
    case LayoutType::VerticalList:       _renderVertical(); break;
    case LayoutType::HorizontalList:     _renderHorizontal(); break;
    case LayoutType::RectStackVertical:  _renderRectStackVertical(); break;
    default:                             _renderGrid(); break; // Default layout is a grid
  }
}


void WidgetContainer::_applySorting() {
  if (_sort_mode == SortMode::Alphabetical) {
    std::stable_sort(_items.begin(), _items.end(),
      [](auto& a, auto& b){
        const std::string* la = nullptr;
        const std::string* lb = nullptr;

        std::visit([&](auto& w){ la = &w.name; }, a.data);
        std::visit([&](auto& w){ lb = &w.name; }, b.data);

        return *la < *lb;
      }
    );
  }
}


void WidgetContainer::_renderGrid() {
  // prevent invalid table
  if (_cols <= 0) return;

  // BeginTable failed, skip EndTable
  if (!ImGui::BeginTable(_id.c_str(), _cols)) return;

  int index = 0;
  for (int r = 0; r < _rows; r++) {
    for (int c = 0; c < _cols; c++) {
      _renderCell(index++);
    }
  }

  ImGui::EndTable();
}


// --------------------- Vertical list ---------------------

void WidgetContainer::_renderVertical() {
  for (int i = 0; i <= static_cast<int>(_items.size()); ++i) {
    // we render drop target before each item (and after last)
    _renderDropTarget(i);

    if (i == static_cast<int>(_items.size())) break;

    ImGuiWidget& w = _items[i];
    ImGui::PushID(w.id);

    //renderDragSource(w);

    // content box for visual separation
    ImGui::BeginChild(("item_" + std::to_string(w.id)).c_str(), ImVec2(-1, 40), true);
    _renderDragSource(w);
    w.render();
    ImGui::EndChild();

    ImGui::PopID();
  }
}

// --------------------- Horizontal list ---------------------

void WidgetContainer::_renderHorizontal() {
  for (int i = 0; i <= static_cast<int>(_items.size()); ++i) {
    _renderDropTarget(i);

    if (i == static_cast<int>(_items.size())) break;

    ImGuiWidget& w = _items[i];
    ImGui::PushID(w.id);

    //renderDragSource(w);

    ImGui::BeginChild(("item_" + std::to_string(w.id)).c_str(), ImVec2(120, 60), true);
    _renderDragSource(w);
    w.render();
    ImGui::EndChild();

    ImGui::PopID();

    ImGui::SameLine();
  }
  ImGui::NewLine(); // ensure next widgets are below the horizontal list
}


// --------------------- Vertical card list ---------------------

void WidgetContainer::_renderRectStackVertical() {
  const ImVec2 CARD_SIZE = ImVec2(-1, 100); // wide → fixed height

  for (int i = 0; i <= (int)_items.size(); ++i) {
    _renderDropTarget(i);

    if (i == _items.size()) break;

    ImGuiWidget& w = _items[i];
    ImGui::PushID(w.id);

    //renderDragSource(w);

    // A fixed-size box for the item
    ImGui::BeginChild(
      ("card_" + std::to_string(w.id)).c_str(),
      CARD_SIZE,
      true,                         // bordered
      ImGuiWindowFlags_NoScrollbar  // keeps it clean
    );

    _renderDragSource(w);
    w.render();

    ImGui::EndChild();
    ImGui::Spacing();

    ImGui::PopID();
  }
}


void WidgetContainer::_renderCell(int index) {
  ImGui::TableNextColumn();

  // Show drop target between cells (so drop into empty cells works)
  _renderDropTarget(index);

  if (index >= static_cast<int>(_items.size())) {
    // empty cell
    return;
  }

  ImGuiWidget& w = _items[index];
  ImGui::PushID(w.id);

  //renderDragSource(w);

  // Render the widget inside a child so it has bounds and background
  ImGui::BeginChild(("cell_" + std::to_string(w.id)).c_str(), ImVec2(-1, 60), false);
  _renderDragSource(w);
  w.render();
  ImGui::EndChild();

  ImGui::PopID();
}


void WidgetContainer::_renderDragSource(ImGuiWidget& item) {
  if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
    ImGui::SetDragDropPayload("LAYOUT_ITEM", &item.id, sizeof(int));
    ImGui::Text("Dragging widget %d", item.id);
    ImGui::EndDragDropSource();
  }
}


void WidgetContainer::_renderDropTarget(int index) {
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LAYOUT_ITEM")) {
      int draggedID = *(int*)payload->Data;
      _moveItemBetweenLayouts(draggedID, *this, index);
    }
    ImGui::EndDragDropTarget();
  }
}


void WidgetContainer::_moveItemBetweenLayouts(int item_id, WidgetContainer& target, int target_index) {
  WidgetContainer* source = nullptr;
  ImGuiWidget moved;

  // Find which container owns the item
  for (auto* c : _registry) {
    for (int i = 0; i < c->_items.size(); i++) {
      if (c->_items[i].id == item_id) {
        source = c;
        moved = std::move(c->_items[i]);
        c->_items.erase(c->_items.begin() + i);
        goto FOUND;
      }
    }
  }
FOUND:

  // Source must be valid
  if (!source) return;

  // Ensure target index in range
  if (target_index > target._items.size()) {
    target_index = target._items.size();
  }

  target.addItem(std::move(moved), target_index);
}
