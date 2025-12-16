#include "hide_taskbar_extension.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/display_server.hpp>

using namespace godot;

void HideTaskBarInWindowsSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("hide", "window"), &HideTaskBarInWindowsSystem::hide);
    ClassDB::bind_method(D_METHOD("show", "window"), &HideTaskBarInWindowsSystem::show);
    ClassDB::bind_method(D_METHOD("is_visible", "window"), &HideTaskBarInWindowsSystem::is_visible);
}

HideTaskBarInWindowsSystem::HideTaskBarInWindowsSystem() {
    UtilityFunctions::print("HideTaskBarInWindowsSystem instance created");
}

HideTaskBarInWindowsSystem::~HideTaskBarInWindowsSystem() {
    UtilityFunctions::print("HideTaskBarInWindowsSystem instance destroyed");
}

#ifdef _WIN32
HWND HideTaskBarInWindowsSystem::get_window_handle(Window* window) {
    if (!window) {
        UtilityFunctions::print("Window object is null");
        return NULL;
    }
    
    // 获取窗口ID
    uint32_t window_id = window->get_window_id();
    UtilityFunctions::print("Window ID: ", window_id);
    
    // 通过DisplayServer获取窗口句柄
    int64_t handle = DisplayServer::get_singleton()->window_get_native_handle(
        DisplayServer::HandleType::WINDOW_HANDLE, window_id);
    HWND hwnd = (HWND)handle;
    
    if (hwnd) {
        UtilityFunctions::print("Got window handle: ", (uint64_t)hwnd);
        
        // 获取窗口标题
        TCHAR title[256];
        int length = GetWindowText(hwnd, title, sizeof(title)/sizeof(TCHAR));
        if (length > 0) {
#ifdef UNICODE
            UtilityFunctions::print("Window title: ", title);
#else
            UtilityFunctions::print("Window title: ", String(title));
#endif
        }
    } else {
        UtilityFunctions::print("Failed to get valid window handle");
    }
    
    return hwnd;
}

bool HideTaskBarInWindowsSystem::hide(Window* window) {
    HWND hwnd = get_window_handle(window);
    
    if (hwnd && IsWindow(hwnd)) {
        // 修改窗口扩展样式以隐藏任务栏图标
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle != 0) {
            exStyle &= ~WS_EX_APPWINDOW;  // 移除APPWINDOW标志
            exStyle |= WS_EX_TOOLWINDOW;   // 添加TOOLWINDOW标志
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
            
            // 重新设置窗口以应用更改
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            
            UtilityFunctions::print("Successfully hidden window from taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to hide window from taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::show(Window* window) {
    HWND hwnd = get_window_handle(window);
    
    if (hwnd && IsWindow(hwnd)) {
        // 恢复窗口扩展样式以显示任务栏图标
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle != 0) {
            exStyle |= WS_EX_APPWINDOW;    // 添加APPWINDOW标志
            exStyle &= ~WS_EX_TOOLWINDOW;  // 移除TOOLWINDOW标志
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
            
            // 重新设置窗口以应用更改
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            
            UtilityFunctions::print("Successfully shown window on taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to show window on taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::is_visible(Window* window) {
    HWND hwnd = get_window_handle(window);
    
    if (hwnd && IsWindow(hwnd)) {
        // 检查窗口扩展样式
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        bool hasAppWindow = (exStyle & WS_EX_APPWINDOW) != 0;
        bool hasToolWindow = (exStyle & WS_EX_TOOLWINDOW) != 0;
        
        // 判断是否在任务栏显示
        bool inTaskbar = hasAppWindow && !hasToolWindow;
        
        UtilityFunctions::print("Window visibility on taskbar: ", inTaskbar ? "Yes" : "No");
        return inTaskbar;
    }
    
    UtilityFunctions::print("Unable to determine window visibility on taskbar");
    return false;
}
#else
// 非Windows平台的空实现
bool HideTaskBarInWindowsSystem::hide(Window* window) {
    UtilityFunctions::print("Current platform does not support hiding taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::show(Window* window) {
    UtilityFunctions::print("Current platform does not support showing taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::is_visible(Window* window) {
    UtilityFunctions::print("Current platform does not support checking taskbar visibility status");
    return false;
}
#endif