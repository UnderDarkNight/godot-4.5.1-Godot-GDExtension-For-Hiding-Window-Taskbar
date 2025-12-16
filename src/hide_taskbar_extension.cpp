#include "hide_taskbar_extension.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/display_server.hpp>

using namespace godot;

void HideTaskBarInWindowsSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("hide", "window"), &HideTaskBarInWindowsSystem::hide);
    ClassDB::bind_method(D_METHOD("show", "window"), &HideTaskBarInWindowsSystem::show);
    ClassDB::bind_method(D_METHOD("is_visible", "window"), &HideTaskBarInWindowsSystem::is_visible);
    
    // 主窗口相关方法
    ClassDB::bind_method(D_METHOD("hide_main_window"), &HideTaskBarInWindowsSystem::hide_main_window);
    ClassDB::bind_method(D_METHOD("show_main_window"), &HideTaskBarInWindowsSystem::show_main_window);
    ClassDB::bind_method(D_METHOD("is_main_window_visible"), &HideTaskBarInWindowsSystem::is_main_window_visible);
    
    // 通过Window对象直接操作的方法
    ClassDB::bind_method(D_METHOD("hide_window_by_object", "window"), &HideTaskBarInWindowsSystem::hide_window_by_object);
    ClassDB::bind_method(D_METHOD("show_window_by_object", "window"), &HideTaskBarInWindowsSystem::show_window_by_object);
    ClassDB::bind_method(D_METHOD("is_window_visible_by_object", "window"), &HideTaskBarInWindowsSystem::is_window_visible_by_object);
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
    } else {
        UtilityFunctions::print("Failed to get valid window handle");
    }
    
    return hwnd;
}

HWND HideTaskBarInWindowsSystem::get_main_window_handle() {
    // 直接通过DisplayServer获取主窗口(索引为0)的Windows句柄
    int64_t handle = DisplayServer::get_singleton()->window_get_native_handle(
        DisplayServer::HandleType::WINDOW_HANDLE, 0);
    HWND hwnd = (HWND)handle;
    
    if (hwnd && IsWindow(hwnd)) {
        UtilityFunctions::print("Got main window handle via DisplayServer: ", (uint64_t)hwnd);
        return hwnd;
    } else {
        UtilityFunctions::print("Failed to get main window handle via DisplayServer");
    }
    
    // 如果通过DisplayServer无法获取，则尝试备用方法
    // 枚举所有顶级窗口并查找属于当前进程的窗口
    DWORD current_process_id = GetCurrentProcessId();
    HWND found_hwnd = NULL;
    
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        // 获取窗口所属的进程ID
        DWORD window_process_id;
        GetWindowThreadProcessId(hwnd, &window_process_id);
        
        // 检查是否属于当前进程
        DWORD current_pid = *((DWORD*)lParam);
        if (window_process_id == current_pid) {
            // 检查是否为可见的顶层窗口
            if (IsWindowVisible(hwnd) && GetParent(hwnd) == NULL) {
                // 检查窗口样式，寻找主窗口特征
                LONG style = GetWindowLong(hwnd, GWL_STYLE);
                if ((style & WS_CAPTION) && (style & WS_SYSMENU)) {
                    // 找到可能的主窗口，保存句柄并停止枚举
                    *((HWND*)lParam) = hwnd;
                    return FALSE; // 停止枚举
                }
            }
        }
        return TRUE; // 继续枚举
    }, (LPARAM)&found_hwnd);
    
    if (found_hwnd && IsWindow(found_hwnd)) {
        UtilityFunctions::print("Got main window handle via enumeration: ", (uint64_t)found_hwnd);
        return found_hwnd;
    }
    
    UtilityFunctions::print("Failed to find main window handle");
    return NULL;
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

// 主窗口相关方法
bool HideTaskBarInWindowsSystem::hide_main_window() {
    HWND hwnd = get_main_window_handle();
    
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
            
            UtilityFunctions::print("Successfully hidden main window from taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to hide main window from taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::show_main_window() {
    HWND hwnd = get_main_window_handle();
    
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
            
            UtilityFunctions::print("Successfully shown main window on taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to show main window on taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::is_main_window_visible() {
    HWND hwnd = get_main_window_handle();
    
    if (hwnd && IsWindow(hwnd)) {
        // 检查窗口扩展样式
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        bool hasAppWindow = (exStyle & WS_EX_APPWINDOW) != 0;
        bool hasToolWindow = (exStyle & WS_EX_TOOLWINDOW) != 0;
        
        // 判断是否在任务栏显示
        bool inTaskbar = hasAppWindow && !hasToolWindow;
        
        UtilityFunctions::print("Main window visibility on taskbar: ", inTaskbar ? "Yes" : "No");
        return inTaskbar;
    }
    
    UtilityFunctions::print("Unable to determine main window visibility on taskbar");
    return false;
}

// 通过Window对象直接操作的方法
bool HideTaskBarInWindowsSystem::hide_window_by_object(Window* window) {
    if (!window) {
        UtilityFunctions::print("Window object is null");
        return false;
    }
    
    // 通过DisplayServer直接获取窗口句柄
    uint32_t window_id = window->get_window_id();
    int64_t handle = DisplayServer::get_singleton()->window_get_native_handle(
        DisplayServer::HandleType::WINDOW_HANDLE, window_id);
    HWND hwnd = (HWND)handle;
    
    if (hwnd && IsWindow(hwnd)) {
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle != 0) {
            exStyle &= ~WS_EX_APPWINDOW;
            exStyle |= WS_EX_TOOLWINDOW;
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
            
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            
            UtilityFunctions::print("Successfully hidden window (ID: ", window_id, ") from taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to hide window (ID: ", window_id, ") from taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::show_window_by_object(Window* window) {
    if (!window) {
        UtilityFunctions::print("Window object is null");
        return false;
    }
    
    // 通过DisplayServer直接获取窗口句柄
    uint32_t window_id = window->get_window_id();
    int64_t handle = DisplayServer::get_singleton()->window_get_native_handle(
        DisplayServer::HandleType::WINDOW_HANDLE, window_id);
    HWND hwnd = (HWND)handle;
    
    if (hwnd && IsWindow(hwnd)) {
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (exStyle != 0) {
            exStyle |= WS_EX_APPWINDOW;
            exStyle &= ~WS_EX_TOOLWINDOW;
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
            
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            
            UtilityFunctions::print("Successfully shown window (ID: ", window_id, ") on taskbar");
            return true;
        }
    }
    
    UtilityFunctions::print("Failed to show window (ID: ", window_id, ") on taskbar");
    return false;
}

bool HideTaskBarInWindowsSystem::is_window_visible_by_object(Window* window) {
    if (!window) {
        UtilityFunctions::print("Window object is null");
        return false;
    }
    
    // 通过DisplayServer直接获取窗口句柄
    uint32_t window_id = window->get_window_id();
    int64_t handle = DisplayServer::get_singleton()->window_get_native_handle(
        DisplayServer::HandleType::WINDOW_HANDLE, window_id);
    HWND hwnd = (HWND)handle;
    
    if (hwnd && IsWindow(hwnd)) {
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        bool hasAppWindow = (exStyle & WS_EX_APPWINDOW) != 0;
        bool hasToolWindow = (exStyle & WS_EX_TOOLWINDOW) != 0;
        
        bool inTaskbar = hasAppWindow && !hasToolWindow;
        
        UtilityFunctions::print("Window (ID: ", window_id, ") visibility on taskbar: ", inTaskbar ? "Yes" : "No");
        return inTaskbar;
    }
    
    UtilityFunctions::print("Unable to determine window (ID: ", window_id, ") visibility on taskbar");
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

// 非Windows平台的空实现（新增方法）
bool HideTaskBarInWindowsSystem::hide_main_window() {
    UtilityFunctions::print("Current platform does not support hiding main window taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::show_main_window() {
    UtilityFunctions::print("Current platform does not support showing main window taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::is_main_window_visible() {
    UtilityFunctions::print("Current platform does not support checking main window taskbar visibility status");
    return false;
}

bool HideTaskBarInWindowsSystem::hide_window_by_object(Window* window) {
    UtilityFunctions::print("Current platform does not support hiding window taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::show_window_by_object(Window* window) {
    UtilityFunctions::print("Current platform does not support showing window taskbar functionality");
    return false;
}

bool HideTaskBarInWindowsSystem::is_window_visible_by_object(Window* window) {
    UtilityFunctions::print("Current platform does not support checking window taskbar visibility status");
    return false;
}
#endif