#include "win_utils.hpp"

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


std::string getFormattedRamUsage(){
  PROCESS_MEMORY_COUNTERS_EX pmc;
  if (!GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
    return "0 Bytes";
  }

  size_t bytes = pmc.WorkingSetSize;
  const char* units[] = { "Bytes", "KB", "MB", "GB", "TB" };
  int unit_index = 0;
  double size = static_cast<double>(bytes);

  // Keep dividing by 1024 until the number is small enough to read
  while (size >= 1024 && unit_index < 4) {
    size /= 1024.0;
    unit_index++;
  }

  char buffer[64];
  // Use %.2f for decimals if MB or GB, otherwise use %.0f for Bytes
  const char* fmt = (unit_index == 0) ? "%.0f %s" : "%.2f %s";
  snprintf(buffer, sizeof(buffer), fmt, size, units[unit_index]);
  
  return std::string(buffer);
}


void openWindowsExplorerAtPath(const std::wstring& path) {
  std::wstring command = L"explorer.exe /select,\"" + path + L"\"";

  // Use ShellExecute to run the command
  ShellExecuteW(NULL, L"open", L"explorer.exe", (L"/select,\"" + path + L"\"").c_str(), NULL, SW_SHOWDEFAULT);
}


// ---------------------- Window functions ----------------------



bool getWindowExecutablePath(HWND hwnd, std::wstring& path) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);

  // Valid process id
  if (!pid) return false;

  HANDLE h_process = OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE,
    pid
  );

  // Valid process handle
  if (!h_process) return false;

  wchar_t temp[MAX_PATH];
  DWORD size = MAX_PATH;

  bool ok = QueryFullProcessImageNameW(
    h_process,
    0,
    temp,
    &size
  );

  CloseHandle(h_process);

  if (!ok) return false;

  path.assign(temp, size);
  return true;
}


std::string getWindowTitle(HWND hwnd) {
  // Check if window still exists
  if (!IsWindow(hwnd)) return "";
  
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


void addWindowToAltTabList(std::vector<std::shared_ptr<WindowInfo>>& list, HWND hwnd) {
  // Check if window still exists
  if (!IsWindow(hwnd)) return;
  
  if (isAltTabWindow(hwnd) && (std::none_of(list.begin(), list.end(), [hwnd](const std::shared_ptr<WindowInfo>& w) {
      return w->hwnd == hwnd;
    }))) {
    
    list.emplace_back(std::make_shared<WindowInfo>(hwnd));
  }
}


void removeWindowFromWindowInfoList(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd) {
  auto it = std::find_if(list.begin(), list.end(), [hwnd](const std::shared_ptr<WindowInfo>& w){
    return w->hwnd == hwnd; // check if hwnd matches
  });
  
  if (it != list.end()) {
    list.erase(it);
  }
}


bool updateWindowInfoListItemTitle(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd) {
  // Check if window still exists
  if (!IsWindow(hwnd)) return false;
  
  auto it = std::find_if(list.begin(), list.end(), [hwnd](const std::shared_ptr<WindowInfo>& w){
    return w->hwnd == hwnd; // check if hwnd matches
  });
  
  if (it != list.end()) {
    std::shared_ptr<WindowInfo> w = *it;
    w->title = getWindowTitle(hwnd);
    return true;
  }
  
  // Value not found, so return false.
  return false;
}


void updateWindowInfoListTextures(std::vector<std::shared_ptr<WindowInfo>>& list, ID3D11Device* pd3d_device) {
  for (const auto& ptr : list) {
    if (ptr == nullptr) continue;
    
    ID3D11ShaderResourceView* tmp = nullptr;
    if (!buildWindowTextureFromHwnd(ptr->hwnd, tmp, pd3d_device)) continue;

    // Delete old texture if it exists
    if (ptr->tex != nullptr) {
      ptr->tex->Release();
    }

    // Only set a new texture if its valid
    if (tmp != nullptr) {
      ptr->tex = tmp;
    }
  }
}


bool updateWindowInfoFocusTime(std::vector<std::shared_ptr<WindowInfo>>& list, const HWND hwnd) {
  // Check if window still exists
  if (!IsWindow(hwnd)) return false;

  for (const auto& ptr : list) {
    if (ptr->hwnd == hwnd) {
      ptr->last_focused = std::chrono::steady_clock::now();
      return true;
    }
  }

  // HWND did not exist in the list
  return false;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM l_param) {
  auto* list = reinterpret_cast<std::vector<std::shared_ptr<WindowInfo>>*>(l_param);

  if (list) {
    addWindowToAltTabList(*list, hwnd);
  }

  return TRUE;
}


std::vector<std::shared_ptr<WindowInfo>> getAllAltTabWindows() {
  std::vector<std::shared_ptr<WindowInfo>> results;
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

std::pair<UINT, UINT> getTexture2DDim(ID3D11ShaderResourceView* tex) {
  if (!tex) return {0, 0};

  // Get the underlying resource
  ID3D11Resource* resource = nullptr;
  tex->GetResource(&resource);
  if (!resource) return {0, 0};

  D3D11_TEXTURE2D_DESC desc;
  ID3D11Texture2D* tex2D = nullptr;

  // Try to cast to Texture2D
  std::pair<UINT, UINT> ret;
  if (SUCCEEDED(resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex2D))) {
    tex2D->GetDesc(&desc);
    
    ret.first  = desc.Width;
    ret.second = desc.Height;

    // use width & height as needed
    tex2D->Release();
  }

  resource->Release();
  return ret;
}


void bitmapToBGRA(HBITMAP bmp, std::vector<uint8_t>& pixels) {
  int w, h; // Dummy size params
  bitmapToBGRA(bmp, pixels, w, h);  
}


void bitmapToBGRA(HBITMAP bmp, std::vector<uint8_t>& pixels, int& width, int& height) {
  BITMAP info;
  GetObject(bmp, sizeof(BITMAP), &info);
  width = info.bmWidth;
  height = info.bmHeight;

  BITMAPINFO bi;
  ZeroMemory(&bi, sizeof(bi));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = width;
  bi.bmiHeader.biHeight = -height; // top-down
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;

  pixels.resize(width * height * 4);
  HDC hdc = CreateCompatibleDC(NULL);
  GetDIBits(hdc, bmp, 0, height, pixels.data(), &bi, DIB_RGB_COLORS);
  DeleteDC(hdc);

  // Alpha shall be 255 for all places.
  for (size_t i = 0; i < pixels.size(); i += 4) {
    pixels[i + 3] = 255; // set alpha to opaque
  }
}


ID3D11ShaderResourceView* bitmapToShaderResourceView(HBITMAP h_bmp, ID3D11Device* pd3d_device) {
  std::vector<uint8_t> pixels;
  int width;
  int height;
  bitmapToBGRA(h_bmp, pixels, width, height);

  // Create DX11 texture
  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width              = width;
  desc.Height             = height;
  desc.MipLevels          = 1;
  desc.ArraySize          = 1;
  desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.SampleDesc.Count   = 1;
  desc.Usage              = D3D11_USAGE_DEFAULT;
  desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;

  D3D11_SUBRESOURCE_DATA sub = {};
  sub.pSysMem = pixels.data();
  sub.SysMemPitch = width * 4;

  ID3D11Texture2D* tex = nullptr;
  pd3d_device->CreateTexture2D(&desc, &sub, &tex);

  ID3D11ShaderResourceView* srv = nullptr;
  pd3d_device->CreateShaderResourceView(tex, NULL, &srv);
  tex->Release();

  return srv;
}


HBITMAP scaleBitmap(HBITMAP src_bitmap, const int new_width, const int new_height) {
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
  RECT rc{};
  if (!GetWindowRect(hwnd, &rc)) {
    return nullptr;
  }

  const int width  = rc.right - rc.left;
  const int height = rc.bottom - rc.top;

  if (width <= 0 || height <= 0) {
    return nullptr;
  }

  HDC h_dc = GetWindowDC(hwnd);
  if (h_dc == nullptr) {
    return nullptr;
  }

  HDC h_mem_dc = CreateCompatibleDC(h_dc);
  if (h_mem_dc == nullptr) {
    ReleaseDC(hwnd, h_dc);
    return nullptr;
  }

  HBITMAP h_bitmap = CreateCompatibleBitmap(h_dc, width, height);
  if (h_bitmap == nullptr) {
    DeleteDC(h_mem_dc);
    ReleaseDC(hwnd, h_dc);
    return nullptr;
  }

  HBITMAP h_old = (HBITMAP)SelectObject(h_mem_dc, h_bitmap);

  const BOOL OK = PrintWindow(hwnd, h_mem_dc, PW_RENDERFULLCONTENT);

  SelectObject(h_mem_dc, h_old);
  DeleteDC(h_mem_dc);
  ReleaseDC(hwnd, h_dc);

  if (!OK) {
    DeleteObject(h_bitmap);
    return nullptr;
  }

  return h_bitmap;
}


HBITMAP captureVisibleWindow(HWND hwnd) {
  RECT rc;
  GetWindowRect(hwnd, &rc);

  const int width  = rc.right - rc.left;
  const int height = rc.bottom - rc.top;

  HDC h_window_dc = GetDC(hwnd);
  HDC h_mem_dc = CreateCompatibleDC(h_window_dc);

  HBITMAP h_bitmap = CreateCompatibleBitmap(h_window_dc, width, height);
  HBITMAP h_old = (HBITMAP)SelectObject(h_mem_dc, h_bitmap);

  BitBlt(h_mem_dc, 0, 0, width, height, h_window_dc, 0, 0, SRCCOPY);

  SelectObject(h_mem_dc, h_old);
  DeleteDC(h_mem_dc);
  ReleaseDC(hwnd, h_window_dc);

  return h_bitmap; // caller must delete with DeleteObject()
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



// ------------------ Premade capturing functions ------------------

bool buildWindowTextureFromHwnd(const HWND hwnd, ID3D11ShaderResourceView*& tex, ID3D11Device* pd3d_device, const int width, const int height) {
  static constexpr int MINIMUM_RESIZE = 128; // Minimum size required for a resize, if its smaller it won't allow it to work
  static constexpr int MAXIMUM_RESIZE = 8192; // Minimum size required for a resize, if its smaller it won't allow it to work

  // Check if dimension size is valid
  static auto isValidDimension = [](const int dim) {
    return dim == -1 || (dim >= MINIMUM_RESIZE && dim <= MAXIMUM_RESIZE);
  };
  
  // Out of bounds
  if (!isValidDimension(width) || !isValidDimension(height)) {
    return false;
  }

  // Create bitmaps
  HBITMAP bmp = captureWindow(hwnd);
  if (bmp == nullptr) return false;

  if (width != -1 && height != -1) {
    HBITMAP resized = scaleBitmap(bmp, width, height);
    if (!resized) {
      DeleteObject(bmp);
      return false;
    }

    DeleteObject(bmp);
    bmp = resized;
  }

  tex = bitmapToShaderResourceView(bmp, pd3d_device);
  DeleteObject(bmp);

  return tex != nullptr;
}