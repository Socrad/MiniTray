#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")

#include "minitray.hpp"
#include <shellapi.h>
#include <stdexcept>
#include <WinUser.h>

namespace miniTray {

MiniTray::MiniTray(const TrayConfig& c) : iconId(c.iconId), guidItem(c.guid), tipText(c.tipText) {
  if (!taskbarCreated) throw std::runtime_error("Failed to register TaskbarCreated message");
}
MiniTray::~MiniTray() {
  if (trayRegistered) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.guidItem = guidItem;
    nid.hWnd = hwnd;
    nid.uFlags = NIF_GUID;
    Shell_NotifyIconW(NIM_DELETE, &nid);
  }

  if(hwnd) DestroyWindow(hwnd);

  if(hicon && ownsIcon) DestroyIcon(hicon);
}

void MiniTray::createTray() {
  if (!hwnd) createWindow(); // create invisible window for message and set hwnd property
  if (!hicon) loadIcon();
  if (!trayRegistered) registerIcon();
}

void MiniTray::loadIcon() {
  if (iconId == NULL) {
    hicon = LoadIconW(nullptr, reinterpret_cast<LPCWSTR>(IDI_APPLICATION));
  } else {
    hicon = static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(iconId), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
    if (!hicon) {
      hicon = LoadIconW(nullptr, reinterpret_cast<LPCWSTR>(IDI_APPLICATION));
      if (!hicon) throw std::runtime_error("Failed to load tray icon");
    } else { 
      ownsIcon = true;
    }
  }
}

void MiniTray::registerIcon() {
  NOTIFYICONDATAW nid{};
  nid.cbSize = sizeof(NOTIFYICONDATAW);
  nid.hWnd = hwnd;
  nid.uCallbackMessage = TRAY_MESSAGE;
  nid.hIcon = hicon;
  wcsncpy_s(nid.szTip, tipText.c_str(), _TRUNCATE);
  nid.guidItem = guidItem;
  nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;

  if(!Shell_NotifyIconW(NIM_ADD, &nid)) throw std::runtime_error("Failed to add notify icon");
  trayRegistered = true;

  nid.uVersion = NOTIFYICON_VERSION_4;
  if(!Shell_NotifyIconW(NIM_SETVERSION, &nid)) {
    if(!Shell_NotifyIconW(NIM_DELETE, &nid)) throw std::runtime_error("Failed to set notify icon and to delete notify icon");
    trayRegistered = false;
    throw std::runtime_error("Failed to set notify icon");
  }
}

void MiniTray::createWindow() {
  const wchar_t className[] = L"MiniTray Class";
  WNDCLASSW wc{};
  wc.lpfnWndProc = wndProc;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.lpszClassName = className;
  if(!RegisterClassW(&wc)) {
    if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) throw std::runtime_error("Failed to register window class");
  }

  hwnd = CreateWindowExW(
    0, className, L"minitrayMessageWindow", 0,
    0, 0, 0, 0,
    NULL, NULL, wc.hInstance, this
  );
  if(!hwnd) throw std::runtime_error("Failed to create message window for MiniTray"); 
}

void MiniTray::showContextMenu() {
  HMENU menu = CreatePopupMenu();
  if (!menu) throw std::runtime_error("Failed to create tray context menu");
  int i{0};
  for (const auto& item : menuItems) {
    AppendMenuW(menu, MF_STRING, ++i, item.title.c_str());
  }
  POINT pos{};
  GetCursorPos(&pos);
  SetForegroundWindow(hwnd);
  UINT selected = TrackPopupMenu(menu, TPM_RETURNCMD, pos.x, pos.y, 0, hwnd, nullptr);
  DestroyMenu(menu);
  i = 0;
  if (selected > 0 && selected <= menuItems.size()) {
    menuItems[selected - 1].callback();
  }
}

LRESULT CALLBACK MiniTray::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_NCCREATE) {
    auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
    auto self = static_cast<MiniTray*>(cs->lpCreateParams);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    return TRUE;
  }
  auto self = reinterpret_cast<MiniTray*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

  if (uMsg == WM_NCDESTROY) {
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }

  if (uMsg == taskbarCreated && self) {
    self->trayRegistered = false;
    try {
      self->registerIcon();
    } catch (const std::exception& e) {
      OutputDebugStringW(L"miniTray: Failed to register tray icon\n");
    } catch (...) {
      OutputDebugStringW(L"miniTray: Failed to register tray icon\n");
    }
    return 0;
  }

  switch (uMsg) {
    case TRAY_MESSAGE:
      if (LOWORD(lParam) == WM_CONTEXTMENU && self) {
        try {
          self->showContextMenu();
        } catch (const std::exception& e) {
          OutputDebugStringW(L"miniTray: context menu handling failed\n");
        } catch (...) {
          OutputDebugStringW(L"miniTray: context menu handling failed\n");
        }
      }
      return 0;
    default:
      return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }
}
} // namespace miniTray