#include "application.hpp"

// ---------------- DirectX variables ----------------

ID3D11Device*           Application::_pd3d_device = nullptr;
ID3D11DeviceContext*    Application::_pd3d_device_context = nullptr;
IDXGISwapChain*         Application::_p_swap_chain = nullptr;
ID3D11RenderTargetView* Application::_main_render_target_view = nullptr;


// ---------------- Tray variables ----------------

WNDCLASSEX     Application::_wc = {};
NOTIFYICONDATA Application::_nid = {};
HWND           Application::_hwnd = nullptr;
HICON          Application::_h_icon = LoadIcon(NULL, IDI_APPLICATION); // Default to a generic application icon.
HWINEVENTHOOK  Application::_hook = nullptr;



// ---------------- Settings Panel ----------------
bool  Application::_settings_panel_visible = false;


// ---------------- Misc variables ----------------

bool                    Application::_overlay_visible = false;
std::vector<WindowInfo> Application::_open_windows = {};
 FpsTimer               Application::_fps_timer;


// ---------------- DirectX functions ----------------

void Application::_createRenderTarget() {
  ID3D11Texture2D* p_back_buffer = nullptr;
  _p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer));
  _pd3d_device->CreateRenderTargetView(p_back_buffer, nullptr, &_main_render_target_view);
  p_back_buffer->Release();
}


void Application::_cleanupRenderTarget() {
  if (_main_render_target_view) {
    _main_render_target_view->Release();
    _main_render_target_view = nullptr;
  }
}


bool Application::_createDeviceD3D(HWND hwnd) {
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = _hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT flags = 0;
  D3D_FEATURE_LEVEL feature_level;
  const D3D_FEATURE_LEVEL levels[1] = { D3D_FEATURE_LEVEL_11_0 };

  if (FAILED(D3D11CreateDeviceAndSwapChain(
    nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
    levels, 1, D3D11_SDK_VERSION, &sd,
    &_p_swap_chain, &_pd3d_device, &feature_level,
    &_pd3d_device_context
  ))) {
    return false;
  }

  _createRenderTarget();
  return true;
}


void Application::_cleanupDeviceD3D() {
  _cleanupRenderTarget();
  if (_p_swap_chain) {
    _p_swap_chain->Release();
    _p_swap_chain = nullptr;
  }
  if (_pd3d_device_context) {
    _pd3d_device_context->Release();
    _pd3d_device_context = nullptr;
  }
  if (_pd3d_device) {
    _pd3d_device->Release();
    _pd3d_device = nullptr;
  }
}


// ----------------- Gui functions -----------------

void Application::_setupImGuiStyles() {
  ImGuiStyle& style = ImGui::GetStyle();

  // Window title bars
  style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);   // normal
  style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // active/focused
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);   // collapsed (semi-transparent)

  // Collapsible headers
  style.Colors[ImGuiCol_Header]         = ImVec4(0.2f, 0.2f, 0.2f, 0.5f); // normal header
  style.Colors[ImGuiCol_HeaderHovered]  = ImVec4(0.3f, 0.3f, 0.3f, 0.7f); // hovered
  style.Colors[ImGuiCol_HeaderActive]   = ImVec4(0.3f, 0.3f, 0.3f, 0.8f); // active

  // ...
  // ...
  // ...
}


// ----------------- Tray functions -----------------

void Application::_addTrayIcon() {
  _nid.cbSize = sizeof(NOTIFYICONDATA);
  _nid.hWnd = _hwnd;
  _nid.uID = 1;
  _nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  _nid.uCallbackMessage = WM_APP + 1;
  _nid.hIcon = _h_icon;
  strncpy(reinterpret_cast<char*>(_nid.szTip), _SYSTEM_TRAY_NAME, sizeof(_nid.szTip) - 1);
  _nid.szTip[sizeof(_nid.szTip) - 1] = '\0';

  Shell_NotifyIcon(NIM_ADD, &_nid);
}


void Application::_removeTrayIcon() {
  Shell_NotifyIcon(NIM_DELETE, &_nid);
}


void Application::_showTrayMenu() {
  POINT pt;
  GetCursorPos(&pt);

  HMENU h_menu = CreatePopupMenu();

  const TCHAR* show_overlay_text = _overlay_visible ? TEXT("* Hide Overlay") : TEXT("Show Overlay");
  AppendMenu(h_menu, MF_STRING, 1, show_overlay_text);
  
  const TCHAR* show_settings_text = _settings_panel_visible ? TEXT("* Hide Settings") : TEXT("Show Settings");
  AppendMenu(h_menu, MF_STRING, 2, show_settings_text);

  AppendMenu(h_menu, MF_SEPARATOR, 0, NULL);
  AppendMenu(h_menu, MF_STRING, 3, TEXT("Exit"));

  SetForegroundWindow(_hwnd); // required for menu to disappear correctly
  int cmd = TrackPopupMenu(h_menu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, _hwnd, NULL);
  DestroyMenu(h_menu);

  if (cmd == 1) {
   _toggleOverlayVisible();
  }
  else if (cmd == 2) {
    if (!_overlay_visible && !_settings_panel_visible) _toggleOverlayVisible();
    _settings_panel_visible = !_settings_panel_visible;
  }
  else if (cmd == 3) {
    PostQuitMessage(0);
  }
}


void Application::_toggleOverlayVisible() {
  _overlay_visible = !_overlay_visible;

  if (_overlay_visible) {
    _jumpstartUI();
    SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    ShowWindow(_hwnd, SW_SHOW);
  } else {
    SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    ShowWindow(_hwnd, SW_HIDE);
  }
}


void Application::_checkInputs() {
  // ---------------- Toggle overlay with INSERT ----------------
  if (GetAsyncKeyState(VK_INSERT) & 1) {
    _toggleOverlayVisible();
  }
}


LRESULT CALLBACK Application::_WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param)) return true;

  switch (msg) {
    case WM_APP + 1: {
      // tray callback
      switch (LOWORD(l_param)) {
        case WM_LBUTTONUP: {
          _toggleOverlayVisible();
          break;
        }
        case WM_RBUTTONUP: {
          _showTrayMenu();
          break;
        }
      }
      break;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
  }
  
  return DefWindowProc(hwnd, msg, w_param, l_param);
}


void CALLBACK Application::_WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG id_object,
  LONG id_child, DWORD event_thread, DWORD event_time) {
  // MUST be a window event
  if (id_object != OBJID_WINDOW) return; // Only window events are needed
  if (hwnd == _hwnd) return; // Ignore events for the "BetterAltTab.exe" program.

  // std::string title = getWindowTitle(hwnd);
  
  // auto p = [](const std::string type) {
  //   std::cout << "\n[" << type << "] Printing updated list: \n";
  //   for (const auto& info : _open_windows) {
  //     std::cout << info.title << "\n";
  //   }
  //   std::cout << std::endl;
  // };

  switch (event) {
    case EVENT_OBJECT_NAMECHANGE:
      // Change title
      updateWindowInfoListItem(_open_windows, hwnd);
      //p("NAMECHANGE");
      break;

    case EVENT_OBJECT_CREATE:
      // Add to list
      addWindowToVisibleAltTabList(_open_windows, hwnd);
      //p("CREATE");
      break;

    case EVENT_OBJECT_DESTROY:
      // Remove from list
      removeWindowFromWindowInfoList(_open_windows, hwnd);
      //p("DESTROY");
      break;

    // case EVENT_OBJECT_SHOW:
    //   // Add to list
    //   addWindowToVisibleAltTabList(_open_windows, hwnd);
    //   p("SHOW");
    //   break;

    // case EVENT_OBJECT_HIDE:
    //   // Remove from list
    //   removeWindowFromWindowInfoList(_open_windows, hwnd);
    //   p("HIDE");
    //   break;
  }
}


// --------------------------------------------------------
// --------------------------------------------------------
// ------------------- Public functions -------------------
// --------------------------------------------------------
// --------------------------------------------------------

bool Application::createApplication(HINSTANCE& h_instance) {
  // Config MUST be intialized
  if (!Config::initialized) return false;

  // Load Icon
  _h_icon = LoadIcon(h_instance, MAKEINTRESOURCE(IDI_BETTER_ALT_TAB_ICON));


  // Create handle
  _wc = {sizeof(WNDCLASSEX), CS_CLASSDC, _WndProc, 0, 0,
    h_instance,
    _h_icon, // big icon
    LoadCursor(NULL, IDC_ARROW),
    NULL, NULL,
    _PROGRAM_HANDLE_NAME,
    _h_icon // small icon
  };
  RegisterClassEx(&_wc);

  HRSRC res = FindResource(h_instance, MAKEINTRESOURCE(IDI_BETTER_ALT_TAB_ICON), RT_GROUP_ICON);
  if (res == NULL) {
    MessageBoxA(NULL, "ICON RESOURCE NOT FOUND!", "ERROR", MB_OK);
  }

  _hwnd = CreateWindowEx(
    WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
    _wc.lpszClassName, _T_WINDOW_NAME,
    WS_POPUP,
    0, 0, // Top-Left corner
    Config::monitor_size.x, Config::monitor_size.y,
    NULL, NULL, _wc.hInstance, NULL
  );


  // Windows event callback binding
  _hook = SetWinEventHook(
    EVENT_MIN, EVENT_MAX,   // receive all events (restricted in callback)
    NULL, 
    _WinEventProc,
    0, 0,                   // all processes/threads
    WINEVENT_OUTOFCONTEXT   // non-invasive, async
  );

  // Error when setting the hook
  if (!_hook) {
    std::cout << "Failed to set hook" << std::endl;
    return false;
  }

  // Gather all windows on startup
  _open_windows = getAllAltTabWindows();


  SetLayeredWindowAttributes(_hwnd, RGB(0,0,0), 0, LWA_COLORKEY);
  ShowWindow(_hwnd, SW_SHOW);

  if (!_createDeviceD3D(_hwnd)) {
    _cleanupDeviceD3D();
    UnregisterClass(_wc.lpszClassName, _wc.hInstance);
    return false;
  }

  // Add tray icon
  _addTrayIcon();

  // ---------------- ImGui setup ----------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  ImGui_ImplWin32_Init(_hwnd);
  ImGui_ImplDX11_Init(_pd3d_device, _pd3d_device_context);

  // Setup widget layers
  _setupImGuiStyles();
  return true;
}

StopwatchTimer stopwatch;

void Application::runApplication() {
  stopwatch.start();
  MSG msg = {};
  while (msg.message != WM_QUIT) {
    // ---------------- Toggle overlay with INSERT ----------------
    _checkInputs();
    
    if (_overlay_visible) {
      // (POLLING): Process all waiting messages, but don't block.
      if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) { 
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        continue;
      }
    }
    else {
      // (BLOCKING): Wait (block) until a message arrives.
      if (GetMessage(&msg, nullptr, 0U, 0U)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else {
        continue; // GetMessage returns 0 on WM_QUIT
      }
    }

    // ------------------------ Render ------------------------
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // FPS Tracker
    _fps_timer.update();
    //std::cout << std::fixed << std::setprecision(2) << "\r" << _fps_timer.getFps() << " fps" << std::flush;

    // Draw UI
    if (_overlay_visible) {
      if (_settings_panel_visible) {
        _renderSettingsUI();
      }
    }
    
    ImGui::Render();
    float clear[4] = {0,0,0,0};
    _pd3d_device_context->OMSetRenderTargets(1, &_main_render_target_view, nullptr);
    _pd3d_device_context->ClearRenderTargetView(_main_render_target_view, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    _p_swap_chain->Present(Config::vsync,0);
  }
}


void Application::destroyApplication() {
  // Do Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
  _cleanupDeviceD3D();
  _removeTrayIcon();
  UnhookWinEvent(_hook);
  DestroyWindow(_hwnd);
  UnregisterClass(_wc.lpszClassName, _wc.hInstance);
}




// -------------------------------- UI Rendering Helpers --------------------------------

/**
 * @brief Outputs right-aligned for an ImGui call
 * @param fmt formatted text
 * @param ... formatted text vars
 */
void ImGuiRightAlignedText(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buf[128];
  vsnprintf(buf, sizeof(buf), fmt, args);

  va_end(args);

  float avail = ImGui::GetContentRegionAvail().x;
  float text_w = ImGui::CalcTextSize(buf).x;

  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - text_w);
  ImGui::TextUnformatted(buf);
}


// -------------------------------- UI Rendering --------------------------------

void Application::_renderSettingsUI() {
  // Top-right corner placement
  ImVec2 pos(
    Config::monitor_size.x - (Config::monitor_size.x * Config::settings_panel_width_percent),   // X aligned to right edge
    0.0f                                                                                        // Y locked to top
  );

  // Sizing
  ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(
    ImVec2(
      Config::monitor_size.x * Config::settings_panel_width_percent,
      Config::monitor_size.y * Config::settings_panel_height_percent
    ),
    ImGuiCond_Always
  );
  
  constexpr ImGuiWindowFlags SETTINGS_PANEL_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  if (ImGui::Begin("Settings", &_settings_panel_visible, SETTINGS_PANEL_FLAGS)) {
    static const float TEXT_HEIGHT = ImGui::CalcTextSize("Placeholder Text").y;
    const float SETTINGS_PANEL_HEIGHT = ImGui::GetWindowSize().y;

    // --- Toolbar Area ---
    {
      // Add a visual separator line before the toolbar content (optional)
      // ImGui::Separator(); 

      // Style/Color changes for the toolbar background can go here (advanced)

      // Toolbar Buttons
      if (ImGui::Button("📂 New Project")) {
        // Handle 'New Project' action
      }
      ImGui::SameLine(); // Puts the next widget on the same line

      if (ImGui::Button("💾 Save")) {
        // Handle 'Save' action
      }
      ImGui::SameLine();

      if (ImGui::Button("↩️ Undo")) {
        // Handle 'Undo' action
      }
      ImGui::SameLine();
      
      // Add some spacing between the toolbar and the main content
      ImGui::Spacing();
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::Spacing();
    }

    // --- Main Content Area ---
    {
      // Add the main action buttons/controls here
      //ImGui::Spacing();
      
      // Panel Width/Height
      constexpr float SETTINGS_PANEL_MIN_WIDTH = 0.2f;
      constexpr float SETTINGS_PANEL_MAX_WIDTH = 1.0f;
      constexpr float SETTINGS_PANEL_MIN_HEIGHT = 0.2f;
      constexpr float SETTINGS_PANEL_MAX_HEIGHT = 1.0f;
      static const float SLIDER_WIDTH = ImGui::GetFontSize() * 0.75f * 4.0f; // Boxes are the size of 4 characters
      static float width_tmp = Config::settings_panel_width_percent;
      static float height_tmp = Config::settings_panel_height_percent;
      ImGui::PushItemWidth(SLIDER_WIDTH);
      ImGui::InputFloat("Panel Width (%)", &width_tmp, 0.0f, 0.0f, "%.2f");
      width_tmp = std::clamp(width_tmp, SETTINGS_PANEL_MIN_WIDTH, SETTINGS_PANEL_MAX_WIDTH);
      if (!ImGui::IsItemActive() && width_tmp != Config::settings_panel_width_percent) {
        Config::settings_panel_width_percent = width_tmp;
      }
      ImGui::SameLine(0.0f, SLIDER_WIDTH);
      ImGui::InputFloat("Panel Height (%)", &height_tmp, 0.0f, 0.0f, "%.2f");
      height_tmp = std::clamp(height_tmp, SETTINGS_PANEL_MIN_HEIGHT, SETTINGS_PANEL_MAX_HEIGHT);
      if (!ImGui::IsItemActive() && height_tmp != Config::settings_panel_height_percent) {
        Config::settings_panel_height_percent = height_tmp;
      }
      ImGui::PopItemWidth();
      
      // Vsync Button
      ImGui::Spacing();
      static bool vsync_checkbox;
      if (ImGui::Checkbox("VSync (Recommended)", &vsync_checkbox)) {
        Config::vsync = vsync_checkbox;
      }

      // BetterAltTab Data + Debug at the bottom of the window
      static double fps_display;
      if (stopwatch.elapsed_s() > 0.1) {
        fps_display = _fps_timer.getFps();
        stopwatch.reset();
        stopwatch.start();
      }
      
      const float DESCRIPTION_POS = (SETTINGS_PANEL_HEIGHT - (TEXT_HEIGHT * 2.7f));
      ImGui::SetCursorPosY(DESCRIPTION_POS); // 5% as padding
      ImGui::Text("%.0f fps", fps_display);
      ImGui::Text("BetterAltTab %s", Config::VERSION);
    }
  }
  ImGui::End();
}
