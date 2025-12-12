#include "im_window.hPP"


static uint64_t TOOLBAR_WIDGET_UNIQUE_ID = 1;
static uint64_t WIDGET_UNIQUE_ID = 1;


// ------------------- Private functions -------------------

void ImWindow::_updateActualSize() {
  if (_autofit) {
    // TODO: HERE
  }
  else {
    float determined_width = std::clamp(_curr_size.x, _min_size.x, std::max(_min_size.x, _max_size.x));
    float determined_height = std::clamp(_curr_size.y, _min_size.y, std::max(_min_size.y, _max_size.y));
    _actual_size = ImVec2(determined_width, determined_height);
  }
}


void ImWindow::_renderToolbar() {
  if (_toolbar_side == Top || _toolbar_side == Bottom) {
    // Horizontal toolbar
    _toolbar->render();
    // if (ImGui::Button("A")) {}
    // ImGui::SameLine();
    // if (ImGui::Button("B")) {}
    // ImGui::SameLine();
    // if (ImGui::Button("C")) {}
  }
  else {
    // Vertical toolbar
    _toolbar->render();
    // if (ImGui::Button("A")) {}
    // if (ImGui::Button("B")) {}
    // if (ImGui::Button("C")) {}
  }
}


void ImWindow::_renderInternalWithToolbar() {
  // Call update functions
  for (auto& item : _layout.getItems()) {
    std::visit([](auto& w){
      if (w.update)w.update();
    }, item.data);
  }
  for (auto& item : _toolbar->getItems()) {
    std::visit([](auto& w){
      if (w.update) w.update();
    }, item.data);
  }

  const float thickness = 30.0f;
  ImVec2 avail = ImGui::GetContentRegionAvail();

  // --- TOP ---
  if (_toolbar_side == ToolbarSide::Top) {
    ImGui::BeginChild("Toolbar", ImVec2(avail.x, thickness), false);
    _renderToolbar();
    ImGui::EndChild();
  }

  // --- LEFT ---
  if (_toolbar_side == ToolbarSide::Left) {
    ImGui::BeginChild("Toolbar", ImVec2(thickness, avail.y), false);
    _renderToolbar();
    ImGui::EndChild();

    ImGui::SameLine();
  }

  // --- CONTENT ---
  ImVec2 content_size = avail;

  if (_toolbar_side == ToolbarSide::Top ||
      _toolbar_side == ToolbarSide::Bottom) {
    content_size.y -= thickness;
  }
  else if (_toolbar_side == ToolbarSide::Left ||
          _toolbar_side == ToolbarSide::Right) {
    content_size.x -= thickness;
  }

  ImGui::BeginChild("Content", content_size, false);
  _layout.render();
  ImGui::EndChild();

  // --- RIGHT ---
  if (_toolbar_side == ToolbarSide::Right) {
    ImGui::SameLine();
    ImGui::BeginChild("Toolbar", ImVec2(thickness, avail.y), false);
    _renderToolbar();
    ImGui::EndChild();
  }

  // --- BOTTOM ---
  if (_toolbar_side == ToolbarSide::Bottom) {
    ImGui::BeginChild("Toolbar", ImVec2(avail.x, thickness), false);
    _renderToolbar();
    ImGui::EndChild();
  }
}


void ImWindow::_renderInternalNoToolbar() {
  // Call update functions
  for (auto& item : _layout.getItems()) {
    std::visit([](auto& w){
      if (w.update)w.update();
    }, item.data);
  }

  ImGui::BeginChild("Content");
  _layout.render();
  ImGui::EndChild();
}


// ------------------- Constructor -------------------

ImWindow::ImWindow(const char* layer_name, float transparency, bool* p_open, ImGuiWindowFlags window_flags, const bool toolbar)
: _toolbar_side(ToolbarSide::Top)
, _layout(layer_name)
, _window_name(layer_name)
, _layer_transparency(transparency)
, _p_open(p_open)
, _window_flags(window_flags)
, _min_size(0.0f, 0.0f)
, _max_size(Config::monitor_size.x, Config::monitor_size.y)
, _curr_size(400.0f, 400.0f)
, _actual_size(400.0f, 300.0f) {
  if (toolbar) {
    _toolbar = std::make_shared<WidgetContainer>("Toolbar");
    _toolbar->setLayout(LayoutType::Grid);  // The only layout for a toolbar
    _toolbar->setGridSize(1, 10);
    _render_pipeline = [this]() { _renderInternalWithToolbar(); };
  }
  else {
    _toolbar = nullptr;
    _render_pipeline = [this]() { _renderInternalNoToolbar(); };
  }

  _layout.setLayout(LayoutType::Grid);  // Default layout
  _layout.setGridSize(2, 2);
}


// ------------------- Public functions -------------------

ImGuiWidget& ImWindow::addWidget(const WidgetVariant& widget, const int position, const bool toolbar) {
  if (toolbar) {
    if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
    return _toolbar->addItem(ImGuiWidget{
      TOOLBAR_WIDGET_UNIQUE_ID++,
      widget
    }, position);
  }
  else {
    return _layout.addItem(ImGuiWidget{
      WIDGET_UNIQUE_ID++,
      widget
    }, position);
  }
}


ImGuiWidget& ImWindow::addText(const std::string& name, const std::string& label, const int position, const bool toolbar) {
  if (toolbar) {
    if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
     return _toolbar->addItem(ImGuiWidget{
      TOOLBAR_WIDGET_UNIQUE_ID++,
      TextData{ name, label }
    }, position);
  }
  else {
    return _layout.addItem(ImGuiWidget{
      WIDGET_UNIQUE_ID++,
      TextData{ name, label }
    }, position);
  }
}


ImGuiWidget& ImWindow::addButton(const std::string& name, const std::string& label, const bool initial_state, const std::function<void(bool)> callback, const int position, const bool toolbar) {
  if (toolbar) {
    if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
    return _toolbar->addItem(ImGuiWidget{
      TOOLBAR_WIDGET_UNIQUE_ID++,
      ButtonData{ name, label, initial_state, callback }
    }, position);
  }
  else {
    return _layout.addItem(ImGuiWidget{
      WIDGET_UNIQUE_ID++,
      ButtonData{ name, label, initial_state, callback }
    }, position);
  }
}


ImGuiWidget& ImWindow::addSlider(const std::string& name, const std::string& label, const float min, const float max, const float value, const std::function<void(float)> callback, const int position, const bool toolbar) {
  if (toolbar) {
    if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
    return _toolbar->addItem(ImGuiWidget{
      TOOLBAR_WIDGET_UNIQUE_ID++,
      SliderData{ name, label, value, min, max, callback }
    }, position);
  }
  else {
    return _layout.addItem(ImGuiWidget{
      WIDGET_UNIQUE_ID++,
      SliderData{ name, label, value, min, max, callback }
    }, position);
  }
}


ImGuiWidget& ImWindow::addCheckbox(const std::string& name, const std::string& label, const bool checked, const std::function<void(bool)> callback, const int position, const bool toolbar) {
  if (toolbar) {
    if (_toolbar == nullptr) _toolbar = std::make_shared<WidgetContainer>("Toolbar");
    return _toolbar->addItem(ImGuiWidget{
      TOOLBAR_WIDGET_UNIQUE_ID++,
      CheckboxData{ name, label, checked, callback }
    }, position);
  }
  else {
    return _layout.addItem(ImGuiWidget{
      WIDGET_UNIQUE_ID++,
      CheckboxData{ name, label, checked, callback }
    }, position);
  }
}


void ImWindow::render() {
  if (_layer_transparency < 1.0f) {
    ImGui::SetNextWindowBgAlpha(_layer_transparency);
  }
  
  // Window Settings
  //ImGui::SetNextWindowSize(_actual_size, ImGuiCond_Once);
  ImGui::SetNextWindowSize(_actual_size, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(_min_size, _max_size);

  // Render
  if (ImGui::Begin(_window_name, _p_open, _window_flags)) {
    _render_pipeline();
  }
  ImGui::End();
}
