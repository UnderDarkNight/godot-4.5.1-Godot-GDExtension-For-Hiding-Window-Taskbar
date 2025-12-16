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

    // 公共方法
    bool hide(Window* window);
    bool show(Window* window);
    bool is_visible(Window* window);

private:
#ifdef _WIN32
    HWND get_window_handle(Window* window);
#endif
};

#endif // HIDE_TASKBAR_EXTENSION_H