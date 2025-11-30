#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#include <windows.h>
#include <shellapi.h>
#include <d3d11.h>
#include <tchar.h>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "../utils/resources.h"
#include "widget_layer.hpp"


// Setup ImGUI WndProc
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);


/**
 * @brief Class that creates the program
 * 
 * NOTE: STATIC-ONLY CLASS
 */
class Application {
  private:
    // ---------------- General variables ----------------
    static constexpr auto _PROGRAM_HANDLE_NAME = TEXT("BetterAltTab"); // Name of the program handle.
    static constexpr auto _T_WINDOW_NAME = TEXT("Overlay"); // Name of the window (TEXT version).
    static constexpr const char* _WINDOW_NAME = "Overlay"; // Name of the window.
    static constexpr const char* _SYSTEM_TRAY_NAME = "BetterAltTab Overlay"; // Name shown in the system tray.
    static int _screen_max_width;
    static int _screen_max_height;


    // ---------------- DirectX variables ----------------
    static ID3D11Device*           _g_pd3dDevice;
    static ID3D11DeviceContext*    _g_pd3dDeviceContext;
    static IDXGISwapChain*         _g_pSwapChain;
    static ID3D11RenderTargetView* _g_mainRenderTargetView;


    // ---------------- Tray variables ----------------
    static WNDCLASSEX _wc;
    static NOTIFYICONDATA _nid;
    static HWND _hwnd;
    static HICON _h_icon;
    static bool _overlay_visible;
    static bool _settings_visible;


    // ---------------- Gui layers ----------------
    static WidgetLayer _overlay_layer;
    static WidgetLayer _settings_layer;
    static std::vector<WidgetLayer> _tab_group_layers;
    static WidgetLayer _hotkey_layer;


    // ---------------- Functions  ----------------


    /**
     * @brief Creates a render target
     */
    static void createRenderTarget();


    /**
     * @brief Cleans up the render target
     */
    static void cleanupRenderTarget();


    /**
     * @brief Creates a DirectX device
     * @param hwnd Window for the device to draw on
     * @returns bool: True/False of success
     */
    static bool createDeviceD3D(HWND hwnd);


    /**
     * @brief Cleans up the DirectX device
     */
    static void cleanupDeviceD3D();


    /**
     * @brief Sets up the widget layers
     */
    static void setupWidgetLayers();


    /**
     * @brief Adds an icon for the application to the system tray.
     */
    static void addTrayIcon();


    /**
     * @brief Removes the icon from the system tray.
     */
    static void removeTrayIcon();


    /**
     * @brief Shows the context menu in the system tray
     */
    static void showTrayMenu();


    /**
     * @brief Toggles the visibility of the overlay
     */
    static void toggleOverlayVisible();


    /**
     * @brief Event-handler for the window
     * @param hwnd: Window contetx
     * @param msg: Message
     * @param w_param: wParam
     * @param l_param: lParam
     * @returns LRESULT: Result of the DefWindowProc function
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    

  public:
    /**
     * @brief Enfore static-only class
     */
    Application() = delete;


    /**
     * @brief Setup application
     */
    static void createApplication(HINSTANCE& h_instance);


    /**
     * @brief Runs the application
     */
    static void runApplication();


    /**
     * @brief Cleanup application
     */
    static void destroyApplication();
};


#endif // APPLICATION_HPP
