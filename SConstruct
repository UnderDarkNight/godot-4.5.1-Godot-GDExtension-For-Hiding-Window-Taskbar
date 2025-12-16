# SConstruct - GDExtension构建配置
import os

# 创建构建环境
env = Environment()

# 检查构建目标类型（默认为debug）
target = ARGUMENTS.get('target', 'debug')  # 可以是 'debug' 或 'release'

# 根据编译器类型设置正确的标志
if env['CC'] == 'cl':  # MSVC
    env.Append(CXXFLAGS=[
        '/std:c++17', 
        '/W3',          # 降低警告级别从/W4到/W3
        '/utf-8', 
        '/EHsc',
        # 禁用特定警告
        '/wd4100',      # 未引用的参数
        '/wd4201',      # 非标准扩展：无名称的结构/联合
        '/wd4244',      # 转换可能丢失数据
        '/wd4819',      # 文件包含不能在当前代码页中表示的字符
        '/wd4865'       # 基础类型更改
    ])
    
    # 根据目标类型设置不同的优化选项
    if target == 'release':
        env.Append(CXXFLAGS=['/O2', '/DNDEBUG'])
    else:
        env.Append(CXXFLAGS=['/Od', '/Zi'])
        
    # 启用Unicode支持和TYPED_METHOD_BIND
    env.Append(CPPDEFINES=['UNICODE', '_UNICODE', 'TYPED_METHOD_BIND'])
else:  # GCC/Clang
    env.Append(CXXFLAGS=['-std=c++17', '-Wall', '-Wno-unused-parameter', '-Wno-sign-compare'])
    env.Append(CPPDEFINES=['TYPED_METHOD_BIND'])
    
    # 根据目标类型设置不同的优化选项
    if target == 'release':
        env.Append(CXXFLAGS=['-O3', '-DNDEBUG'])
    else:
        env.Append(CXXFLAGS=['-O0', '-g'])

# 添加包含路径
godot_cpp_path = "./godot-cpp"  # 使用相对路径

env.Append(CPPPATH=[
    '.',
    'src',
    os.path.join(godot_cpp_path, "gdextension"),
    os.path.join(godot_cpp_path, "include"),
    os.path.join(godot_cpp_path, "gen", "include")
])

# 添加库路径
env.Append(LIBPATH=[
    os.path.join(godot_cpp_path, "bin")
])

# 添加库（根据目标平台和构建类型选择合适的库）
# 注意：即使在release模式下也使用debug版本的godot-cpp库，因为我们只有debug版本
output_name = ''
if env['PLATFORM'] == 'win32':
    # 不管是debug还是release构建，都使用debug版本的godot-cpp库
    env.Append(LIBS=[
        'libgodot-cpp.windows.template_debug.x86_64.lib',
        'user32.lib'  # 添加Windows系统库
    ])
    
    # 但输出文件名根据目标类型区分
    if target == 'release':
        output_name = 'hide_taskbar_windows_release'
    else:
        output_name = 'hide_taskbar_windows_debug'
elif env['PLATFORM'] == 'posix':
    env.Append(LIBS=['godot-cpp'])
    output_name = 'hide_taskbar_windows' + ('_release' if target == 'release' else '_debug')

# 定义源文件
sources = [
    'src/hide_taskbar_extension.cpp',
    'src/register_extension.cpp'
]

# 构建GDExtension库
library = env.SharedLibrary(output_name, sources)

# 设置默认目标
Default(library)

# 安装目标
install_target = env.Install('./bin', library)
Alias('install', install_target)