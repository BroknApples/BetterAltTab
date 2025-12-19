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


// ---------------- Misc variables ----------------

bool               Application::_overlay_visible = false;
FpsTimer           Application::_fps_timer{};
TabGroupMap        Application::_tab_groups{};
TabGroupLayoutList Application::_tab_group_layouts{};


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
  AppendMenu(h_menu, MF_STRING, TrayItems::SHOW_OVERLAY, show_overlay_text);
  AppendMenu(h_menu, MF_SEPARATOR, TrayItems::SEPARATOR, NULL);
  
  const TCHAR* show_tab_groups_text = ImGuiUI::isTabGroupsVisible() ? TEXT("* Hide Tabs") : TEXT("Show Tabs");
  AppendMenu(h_menu, MF_STRING, TrayItems::SHOW_TAB_GROUPS, show_tab_groups_text);

  const TCHAR* show_hotkeys_text = ImGuiUI::isHotkeyPanelVisible() ? TEXT("* Hide Hotkeys") : TEXT("Show Hotkeys");
  AppendMenu(h_menu, MF_STRING, TrayItems::SHOW_HOTKEYS, show_hotkeys_text);

  const TCHAR* show_settings_text = ImGuiUI::isSettingsPanelVisible() ? TEXT("* Hide Settings") : TEXT("Show Settings");
  AppendMenu(h_menu, MF_STRING, TrayItems::SHOW_SETTINGS, show_settings_text);

  AppendMenu(h_menu, MF_SEPARATOR, TrayItems::SEPARATOR, NULL);
  AppendMenu(h_menu, MF_STRING, TrayItems::EXIT_APP, TEXT("Exit"));

  SetForegroundWindow(_hwnd); // required for menu to disappear correctly
  const int cmd = TrackPopupMenu(h_menu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, _hwnd, NULL);
  DestroyMenu(h_menu);

  switch (cmd) {
    case TrayItems::SHOW_OVERLAY: {
      _toggleOverlayVisible();
      break;
    }
    case TrayItems::SHOW_TAB_GROUPS: {
      const bool NOT_VIS = !ImGuiUI::isTabGroupsVisible();
      if (!_overlay_visible && NOT_VIS) _toggleOverlayVisible();
      ImGuiUI::setTabGroupsVisibility(NOT_VIS);
      break;
    }
    case TrayItems::SHOW_HOTKEYS: {
      const bool NOT_VIS = !ImGuiUI::isHotkeyPanelVisible();
      if (!_overlay_visible && NOT_VIS) _toggleOverlayVisible();
      ImGuiUI::setHotkeyPanelVisibility(NOT_VIS);
      break;
    }
    case TrayItems::SHOW_SETTINGS: {
      const bool NOT_VIS = !ImGuiUI::isSettingsPanelVisible();
      if (!_overlay_visible && NOT_VIS) _toggleOverlayVisible();
      ImGuiUI::setSettingsPanelVisibility(NOT_VIS);
      break;
    }
    case TrayItems::EXIT_APP: {
      PostQuitMessage(0);
      break;
    }
      
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
  // Toggle overlay with INSERT
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
      updateWindowInfoListItem(_tab_groups[StaticTabGroups::OPEN_TABS], hwnd);
      //p("NAMECHANGE");
      break;

    case EVENT_OBJECT_CREATE:
      // Add to list
      addWindowToAltTabList(_tab_groups[StaticTabGroups::OPEN_TABS], hwnd);
      //p("CREATE");
      break;

    case EVENT_OBJECT_DESTROY:
      // Remove from list
      removeWindowFromWindowInfoList(_tab_groups[StaticTabGroups::OPEN_TABS], hwnd);
      //p("DESTROY");
      break;

    // case EVENT_OBJECT_SHOW:
    //   // Add to list
    //   addWindowToVisibleAltTabList(_tab_groups[StaticTabGroups::OPEN_TABS], hwnd);
    //   p("SHOW");
    //   break;

    // case EVENT_OBJECT_HIDE:
    //   // Remove from list
    //   removeWindowFromWindowInfoList(_tab_groups[StaticTabGroups::OPEN_TABS], hwnd);
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

  // -------------------------------------------------
  // ---------------- Windows.h setup ----------------
  // -------------------------------------------------

  // ------ Check icon ------

  // Check if resource is valid
  HRSRC res = FindResource(h_instance, MAKEINTRESOURCE(IDI_BETTER_ALT_TAB_ICON), RT_GROUP_ICON);
  if (res == NULL) {
    MessageBoxA(NULL, "ICON RESOURCE NOT FOUND!", "ERROR", MB_OK);
  }

  // Load Icon
  _h_icon = LoadIcon(h_instance, MAKEINTRESOURCE(IDI_BETTER_ALT_TAB_ICON));


  // ------ Create window class ------
  _wc = {sizeof(WNDCLASSEX), CS_CLASSDC, _WndProc, 0, 0,
    h_instance,
    _h_icon, // big icon
    LoadCursor(NULL, IDC_ARROW),
    NULL, NULL,
    _PROGRAM_HANDLE_NAME,
    _h_icon // small icon
  };
  RegisterClassEx(&_wc);


  // ------ Create Window Handle ------
  _hwnd = CreateWindowEx(
    WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
    _wc.lpszClassName, _T_WINDOW_NAME,
    WS_POPUP,
    0, 0, // Top-Left corner
    Config::monitor_size.x, Config::monitor_size.y,
    NULL, NULL, _wc.hInstance, NULL
  );


  // ------ Windows event callback binding ------

  _hook = SetWinEventHook(
    EVENT_MIN, EVENT_MAX,
    NULL, 
    _WinEventProc,
    0, 0,                   //  --> all processes/threads
    WINEVENT_OUTOFCONTEXT
  );

  // Error when setting the hook
  if (!_hook) {
    std::cout << "Failed to set hook" << std::endl;
    return false;
  }

  SetLayeredWindowAttributes(_hwnd, RGB(0,0,0), 0, LWA_COLORKEY);
  ShowWindow(_hwnd, SW_SHOW);

  // Add tray icon
  _addTrayIcon();

  // -------------------------------------------------
  // ----------------- DirectX setup -----------------
  // -------------------------------------------------

  if (!_createDeviceD3D(_hwnd)) {
    _cleanupDeviceD3D();
    UnregisterClass(_wc.lpszClassName, _wc.hInstance);
    return false;
  }


  // -------------------------------------------------
  // ------------------ ImGui setup ------------------
  // -------------------------------------------------

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  ImGui_ImplWin32_Init(_hwnd);
  ImGui_ImplDX11_Init(_pd3d_device, _pd3d_device_context);


  // -------------------------------------------------
  // ------------------ Misc setup -------------------
  // -------------------------------------------------
  
  // Tab groups
  _tab_groups[StaticTabGroups::OPEN_TABS] = getAllAltTabWindows();
  _tab_group_layouts[StaticTabGroups::OPEN_TABS] = TabGroupLayout::GRID;
  _tab_groups[StaticTabGroups::HOTKEYS] = {};
  _tab_group_layouts[StaticTabGroups::HOTKEYS] = TabGroupLayout::GRID;
  // TODO: Render tab groups from config.json

  // ImGui Widgets
  ImGuiUI::_setupImGuiStyles();

  return true;
}


void Application::runApplication() {
  MSG msg = {};
  while (msg.message != WM_QUIT) {
    // ------------------------ Events ------------------------

    // Check inputs
    _checkInputs();
    
    // Poll messages
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
        continue; // GetMessage() returns 0 on WM_QUIT
      }
    }


    // ------------------------ Render ------------------------

    // Pre-frame setup
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Draw UI onto buffer
    _fps_timer.update();
    if (_overlay_visible) {
      ImGuiUI::drawUI(_fps_timer.getFps(), _fps_timer.getDelta(), _tab_groups, _tab_group_layouts);
    }
    
    // Render onto screen
    ImGui::Render();
    float clear[4] = {0,0,0,0};
    _pd3d_device_context->OMSetRenderTargets(1, &_main_render_target_view, nullptr);
    _pd3d_device_context->ClearRenderTargetView(_main_render_target_view, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    _p_swap_chain->Present(Config::vsync, 0);
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
