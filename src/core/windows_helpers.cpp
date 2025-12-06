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

std::string getWindowTitle(HWND hwnd) {
  const int len = GetWindowTextLengthA(hwnd);
  std::string title(len, '\0');
  GetWindowTextA(hwnd, title.data(), len + 1);
  return title;
}


bool isAltTabWindow(HWND hwnd) {
  if (!IsWindowVisible(hwnd)) return false;

  // Exclude cloaked (hidden by Windows) windows
  DWORD cloaked = 0;
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
    if (cloaked != 0) return false;
  }

  // Must have a title
  std::string title = getWindowTitle(hwnd);
  
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
    
    std::string title = getWindowTitle(hwnd);
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
    std::string new_title = getWindowTitle(hwnd);
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


void focusWindow(HWND hwnd) {
  if (IsWindow(hwnd)) {
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);
    SetFocus(hwnd);
  }
}


// --------------------- Window Capturing ---------------------

void bitmapToRGBA(HBITMAP bmp, std::vector<uint8_t>& output) {
  BITMAP info;
  GetObject(bmp, sizeof(BITMAP), &info);
  int w = info.bmWidth;
  int h = info.bmHeight;

  BITMAPINFO bi;
  ZeroMemory(&bi, sizeof(bi));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = w;
  bi.bmiHeader.biHeight = -h; // top-down
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;

  output.resize(w * h * 4);
  HDC hdc = CreateCompatibleDC(NULL);
  GetDIBits(hdc, bmp, 0, h, output.data(), &bi, DIB_RGB_COLORS);
  DeleteDC(hdc);
}


HBITMAP downscaleBitmap(HBITMAP src_bitmap, const int new_width, const int new_height) {
  // Get source bitmap info
  BITMAP bmp;
  GetObject(src_bitmap, sizeof(BITMAP), &bmp);

  const int src_width  = bmp.bmWidth;
  const int src_height = bmp.bmHeight;

  // Create compatible DCs
  HDC src_hdc = CreateCompatibleDC(NULL);
  HDC dest_hdc = CreateCompatibleDC(NULL);

  HBITMAP h_old_src = (HBITMAP)SelectObject(src_hdc, src_bitmap);

  // Create destination bitmap
  HBITMAP dest_bitmap = CreateCompatibleBitmap(src_hdc, new_width, new_height);
  HBITMAP h_old_dest = (HBITMAP)SelectObject(dest_hdc, dest_bitmap);

  // High-quality scaling
  SetStretchBltMode(dest_hdc, HALFTONE); // better than COLORONCOLOR

  // Copy & scale
  StretchBlt(
    dest_hdc, 0, 0, new_width, new_height,
    src_hdc, 0, 0, src_width, src_height,
    SRCCOPY
  );

  // Cleanup
  SelectObject(src_hdc, h_old_src);
  SelectObject(dest_hdc, h_old_dest);
  DeleteDC(src_hdc);
  DeleteDC(dest_hdc);

  return dest_bitmap; // caller must DeleteObject()
}


HBITMAP captureWindow(HWND hwnd) {
  RECT rc;
  GetWindowRect(hwnd, &rc);

  const int width  = rc.right - rc.left;
  const int height = rc.bottom - rc.top;

  HDC h_dc = GetDC(NULL);
  HDC h_mem_dc = CreateCompatibleDC(h_dc);

  HBITMAP h_bitmap = CreateCompatibleBitmap(h_dc, width, height);
  HBITMAP h_old = (HBITMAP)SelectObject(h_mem_dc, h_old);

  PrintWindow(hwnd, h_mem_dc, PW_RENDERFULLCONTENT);

  SelectObject(h_mem_dc, h_old);
  DeleteDC(h_mem_dc);
  ReleaseDC(NULL, h_dc);

  return h_bitmap;
}


HBITMAP captureVisibleWindow(HWND hwnd) {
  RECT rc;
  GetWindowRect(hwnd, &rc);

  const int width  = rc.right - rc.left;
  const int height = rc.bottom - rc.top;

  HDC h_window_dc = GetDC(hwnd);
  HDC h_mem_dc = CreateCompatibleDC(h_window_dc);

  HBITMAP hBitmap = CreateCompatibleBitmap(h_window_dc, width, height);
  HBITMAP h_old = (HBITMAP)SelectObject(h_mem_dc, hBitmap);

  BitBlt(h_mem_dc, 0, 0, width, height, h_window_dc, 0, 0, SRCCOPY);

  SelectObject(h_mem_dc, h_old);
  DeleteDC(h_mem_dc);
  ReleaseDC(hwnd, h_window_dc);

  return hBitmap; // caller must delete with DeleteObject()
}


// -------------- DWM Thumbnail  --------------

DwmThumbnail::DwmThumbnail()
: _thumbnail(nullptr)
, _target(nullptr)
, _host(nullptr) {}


DwmThumbnail::~DwmThumbnail() {
  unregisterThumbnail();
}


bool DwmThumbnail::registerThumbnail(HWND host_hwnd, HWND target_hwnd) {
  if (!host_hwnd || !target_hwnd) return false;

  _host = host_hwnd;
  _target = target_hwnd;

  HRESULT hr = DwmRegisterThumbnail(_host, _target, &_thumbnail);
  return SUCCEEDED(hr) && _thumbnail != nullptr;
}


void DwmThumbnail::updateThumbnail(const RECT dest, const bool visible) {
  if (!_thumbnail) return;

  DWM_THUMBNAIL_PROPERTIES props;
  ZeroMemory(&props, sizeof(props));
  props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE;
  props.rcDestination = dest;
  props.fVisible = visible ? TRUE : FALSE;

  DwmUpdateThumbnailProperties(_thumbnail, &props);
}


void DwmThumbnail::updateSize(const int left, const int top, const int width, const int height, const bool visible) {
  RECT r = { left, top, left + width, top + height };
  updateThumbnail(r, visible);
}


void DwmThumbnail::unregisterThumbnail() {
  if (_thumbnail) {
    DwmUnregisterThumbnail(_thumbnail);
    _thumbnail = nullptr;
  }
}