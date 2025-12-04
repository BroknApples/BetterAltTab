#include "windows_helpers.hpp"

// ---------------------- Instance functions ----------------------
bool isInstanceUnique() {
  HANDLE h_mutex = CreateMutexA(NULL, FALSE, "MyApp_SingleInstance_1234");

  if (h_mutex == NULL) {
    std::cout << "CreateMutex failed (%lu)\n" << GetLastError() << std::endl;
    return false;
  }

  // Must check immediately — anything else can overwrite the error value.
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    std::cout << "Another instance is already running.\n" << std::endl;
    CloseHandle(h_mutex);
    return false;
  }

  return true;
}


// ---------------------- Window functions ----------------------
bool isAltTabWindow(HWND hwnd) {
  if (!IsWindowVisible(hwnd)) return false;

  // Exclude cloaked (hidden by Windows) windows
  DWORD cloaked = 0;
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
    if (cloaked != 0) return false;
  }

  // Must have a title
  const int len = GetWindowTextLengthA(hwnd);
  std::string title(len, '\0');
  GetWindowTextA(hwnd, title.data(), len + 1);
  
  // Exclude tool windows (small floating utility windows)
  LONG ex_style = GetWindowLongA(hwnd, GWL_EXSTYLE);
  if (ex_style & WS_EX_TOOLWINDOW) return false;

  // If has an owner, must have WS_EX_APPWINDOW to appear in Alt-Tab
  HWND owner = GetWindow(hwnd, GW_OWNER);
  if (owner != NULL && !(ex_style & WS_EX_APPWINDOW)) return false;

  // If NO owner, must NOT be marked as a tool window (already checked)
  // Program Manager / shell windows usually get filtered here

  return true;
}


void addWindowToVisibleAltTabList(std::vector<WindowInfo>& list, HWND hwnd) {
  if (isAltTabWindow(hwnd) && std::none_of(list.begin(), list.end(), [hwnd](const WindowInfo& w) {
      return w.hwnd == hwnd;
    })) {
    
    const int len = GetWindowTextLengthA(hwnd);
    std::string title(len, '\0');
    GetWindowTextA(hwnd, title.data(), len + 1);
    list.emplace_back(hwnd, title);
  }
}


void removeWindowFromWindowInfoList(std::vector<WindowInfo>& list, const HWND hwnd) {
  auto it = std::find_if(list.begin(), list.end(), [hwnd](const WindowInfo& w){
    return w.hwnd == hwnd; // check if hwnd matches
  });
  
  if (it != list.end()) {
    list.erase(it);
  }
}


bool updateWindowInfoListItem(std::vector<WindowInfo>& list, const HWND hwnd) {
  auto it = std::find_if(list.begin(), list.end(), [hwnd](const WindowInfo& w){
    return w.hwnd == hwnd; // check if hwnd matches
  });
  
  if (it != list.end()) {
    WindowInfo& w = *it;

    const int len = GetWindowTextLengthA(hwnd);
    std::string new_title(len, '\0');
    GetWindowTextA(hwnd, new_title.data(), len + 1);

    w.title = new_title;
    return true;
  }
  
  // Value not found, so return false.
  return false;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM l_param) {
  auto* list = reinterpret_cast<std::vector<WindowInfo>*>(l_param);

  if (list) {
    addWindowToVisibleAltTabList(*list, hwnd);
  }

  return TRUE;
}


std::vector<WindowInfo> getAllAltTabWindows() {
  std::vector<WindowInfo> results;
  EnumWindows(EnumWindowsProc, (LPARAM)&results);
  return results;
}
