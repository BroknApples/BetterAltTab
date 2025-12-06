#ifndef WINDOWS_HELPERS_HPP
#define WINDOWS_HELPERS_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX


#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


// Define PW_RENDERFULLCONTENT if missing (MinGW headers may not have it)
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif


// ---------------------- Instance functions ----------------------


/**
 * @brief Checks if the current executable instance is unique
 * @returns bool: True/False of uniqueness
 */
bool isInstanceUnique();


// ---------------------- Window functions ----------------------


/**
 * @brief Holds basic information for a Windows window
 */
struct WindowInfo {
  WindowInfo() = default;
  WindowInfo(HWND h, const std::string& t) : hwnd(h), title(t) {}
  WindowInfo(const std::string& t, HWND h) : hwnd(h), title(t) {}

  HWND hwnd;
  std::string title;
};


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
 * @brief Adds a window to a list if its an alt-tab window and currently visible.
 * @param list: List to add to
 * @param hwnd: Window handle to check
 */
void addWindowToVisibleAltTabList(std::vector<WindowInfo>& list, HWND hwnd);


/**
 * @brief Removes a window from a window info list
 * @param list: List to remove from
 * @param hwnd: Handle of the window to remove
 */
void removeWindowFromWindowInfoList(std::vector<WindowInfo>& list, const HWND hwnd);


/**
 * @brief Sets a new title for a window if it's present in a window info list
 * @param list: List to search
 * @param new_title: New title
 * @returns bool: True/False of success
 */
bool updateWindowInfoListItem(std::vector<WindowInfo>& list, const HWND hwnd);


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
 * @returns std::vector<WindowInfo>: Info about the windows
 */
std::vector<WindowInfo> getAllAltTabWindows();


/**
 * @brief Sets the specified window to the be the one in focus
 * @param hwnd: Window handle to set to focus
 */
void focusWindow(HWND hwnd);


// --------------------- Window Capturing ---------------------


/**
 * @brief Get an RGBA vector of an HBITMAP
 * @param bmp: Bitmap
 * @param output: Output buffer
 */
void bitmapToRGBA(HBITMAP bmp, std::vector<uint8_t>& output);


/**
 * @brief Downscales as bitmap to a new size
 * 
 * NOTE: caller must DeleteObject()
 * @param src_bitmap: Original bitmap
 * @param new_width: New width
 * @param new_height: New height
 * @returns HBITMAP: Resized bitmap
 */
HBITMAP downscaleBitmap(HBITMAP src_bitmap, const int new_width, const int new_height);

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


#endif // WINDOWS_HELPERS_HPP
