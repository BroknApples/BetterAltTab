#ifndef WIN_UTILS_HPP
#define WIN_UTILS_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <chrono>
#include <filesystem>
#include <d3d11.h>
#include <windows.h>
#include <dwmapi.h>
#include <psapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "psapi.lib")

// Define PW_RENDERFULLCONTENT if missing (MinGW headers may not have it)
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif


// ---------------------- Struct definitions ----------------------

struct WindowInfo;


// ---------------------- Instance functions ----------------------


/**
 * @brief Checks if the current executable instance is unique
 * @returns bool: True/False of uniqueness
 */
bool isInstanceUnique();


/**
 * @brief Gets the current ram Usage of the program
 * @returns std::string: Ram usage in its best units
 */
std::string getFormattedRamUsage();


/**
 * @brief Opens the windows file explorer at the given path
 * @param path: Path to the file to open at
 */
void openWindowsExplorerAtPath(const std::wstring& path);


// ---------------------- Window functions ----------------------


/**
 * @brief Given an hwnd, find its location in the windows filesystem
 * @param hwnd: Handle of a window
 * @param path: Output variable for the path
 * @returns bool: Success of process
 */
bool getWindowExecutablePath(HWND hwnd, std::wstring& path);


/**
 * @brief Given an hwnd, get its HICON
 * @param hwnd: Hanlde of a window
 * @returns HICON: Icon object
 */
HICON getIconFromHwnd(HWND hwnd);


/**
 * @brief Given an hicon, create a shader resource view for it
 * @param device: Rendering device
 * @param icon: Icon object
 * @param size: Size of the icon in pixels
 * @returns ID3D11ShaderResourceView*: DirectX11 texture
 */
ID3D11ShaderResourceView* createTextureFromIcon(ID3D11Device* device, HICON icon, const int size);


/**
 * @brief Gets the title of a window from its handle
 * @param hwnd: Handle of a window
 * @returns std::string: Title of the window
 */
std::string getWindowTitle(HWND hwnd);


/**
 * @brief Checks if a given hwnd is an alt-tab visible window
 * @returns bool: True/False of visibility
 */
bool isAltTabWindow(HWND hwnd);


/**
 * @brief Adds a window to a list if its an alt-tab window.
 * @param list: List to add to
 * @param hwnd: Window handle to check
 */
void addWindowToAltTabList(std::vector<std::shared_ptr<WindowInfo>>& list, HWND hwnd);


/**
 * @brief Removes a window from a window info list
 * @param list: List to remove from
 * @param hwnd: Handle of the window to remove
 */
void removeWindowFromWindowInfoList(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd);


/**
 * @brief Sets a new title for a window if it's present in a window info list
 * @param list: List to search
 * @param hwnd: Window handle to update
 * @returns bool: True/False of success
 */
bool updateWindowInfoListItemTitle(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd);


/**
 * @brief Updates the stored textures for every WindowInfo object in a list
 * @param list: List to update
 * @param pd3d_device: Device used to render with
 */
void updateWindowInfoListTextures(std::vector<std::shared_ptr<WindowInfo>>& list, ID3D11Device* pd3d_device);


/**
 * @brief Updates the given hwnd's last focus time in a window info list
 * @param list: List to update in
 * @param hwnd: hwnd to update
 * @returns bool: True if the hwnd exists in the list, false otherwise.
 */
bool updateWindowInfoFocusTime(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd);

/**
 * @brief Callback for EnumWindows
 * @param hwnd: hwnd to check
 * @param lParam: parameters passed to the function
 * @returns BOOL: true/false of success
 */
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM l_param);


/**
 * @brief Gets all the alt-tab visible windows currently active on your computer
 * NOTE: Should probably only use on startup due to performance worries.
 * @returns std::vector<std::shared_ptr<WindowInfo>>: Info about the windows
 */
std::vector<std::shared_ptr<WindowInfo>> getAllAltTabWindows();


/**
 * @brief Sets the specified window to the be the one in focus
 * @param hwnd: Window handle to set to focus
 */
void focusWindow(HWND hwnd);


// --------------------- Window Capturing ---------------------

/**
 * @brief Gets the dimensions of a DirectX11 texture
 */
std::pair<UINT, UINT> getTexture2DDim(ID3D11ShaderResourceView* tex);


/**
 * @brief Get a BGRA vector of an HBITMAP
 * @param bmp: Bitmap
 * @param pixels: Output buffer
 */
void bitmapToBGRA(HBITMAP bmp, std::vector<uint8_t>& pixels);


/**
 * @brief Get a BGRA vector of an HBITMAP
 * @param bmp: Bitmap
 * @param pixels: Output buffer
 * @param width: Filled in with the width of the vector
 * @param height: Filled in with the height of the vector
 */
void bitmapToBGRA(HBITMAP bmp, std::vector<uint8_t>& pixels, int& width, int& height);


/**
 * @brief Converts an HBITMAP to a texture usable by ImGui
 * @param h_bmp: Bitmap to convert
 * @param pd3d_device: GPU device to render the texture on
 * @returns ID3D11ShaderResourceView*: DirectX11 shader resource.
 */
ID3D11ShaderResourceView* bitmapToShaderResourceView(HBITMAP h_bmp, ID3D11Device* pd3d_device);


/**
 * @brief Scales a bitmap to a new size
 * 
 * NOTE: caller must DeleteObject()
 * @param src_bitmap: Original bitmap
 * @param new_width: New width
 * @param new_height: New height
 * @returns HBITMAP: Resized bitmap
 */
HBITMAP scaleBitmap(HBITMAP src_bitmap, const int new_width, const int new_height);

/**
 * @brief Returns a bitmap of a window, regardless of its visibility
 * @param hwnd: Window handle
 * @returns HBITMAP: bitmap
 */
HBITMAP captureWindow(HWND hwnd);


/**
 * @brief Returns a bitmap of a window currently visible on the screen
 * @param hwnd: Window handle
 * @returns HBITMAP: bitmap
 */
HBITMAP captureVisibleWindow(HWND hwnd);


class DwmThumbnail {
  private:
    HTHUMBNAIL _thumbnail;
    HWND _target;
    HWND _host;

  public:
    /**
     * @brief Default constructor.
     */
    DwmThumbnail();
    

    /**
     * @brief Default destructor.
     */
    ~DwmThumbnail();


    /**
     * @brief Register a thumbnail for a target window
     * @param host_hwnd: Window to draw into
     * @param target_hwnd: Window to draw
     * @returns bool: True/False of success
     */
    bool registerThumbnail(HWND host_hwnd, HWND target_hwnd);


    /**
     * @brief Update thumbnail properties (position, size, visible)
     * @param dest: Destination rectangle
     * @param visible: Should it actally be drawn?
     */
    void updateThumbnail(const RECT dest, const bool visible = true);


    /**
     * @brief Resize/position easily using left/top/right/bottom
     * @param left: x-position of the top-left corner
     * @param top: y-position of the top-left corner
     * @param width: width of the rect
     * @param height: height of the rect
     * @param visible: Should it actally be drawn?
     */
    void updateSize(const int left, const int top, const int width, const int height, const bool visible = true);


    /**
     * @brief Unregister thumbnail
     */
    void unregisterThumbnail();

    /**
     * @brief checks if the current class is registered to the dwm api
     * @returns bool: True/False of if the thumbnail is registered
     */
    bool isRegistered() const {
      return _thumbnail != nullptr;
    }
};


// ------------------ Premade capturing functions ------------------

/**
 * @brief Captures the image of a window and creates a gpu texture of (width * height) size
 * @param hwnd: Window handle
 * @param pd3d_device: GPU device to render with
 * @param width: Width of the texture (DEFAULT = -1; No scaling)
 * @param height: Height of the texture (DEFAULT = -1; No scaling)
 * @param tex: Texture to write into
 * @returns bool: Success?
 */
bool buildWindowTextureFromHwnd(const HWND hwnd, ID3D11ShaderResourceView*& tex, ID3D11Device* pd3d_device, const int width = -1, const int height = -1);


// ------------------ Structs ------------------


/**
 * @brief Holds basic information for a Windows window
 */
struct WindowInfo {
  WindowInfo() = default;
  WindowInfo(HWND h) : hwnd(h), tex(nullptr) {
    title = getWindowTitle(hwnd);
    last_focused = std::chrono::steady_clock::now();
  }
  ~WindowInfo() {
    tex->Release();
    icon->Release();
  }

  HWND hwnd;
  std::string title;
  ID3D11ShaderResourceView* tex;
  ID3D11ShaderResourceView* icon;
  std::chrono::steady_clock::time_point last_focused;
};


#endif // WIN_UTILS_HPP


// Example:

// // build the texture
// const int width = 496 * 3;
// const int height = 279 * 3;
// static ID3D11ShaderResourceView* preview_tex = nullptr;
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
