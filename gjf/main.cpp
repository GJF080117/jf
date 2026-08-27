// ============================================================
// main.cpp  程序入口
// 《剑指苍穹》单机版文字 MUD 游戏
// ============================================================
#include "game.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // ---- Windows 控制台兼容 ----
    // 1) 启用 ANSI 转义序列（彩色字符界面在 cmd / 终端正常显示）
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(hOut, &mode)) {
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    // 2) 将控制台输入 / 输出代码页切换为 UTF-8（保证中文正常显示与输入）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    Game game;
    game.run();
    return 0;
}
