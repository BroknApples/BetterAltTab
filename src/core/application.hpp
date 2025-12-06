#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <windows.h>
#include <shellapi.h>
#include <d3d11.h>
#include <tchar.h>
#include <string>
#include <memory>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "timers.hpp"
#include "config.hpp"
#include "resources.h"
#include "widget_layer.hpp"
#include "widget_container.hpp"
#include "windows_helpers.hpp"


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


    // ---------------- DirectX variables ----------------
    static ID3D11Device*           _pd3d_device;
    static ID3D11DeviceContext*    _pd3d_device_context;
    static IDXGISwapChain*         _p_swap_chain;
    static ID3D11RenderTargetView* _main_render_target_view;


    // ---------------- Tray variables ----------------
    static WNDCLASSEX _wc;
    static NOTIFYICONDATA _nid;
    static HWND _hwnd;
    static HICON _h_icon;
    static HWINEVENTHOOK _hook;


    // ---------------- Gui layers ----------------
    static std::unique_ptr<WidgetLayer> _overlay_layer;
    static std::unique_ptr<WidgetLayer> _settings_layer;
    static std::unique_ptr<std::vector<WidgetLayer>> _tab_group_layers;
    static std::unique_ptr<WidgetLayer> _hotkey_layer;


    // ---------------- Misc variables ----------------
    static bool _overlay_visible;
    static bool _settings_visible;
    static std::vector<WindowInfo> _open_windows;
    static FpsTimer _fps_timer;


    // ---------------- Functions  ----------------


    /**
     * @brief Creates a render target
     */
    static void _createRenderTarget();


    /**
     * @brief Cleans up the render target
     */
    static void _cleanupRenderTarget();


    /**
     * @brief Creates a DirectX device
     * @param hwnd Window for the device to draw on
     * @returns bool: True/False of success
     */
    static bool _createDeviceD3D(HWND hwnd);


    /**
     * @brief Cleans up the DirectX device
     */
    static void _cleanupDeviceD3D();


    /**
     * @brief Sets up the widget layers
     */
    static void _setupWidgetLayers();


    /**
     * @brief Adds an icon for the application to the system tray.
     */
    static void _addTrayIcon();


    /**
     * @brief Removes the icon from the system tray.
     */
    static void _removeTrayIcon();


    /**
     * @brief Shows the context menu in the system tray
     */
    static void _showTrayMenu();


    /**
     * @brief Toggles the visibility of the overlay
     */
    static void _toggleOverlayVisible();


    /**
     * @brief Event-handler for the window
     * @param hwnd: Window contetx
     * @param msg: Message
     * @param w_param: wParam
     * @param l_param: lParam
     * @returns LRESULT: Result of the DefWindowProc function
     */
    static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);


    /**
     * @brief Callback for Windows events
     * @param hook: Hook which triggered this callback
     * @param event: Type of event
     * @param hwnd: The window handle associated with the event
     * @param id_object: UI object that generated the event
     * @param id_child: The specific child UI element that triggered the event
     * @param event_thread: The thread ID that triggered this event
     * @param event_time: The time this event was triggered
     */
    static void CALLBACK _WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG id_object,
      LONG id_child, DWORD event_thread, DWORD event_time);
    

  public:
    /**
     * @brief Enfore static-only class
     */
    Application() = delete;


    /**
     * @brief Setup application
     * @param h_instance: Handle of the process
     * @returns bool: True/False of success
     */
    static bool createApplication(HINSTANCE& h_instance);


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
