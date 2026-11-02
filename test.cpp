#include <Windows.h>
#include "minitray.hpp"

LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CLOSE:
      ShowWindow(hwnd, SW_HIDE);
      return 0;
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

  WNDCLASSW wc{};
  wc.lpszClassName = L"TestWindowClass";
  wc.hInstance = hInstance;
  wc.lpfnWndProc = wndProc;
  RegisterClassW(&wc);

  HWND hwnd = CreateWindowExW(
    0, L"TestWindowClass", L"Test Window", WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, hInstance, NULL
  );

  miniTray::TrayConfig tc{};
  tc.tipText = L"MiniTrayTest";
  tc.iconId = NULL;
  tc.guid = {
    0x8F3A2D71, 0xC946, 0x4E85,
    {0xB2, 0x6F, 0x91, 0xD4, 0x7A, 0x0C, 0xE5, 0x38}
  };

  miniTray::MiniTray tray{tc};
  tray.addMenuItem(L"열기", [hwnd, nCmdShow](){ 
    ShowWindow(hwnd, nCmdShow); 
    SetForegroundWindow(hwnd);
  });
  tray.addMenuItem(L"종료", [](){ PostQuitMessage(0); });
  tray.createTray();
  
  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

}