#pragma once
#include <string>
#include <functional>
#include <vector>
#include <Windows.h>
namespace miniTray {
struct TrayConfig {
  int iconId;
  GUID guid;
  std::wstring tipText;
};

struct MenuItem {
  std::wstring title;
  std::function<void()> callback;
};

class MiniTray {
  public:
    explicit MiniTray(const TrayConfig& c);
    MiniTray(const MiniTray&) = delete;
    MiniTray& operator=(const MiniTray&) = delete;
    MiniTray(MiniTray&&) = delete;
    MiniTray& operator=(MiniTray&&) = delete;
    ~MiniTray();
    void createTray();
    void addMenuItem(const std::wstring& title, std::function<void()> cb) { menuItems.push_back(MenuItem{title,cb}); }

  private:
    static constexpr UINT TRAY_MESSAGE{WM_APP + 1};
    inline static UINT taskbarCreated{RegisterWindowMessageW(L"TaskbarCreated")};
    HWND hwnd{};
    int iconId{0};
    HICON hicon{};
    GUID guidItem{};
    std::wstring tipText;
    std::vector<MenuItem> menuItems{};
    bool ownsIcon{false};
    bool trayRegistered{false};

    void createWindow();
    void loadIcon();
    void registerIcon();
    void showContextMenu();
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
} // namespace miniTray