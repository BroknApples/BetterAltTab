#ifndef WINDOWS_HELPERS_HPP
#define WINDOWS_HELPERS_HPP


#include <iostream>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


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
  WindowInfo(HWND h, const std::string& t) : hwnd(h), title(t) {}
  WindowInfo(const std::string& t, HWND h) : hwnd(h), title(t) {}

  HWND hwnd;
  std::string title;
};


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


#endif // WINDOWS_HELPERS_HPP
