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



// ---------------- Gui layers ----------------

std::unique_ptr<WidgetLayer>              Application::_overlay_layer = nullptr;
std::unique_ptr<WidgetLayer>              Application::_settings_layer = nullptr;
std::unique_ptr<std::vector<WidgetLayer>> Application::_tab_group_layers = nullptr;
std::unique_ptr<WidgetLayer>              Application::_hotkey_layer = nullptr;


// ---------------- Misc variables ----------------

bool                    Application::_overlay_visible = false;
bool                    Application::_settings_visible = false;
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

void Application::_setupWidgetLayers() {
  // ---------------- Overlay window ----------------
  constexpr ImGuiWindowFlags OVERLAY_FLAGS = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
  _overlay_layer = std::make_unique<WidgetLayer>(_WINDOW_NAME, 0.1f, nullptr, OVERLAY_FLAGS, false);
  _overlay_layer->setLayout(LayoutType::VerticalList);
  _overlay_layer->setMinCellSize(200.0f, 150.0f);
  _overlay_layer->setCellSize(600.0f, 500.0f);
  _overlay_layer->setMaxCellSize(800.0f, 600.0f);
  _overlay_layer->setGridSize(5, 1);
  _overlay_layer->addText("HelpText1", "BetterAltTab Overlay Active");
  _overlay_layer->addText("HelpText2", "Use the tray icon to open Settings or Exit");
  auto& ref = _overlay_layer->addText("FPS Label", std::to_string(_fps_timer.getFps()) + " fps");
  ref.setSetFunction([&ref]() {
     if (auto* t = std::get_if<TextData>(&ref.data)) {
      t->label = std::to_string(_fps_timer.getFps()) + " fps";
    }
  });

  // ---------------- Settings window ----------------
  _settings_layer = std::make_unique<WidgetLayer>("Settings", 1.0f, &_settings_visible, 0, false);
  _settings_layer->setMinCellSize(200.0f, 150.0f);
  _settings_layer->setCellSize(600.0f, 500.0f);
  _settings_layer->setMaxCellSize(800.0f, 600.0f);
  _settings_layer->setToolbarSide(ToolbarSide::Top);
  _settings_layer->setGridSize(2, 2);
  _settings_layer->addText("DescriptionText1", "This is the settings page");
  _settings_layer->addButton("ResizeableButton", "ToggleResize", false, [](bool val) {
    //setSettingsVisibility(false);
    std::cout << "Toggling: " << !_settings_layer->isResizable() << "\n";
    _settings_layer->toggleResizable(!_settings_layer->isResizable());
  });
  _settings_layer->addText("1", "This is the settings page", -1, true);
  _settings_layer->addText("2", "This is the settings page", -1, true);
  _settings_layer->addText("3", "This is the settings page", -1, true);
  _settings_layer->addText("4", "This is the settings page", -1, true);

  _tab_group_layers = std::make_unique<std::vector<WidgetLayer>>();
  _hotkey_layer = std::make_unique<WidgetLayer>("Hotkeys");
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
  
  const TCHAR* show_settings_text = _settings_visible ? TEXT("* Hide Settings") : TEXT("Show Settings");
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
    if (!_overlay_visible && !_settings_visible) _toggleOverlayVisible();
    _settings_visible = !_settings_visible;
  }
  else if (cmd == 3) {
    PostQuitMessage(0);
  }
}


void Application::_toggleOverlayVisible() {
  _overlay_visible = !_overlay_visible;

  if (_overlay_visible) {
    SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    ShowWindow(_hwnd, SW_SHOW);
  } else {
    SetWindowLong(_hwnd, GWL_EXSTYLE, GetWindowLong(_hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    ShowWindow(_hwnd, SW_HIDE);
  }
}


LRESULT CALLBACK Application::_WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param)) return true;

  switch (msg) {
    case WM_APP + 1: {
      // tray callback
      switch (LOWORD(l_param)) {
        case WM_LBUTTONUP: {
          _overlay_visible = !_overlay_visible;
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
  _setupWidgetLayers();
  return true;
}


void Application::runApplication() {
  MSG msg = {};
  while (msg.message != WM_QUIT) {
    // ---------------- Toggle overlay with INSERT ----------------
    if (GetAsyncKeyState(VK_INSERT) & 1) {
      _toggleOverlayVisible();
    }

    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      continue;
    }

    if (!_overlay_visible) {
      Sleep(10);
      continue;
    }

    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Track fps
    _fps_timer.update();

    // ------------------------ Render UI ------------------------
    // static ID3D11ShaderResourceView* preview_tex = nullptr;

    // // build the texture
    // const int width = 496 * 3;
    // const int height = 279 * 3;
    // ID3D11ShaderResourceView* tex = nullptr;
    // buildWindowTextureFromHwnd(_open_windows[0].hwnd, _pd3d_device, width, height, tex);

    // // free previous texture safely
    // if (preview_tex)
    //   preview_tex->Release();

    // preview_tex = tex;

    // // imgui render
    // ImGui::SetNextWindowSize(ImVec2{width, height}, ImGuiCond_Once);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    // ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoDecoration);
    // ImGui::Image((ImTextureID)preview_tex, ImVec2{width, height});
    // ImGui::End();
    // ImGui::PopStyleVar();
    

    if (_overlay_visible) {
      _overlay_layer->render();
      if (_settings_visible) {
        _settings_layer->render();
      }
    }
    
    // TODO: One Widget layer for the 10 quick nav hotkeys. (bottom of the screen by default, but can be easily moved to the top, left, right, or your own placement).
    // TODO: A vector of layers for the many tab groups you have set up.
    // --> Get all processes currently open or in the system tray (create a special tab group for system tray processes, always located on the bottom right)

    ImGui::Render();
    float clear[4] = {0,0,0,0};
    _pd3d_device_context->OMSetRenderTargets(1, &_main_render_target_view, nullptr);
    _pd3d_device_context->ClearRenderTargetView(_main_render_target_view, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    _p_swap_chain->Present(1,0);
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
