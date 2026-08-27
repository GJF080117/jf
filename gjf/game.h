// ============================================================
// game.h  游戏引擎：战斗系统、存档系统、命令解析与主循环
// 设计模式：命令分发（命令表 map<string,function>）、
//          单例式游戏上下文（Game 聚合 Player/World）
// ============================================================
#ifndef MUD_GAME_H
#define MUD_GAME_H

#include "world.h"

// ---------- 战斗系统（回合制） ----------
// 封装一次玩家对单名敌人的战斗流程
class Combat {
public:
    // 返回玩家是否胜利
    static bool fight(Game* game, const std::shared_ptr<Enemy>& enemy);
};

// ---------- 存档系统 ----------
class SaveLoad {
public:
    static const std::string SAVE_PATH;

    // 将玩家状态与地图进度写入文件
    static bool save(Player& p, World& w, const std::string& path);
    // 从文件读取并恢复玩家状态与地图进度
    static bool load(Player& p, World& w, const std::string& path);
};

// ---------- 游戏引擎 ----------
class Game {
public:
    Game();
    // 主循环入口
    void run();

private:
    // ---- 游戏状态 ----
    std::unique_ptr<Player> player_;   // 玩家（创建后持有）
    World world_;                      // 世界地图
    bool running_ = false;             // 主循环是否继续
    bool inBattle_ = false;            // 是否处于战斗

    // ---- 生命周期 ----
    void showTitle();                  // 标题画面
    void mainMenu();                   // 新游戏 / 读档 / 退出
    void createCharacter();            // 创建角色
    void startGame(bool fresh);        // 开始 / 继续游戏
    void gameOver();                   // 角色死亡处理
    void enterArea(int idx, bool announce); // 进入区域
    void moveTo(char dir);             // 移动
    void refreshRoom();                // 进入房间后的刷新与描述

    // ---- 命令 ----
    void processCommand(const std::string& line);
    void doLook();
    void doStatus();
    void doInventory();
    void doEquip(const std::string& arg);
    void doUnequip(const std::string& arg);
    void doUse(const std::string& arg);
    void doDrop(const std::string& arg);
    void doPick(const std::string& arg);
    void doAttack(const std::string& arg);
    void doDefend();
    void doFlee();
    void doShopList();
    void doBuy(const std::string& arg);
    void doSell(const std::string& arg);
    void doRest();
    void doTrain(const std::string& arg);
    void doSave();
    void doLoad();
    void doNewGame();
    void doHelp();
    void doSay(const std::string& text);

    // ---- 战斗 ----
    bool runCombat(const std::shared_ptr<Enemy>& enemy);
    void printBattleOptions() const;
    bool playerTurn(const std::shared_ptr<Enemy>& enemy);
    void enemyTurn(const std::shared_ptr<Enemy>& enemy);
    bool hitCheck(int accuracy, int dodge) const;   // 命中判定
    bool defending_ = false;                        // 本回合是否防御

    // ---- 辅助 ----
    int parseIndex(const std::string& arg) const;  // 解析 1-based 索引
    void print(const std::string& color, const std::string& s) const;
    friend class Combat;
};

#endif // MUD_GAME_H
