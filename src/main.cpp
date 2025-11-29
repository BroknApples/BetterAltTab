#include <windows.h>
#include <shellapi.h>
#include <d3d11.h>
#include <tchar.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"


#define DEBUG 1

#if DEBUG
  #define DEBUG_PRINT(x) std::cout << x << std::endl
#else
  #define DEBUG_PRINT(x) // nothing
#endif


// ---------------- DirectX globals ----------------
ID3D11Device*           g_pd3dDevice = nullptr;
ID3D11DeviceContext*    g_pd3dDeviceContext = nullptr;
IDXGISwapChain*         g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ---------------- Tray globals ----------------
NOTIFYICONDATA nid = {};
HWND hwnd = nullptr;
bool overlayVisible = false;
bool showSettings = false;

// ---------------- DirectX functions ----------------
void CreateRenderTarget() {
  ID3D11Texture2D* pBackBuffer = nullptr;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
  pBackBuffer->Release();
}


void CleanupRenderTarget() {
  if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

bool CreateDeviceD3D(HWND hwnd) {
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT flags = 0;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL levels[1] = { D3D_FEATURE_LEVEL_11_0 };

  if (FAILED(D3D11CreateDeviceAndSwapChain(
    nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
    levels, 1, D3D11_SDK_VERSION, &sd,
    &g_pSwapChain, &g_pd3dDevice, &featureLevel,
    &g_pd3dDeviceContext
  ))) {
    return false;
  }

  CreateRenderTarget();
  return true;
}


void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
  if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
  if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}


// ---------------- Tray functions ----------------
void AddTrayIcon() {
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hwnd;
  nid.uID = 1;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = WM_APP + 1;
  nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  strncpy(reinterpret_cast<char*>(nid.szTip), "BetterAltTab Overlay", sizeof(nid.szTip) - 1);
  nid.szTip[sizeof(nid.szTip) - 1] = '\0';
  Shell_NotifyIcon(NIM_ADD, &nid);
}


void RemoveTrayIcon() {
  Shell_NotifyIcon(NIM_DELETE, &nid);
}

// Show simple context menu on right click
void ShowTrayMenu() {
  POINT pt;
  GetCursorPos(&pt);

  HMENU hMenu = CreatePopupMenu();
  AppendMenu(hMenu, MF_STRING, 1, TEXT("Settings"));
  AppendMenu(hMenu, MF_STRING, 2, TEXT("Exit"));

  SetForegroundWindow(hwnd); // required for menu to disappear correctly
  int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
  DestroyMenu(hMenu);

  if (cmd == 1) showSettings = true;
  else if (cmd == 2) PostQuitMessage(0);
}


// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0,
                    hInstance, NULL, NULL, NULL, NULL,
                    _T("OverlayWindow"), NULL };
  RegisterClassEx(&wc);

  int screenW = GetSystemMetrics(SM_CXSCREEN);
  int screenH = GetSystemMetrics(SM_CYSCREEN);

  hwnd = CreateWindowEx(
    WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
    wc.lpszClassName, _T("Overlay"),
    WS_POPUP, 0, 0, screenW, screenH,
    NULL, NULL, wc.hInstance, NULL
  );

  SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);
  ShowWindow(hwnd, SW_SHOW);

  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // ---------------- ImGui setup ----------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  AddTrayIcon();

  MSG msg = {};
  while (msg.message != WM_QUIT) {
    // ---------------- Toggle overlay with INSERT ----------------
    if (GetAsyncKeyState(VK_INSERT) & 1) {
      overlayVisible = !overlayVisible;

      if (overlayVisible) {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
        ShowWindow(hwnd, SW_SHOW);
      } else {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
        ShowWindow(hwnd, SW_HIDE);
      }
    }

    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      continue;
    }

    if (!overlayVisible) {
      Sleep(10);
      continue;
    }

    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ---------------- Main overlay UI ----------------
    ImGui::SetNextWindowBgAlpha(0.1f);
    ImGui::Begin("Overlay", nullptr,
      ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus
    );
    ImGui::Text("BetterAltTab Overlay Active");
    ImGui::Text("Use the tray icon to open Settings or Exit");
    ImGui::End();

    // ---------------- Settings window ----------------
    if (showSettings) {
      ImGui::Begin("Settings", &showSettings);
      ImGui::Text("This is the settings page");
      if (ImGui::Button("Close")) showSettings = false;
      ImGui::End();
    }

    ImGui::Render();
    float clear[4] = {0,0,0,0};
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present(1,0);
  }

  // ---------------- Cleanup ----------------
  RemoveTrayIcon();
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
  CleanupDeviceD3D();
  DestroyWindow(hwnd);
  UnregisterClass(wc.lpszClassName, wc.hInstance);
  return 0;
}


// ---------------- WndProc ----------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;

  switch (msg) {
  case WM_APP + 1: // tray callback
    switch (LOWORD(lParam)) {
      case WM_LBUTTONUP: overlayVisible = !overlayVisible; break;
      case WM_RBUTTONUP: ShowTrayMenu(); break;
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
