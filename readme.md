


```text

本工程文件目录结构：

project-root/
|-- .gitignore
|-- README.md
|-- include
|-- godot-cpp
|-- src/
|    |-- hide_taskbar_extension.cpp
|    |-- hide_taskbar_extension.h
|    |-- register_extension.cpp
|-- SConstruct




只要这些就足够了：

    你需要准备scons 编译环境。（问AI如何部署）

    下载官方的godot-cpp项目。放到同名文件夹里。

    运行scons编译出 relase 和 debug 版本 的动态库。
            命令: scons target=debug
            命令: scons target=release

    然后加载进入 Godot 项目里。


本项目的作用是辅助隐藏Godot的任务栏图标。

Godot 版本为 4.5.1 正常运行。

剩下的问AI吧 ~ 反正也没几个文件 ~
