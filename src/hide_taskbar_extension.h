#ifndef HIDE_TASKBAR_EXTENSION_H
#define HIDE_TASKBAR_EXTENSION_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/display_server.hpp>

using namespace godot;

class HideTaskBarInWindowsSystem : public Object {
    GDCLASS(HideTaskBarInWindowsSystem, Object);

protected:
    static void _bind_methods();

public:
    HideTaskBarInWindowsSystem();
    ~HideTaskBarInWindowsSystem();

    // 公共方法 - 通过Window对象操作
    bool hide(Window* window);
    bool show(Window* window);
    bool is_visible(Window* window);
    
    // 主窗口相关方法
    bool hide_main_window();
    bool show_main_window();
    bool is_main_window_visible();

    bool set_clickable(Window* window, bool clickable);
    bool is_clickable(Window* window);

    // 在类的公共方法部分添加
    int64_t get_window_system_handle(Window* window);
    
    // // 通过Window对象直接操作的方法
    // bool hide_window_by_object(Window* window);
    // bool show_window_by_object(Window* window);
    // bool is_window_visible_by_object(Window* window);
    
private:
#ifdef _WIN32
    HWND get_window_handle(Window* window);
    HWND get_main_window_handle();
#endif
};

#endif // HIDE_TASKBAR_EXTENSION_H