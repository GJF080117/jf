@echo off
rem ============================================================
rem  《剑指苍穹》Windows 一键编译脚本
rem  依赖：MinGW-w64 / w64devkit 的 g++（需支持 C++17）
rem  用法：双击本脚本，或命令行运行 build_windows.bat
rem ============================================================
chcp 65001 >nul
title 编译《剑指苍穹》Windows 版

where g++ >nul 2>nul
if errorlevel 1 (
    echo [错误] 未找到 g++ 编译器。
    echo 请安装 w64devkit 或 MinGW-w64，并将其 bin 目录加入 PATH。
    echo 下载参考：https://github.com/skeeto/w64devkit/releases
    pause
    exit /b 1
)

echo 正在编译（g++ -std=c++17 -O2）...
g++ -std=c++17 -Wall -Wextra -O2 main.cpp item.cpp creature.cpp room.cpp world.cpp game.cpp -o mud.exe
if errorlevel 1 (
    echo [错误] 编译失败，请检查上方错误信息。
    pause
    exit /b 1
)

echo.
echo 编译成功！已生成 mud.exe
echo 运行方式：直接双击 mud.exe，或在命令行输入 mud.exe 回车。
pause
