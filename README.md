
# MiniTray

A lightweight Windows system tray helper written in C++17 using the Win32 API.

MiniTray is a small personal learning project that provides a simple C++ interface for managing a system tray icon and its context menu without external GUI frameworks.

> **Experimental:** This project is under development. The API may change, and the library has not been thoroughly tested for production use.

## Features

- System tray icon registration and removal
- Default Windows icon or custom icon resources
- Tooltip text
- Right-click context menu
- Menu actions using C++ callbacks
- Automatic icon re-registration after Windows Explorer restarts
- RAII-based resource cleanup

## Requirements

- Windows 10/11
- C++17 or later
- Windows SDK

Tested with MSVC 2022 on Windows 11.

## Example

```cpp
#include <Windows.h>
#include "minitray.hpp"

// Inside a Windows application with a message loop:

miniTray::TrayConfig config{};
config.tipText = L"MiniTray Example";
config.iconId = 0;
config.guid = {
  0x8F3A2D71, 0xC946, 0x4E85,
  {0xB2, 0x6F, 0x91, 0xD4, 0x7A, 0x0C, 0xE5, 0x38}
};

miniTray::MiniTray tray{config};

tray.addMenuItem(L"Exit", []() {
  PostQuitMessage(0);
});

tray.createTray();
```

The application must provide a Windows message loop on the thread that owns the tray window.

The GUID above is for demonstration only. Generate a unique GUID for your application.

## Build

Add `minitray.hpp` and `minitray.cpp` to your project.

For MSVC:

```cmd
cl /std:c++17 /EHsc /utf-8 test.cpp minitray.cpp /Fe:MiniTrayTest.exe user32.lib shell32.lib
```

## Status

Experimental / v0.1.0

This library was developed primarily for learning purposes. Use at your own risk.
