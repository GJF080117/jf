// ============================================================
// types.h  公共类型与工具定义
// 《剑指苍穹》单机版文字 MUD 游戏
// 面向对象程序设计课程设计
// ============================================================
#ifndef MUD_TYPES_H
#define MUD_TYPES_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <random>
#include <functional>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip>

// ---------- 控制台颜色（ANSI 转义序列，Linux 终端支持） ----------
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
}

// ---------- 常用工具函数 ----------
namespace Utils {
    // 全局随机数引擎
    inline std::mt19937& rng() {
        static std::mt19937 engine(std::random_device{}());
        return engine;
    }

    // 返回 [lo, hi] 之间的随机整数
    inline int randInt(int lo, int hi) {
        if (hi <= lo) return lo;
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(rng());
    }

    // 以 prob(0~100) 的概率返回 true
    inline bool chance(int prob) {
        return randInt(1, 100) <= prob;
    }

    // 字符串转小写（用于英文命令匹配，不区分大小写）
    inline std::string toLower(const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return r;
    }

    // 去除首尾空白
    inline std::string trim(const std::string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }

    // 分割字符串（按空白）
    inline std::vector<std::string> split(const std::string& s) {
        std::vector<std::string> out;
        std::istringstream iss(s);
        std::string word;
        while (iss >> word) out.push_back(word);
        return out;
    }

    // 输出一行彩色文本
    inline void print(const std::string& color, const std::string& text) {
        std::cout << color << text << Color::RESET << std::endl;
    }

    inline void printBold(const std::string& text) {
        std::cout << Color::BOLD << text << Color::RESET << std::endl;
    }
}

// ---------- 物品品质 ----------
enum class Rarity {
    COMMON,    // 普通（白）
    FINE,      // 精良（绿）
    RARE,      // 稀有（蓝）
    LEGENDARY  // 传说（橙）
};

inline std::string rarityName(Rarity r) {
    switch (r) {
        case Rarity::COMMON:   return "普通";
        case Rarity::FINE:     return "精良";
        case Rarity::RARE:     return "稀有";
        case Rarity::LEGENDARY:return "传说";
    }
    return "未知";
}

inline std::string rarityColor(Rarity r) {
    switch (r) {
        case Rarity::COMMON:   return Color::WHITE;
        case Rarity::FINE:     return Color::GREEN;
        case Rarity::RARE:     return Color::BLUE;
        case Rarity::LEGENDARY:return Color::YELLOW;
    }
    return Color::RESET;
}

// ---------- 物品大类 ----------
enum class ItemType {
    WEAPON,   // 武器
    ARMOR,    // 盔甲
    POTION,   // 药水
    MONEY     // 金钱
};

// ---------- 命令枚举（供统一分发） ----------
enum class Cmd {
    NONE, MOVE_N, MOVE_S, MOVE_E, MOVE_W, LOOK, STATUS, INVENTORY,
    EQUIP, UNEQUIP, USE, DROP, ATTACK, DEFEND, FLEE, HELP, WHO,
    LIST, BUY, SELL, REST, SAVE, LOAD, NEW, SAY, TIME, QUIT
};

// 前向声明（避免头文件循环依赖）
class Item;
class Weapon;
class Armor;
class Potion;
class Money;
class Creature;
class Player;
class Enemy;
class NPC;
class Room;
class Area;
class World;
class Game;

#endif // MUD_TYPES_H
