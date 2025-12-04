#include "application.hpp"

// ---------------- DirectX variables ----------------
ID3D11Device*           Application::_g_pd3dDevice = nullptr;
ID3D11DeviceContext*    Application::_g_pd3dDeviceContext = nullptr;
IDXGISwapChain*         Application::_g_pSwapChain = nullptr;
ID3D11RenderTargetView* Application::_g_mainRenderTargetView = nullptr;


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
std::vector<WindowInfo> Application::_visible_windows = {};


// ---------------- DirectX functions ----------------
void Application::_createRenderTarget() {
  ID3D11Texture2D* p_back_buffer = nullptr;
  _g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer));
  _g_pd3dDevice->CreateRenderTargetView(p_back_buffer, nullptr, &_g_mainRenderTargetView);
  p_back_buffer->Release();
}


void Application::_cleanupRenderTarget() {
  if (_g_mainRenderTargetView) {
    _g_mainRenderTargetView->Release();
    _g_mainRenderTargetView = nullptr;
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
    &_g_pSwapChain, &_g_pd3dDevice, &feature_level,
    &_g_pd3dDeviceContext
  ))) {
    return false;
  }

  _createRenderTarget();
  return true;
}


void Application::_cleanupDeviceD3D() {
  _cleanupRenderTarget();
  if (_g_pSwapChain) {
    _g_pSwapChain->Release();
    _g_pSwapChain = nullptr;
  }
  if (_g_pd3dDeviceContext) {
    _g_pd3dDeviceContext->Release();
    _g_pd3dDeviceContext = nullptr;
  }
  if (_g_pd3dDevice) {
    _g_pd3dDevice->Release();
    _g_pd3dDevice = nullptr;
  }
}


// ----------------- Gui functions -----------------
void Application::_setupWidgetLayers() {
  // ---------------- Overlay window ----------------
  constexpr ImGuiWindowFlags OVERLAY_FLAGS = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
  _overlay_layer = std::make_unique<WidgetLayer>(_WINDOW_NAME, 0.1f, nullptr, OVERLAY_FLAGS);
  _overlay_layer->setLayout(LayoutType::VerticalList);
  _overlay_layer->setCellSize(600.0f, 500.0f);
  _overlay_layer->setGridSize(5, 1);
  _overlay_layer->addText("HelpText1", "BetterAltTab Overlay Active");
  _overlay_layer->addText("HelpText2", "Use the tray icon to open Settings or Exit");
  
  // ---------------- Settings window ----------------
  _settings_layer = std::make_unique<WidgetLayer>("Settings", 1.0f, &_settings_visible);
  _settings_layer->setCellSize(600.0f, 500.0f);
  _settings_layer->setGridSize(2, 2);
  _settings_layer->addText("DescriptionText1", "This is the settings page");
  _settings_layer->addButton("ResizeableButton", "ToggleResize", false, [](bool val) {
    //setSettingsVisibility(false);
    std::cout << "Toggling: " << !_settings_layer->isResizable() << "\n";
    _settings_layer->toggleResizable(!_settings_layer->isResizable());
  });

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
  if (id_object != OBJID_WINDOW) return;

  // char title[256] = {};
  // GetWindowTextA(hwnd, title, sizeof(title));

  switch (event) {
    case EVENT_OBJECT_CREATE:
      // Add to list
      addWindowToVisibleAltTabList(_visible_windows, hwnd);
      //printf("[CREATE] %p \"%s\"\n", hwnd, title);
      break;

    case EVENT_OBJECT_DESTROY:
      // Remove from list
      removeWindowFromWindowInfoList(_visible_windows, hwnd);
      //printf("[DESTROY] %p\n", hwnd);
      break;

    case EVENT_OBJECT_SHOW:
      // Add to list
      addWindowToVisibleAltTabList(_visible_windows, hwnd);
      //printf("[SHOW] %p \"%s\"\n", hwnd, title);
      break;

    case EVENT_OBJECT_HIDE:
      // Remove from list
      removeWindowFromWindowInfoList(_visible_windows, hwnd);
      //printf("[HIDE] %p \"%s\"\n", hwnd, title);
      break;

    case EVENT_SYSTEM_FOREGROUND:
      // Update current selection -> NOT NEEDED HERE
      //printf("[FOREGROUND] %p \"%s\"\n", hwnd, title);
      break;

    case EVENT_OBJECT_NAMECHANGE:
      // Change title
      updateWindowInfoListItem(_visible_windows, hwnd);
      //printf("[NAMECHANGE] %p \"%s\"\n", hwnd, title);
      break;
    }
  
  std::cout << "\nPrinting updated list: \n";
  for (const auto& info : _visible_windows) {
    std::cout << info.title << "\n";
  }
  std::cout << std::endl;
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
  _visible_windows = getAllAltTabWindows();


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
  ImGui_ImplDX11_Init(_g_pd3dDevice, _g_pd3dDeviceContext);

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

    // ------------------------ Render UI ------------------------
    _overlay_layer->render();
    if (_settings_visible) {
      _settings_layer->render();
    }
    // TODO: One Widget layer for the 10 quick nav hotkeys. (bottom of the screen by default, but can be easily moved to the top, left, right, or your own placement).
    // TODO: A vector of layers for the many tab groups you have set up.
    // --> Get all processes currently open or in the system tray (create a special tab group for system tray processes, always located on the bottom right)

    ImGui::Render();
    float clear[4] = {0,0,0,0};
    _g_pd3dDeviceContext->OMSetRenderTargets(1, &_g_mainRenderTargetView, nullptr);
    _g_pd3dDeviceContext->ClearRenderTargetView(_g_mainRenderTargetView, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    _g_pSwapChain->Present(1,0);
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
