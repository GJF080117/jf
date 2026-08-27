// ============================================================
// game.cpp  游戏引擎实现
// ============================================================
#include "game.h"

using namespace Utils;
using namespace Color;

const std::string SaveLoad::SAVE_PATH = "save.dat";

// ============================================================
// 物品序列化辅助（存档用）
// ============================================================
namespace {
    // 按分隔符分割字符串（存档格式解析用）
    std::vector<std::string> split(const std::string& s, char sep) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : s) {
            if (c == sep) { out.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        out.push_back(cur);
        return out;
    }

    // 物品 → 文本：tag|name|price|rarity|v1|v2
    std::string itemToText(const std::shared_ptr<Item>& it) {
        std::string base = it->typeTag() + "|" + it->getName() + "|" +
                           std::to_string(it->getPrice()) + "|" +
                           std::to_string(static_cast<int>(it->getRarity())) + "|";
        if (auto w = std::dynamic_pointer_cast<Weapon>(it))
            return base + std::to_string(w->getAttack()) + "|" +
                   std::to_string(w->getSpeed());
        if (auto a = std::dynamic_pointer_cast<Armor>(it))
            return base + std::to_string(a->getDefense()) + "|0";
        if (auto p = std::dynamic_pointer_cast<Potion>(it))
            return base + std::to_string(p->getHeal()) + "|0";
        if (auto m = std::dynamic_pointer_cast<Money>(it))
            return base + std::to_string(m->getAmount()) + "|0";
        return "";
    }

    // 文本 → 物品
    std::shared_ptr<Item> textToItem(const std::string& s) {
        auto parts = split(s, '|');
        if (parts.size() < 6) return nullptr;
        std::string tag = parts[0];
        std::string name = parts[1];
        int price = std::stoi(parts[2]);
        Rarity r = static_cast<Rarity>(std::stoi(parts[3]));
        int v1 = std::stoi(parts[4]);
        int v2 = std::stoi(parts[5]);
        return ItemFactory::create(tag, name, price, r, v1, v2);
    }
}

// ============================================================
// 存档系统实现
// ============================================================
bool SaveLoad::save(Player& p, World& w, const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;
    f << "MUDSAVE|1\n";
    f << "name=" << p.getName() << "\n";
    f << "profession=" << p.getProfession() << "\n";
    f << "level=" << p.getLevel() << "\n";
    f << "exp=" << p.getExp() << "\n";
    f << "money=" << p.getMoney() << "\n";
    f << "attrPoints=" << p.getAttributePoints() << "\n";
    f << "strength=" << p.getStrength() << "\n";
    f << "health=" << p.getHealth() << "\n";
    f << "agility=" << p.getAgility() << "\n";
    f << "curHp=" << p.getCurHp() << "\n";
    f << "area=" << w.getCurrentAreaIdx() << "\n";
    f << "room=" << w.getCurrentRoomIdx() << "\n";

    // 各区域通关标志
    std::string cleared;
    for (size_t i = 0; i < w.getAreas().size(); ++i)
        cleared += (i ? "," : "") + std::string(w.isAreaCleared(i) ? "1" : "0");
    f << "cleared=" << cleared << "\n";

    // 背包
    std::string inv;
    for (const auto& it : p.getInventory())
        inv += (inv.empty() ? "" : ";") + itemToText(it);
    f << "inv=" << inv << "\n";

    // 当前装备
    f << "weapon=" << (p.getWeapon() ? itemToText(p.getWeapon()) : "") << "\n";
    f << "armor=" << (p.getArmor() ? itemToText(p.getArmor()) : "") << "\n";
    f.close();
    return true;
}

bool SaveLoad::load(Player& p, World& w, const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::map<std::string, std::string> kv;
    std::string line;
    std::getline(f, line);   // 头行 MUDSAVE|1
    if (line.find("MUDSAVE") == std::string::npos) return false;
    while (std::getline(f, line)) {
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        kv[line.substr(0, eq)] = line.substr(eq + 1);
    }

    p = Player(kv["name"], kv["profession"]);
    // 覆盖成长属性（恢复存档数值）
    p.restoreSave(
        std::stoi(kv["level"]), std::stoi(kv["exp"]), std::stoi(kv["money"]),
        std::stoi(kv["attrPoints"]), std::stoi(kv["strength"]),
        std::stoi(kv["health"]), std::stoi(kv["agility"]),
        std::stoi(kv["curHp"]));

    // 恢复地图位置
    w.enterArea(std::stoi(kv["area"]));
    w.setCurrentRoomIdx(std::stoi(kv["room"]));

    // 恢复通关标志与 Boss 清除
    auto cl = split(kv["cleared"], ',');
    for (size_t i = 0; i < cl.size() && i < w.getAreas().size(); ++i) {
        if (cl[i] == "1") {
            w.getAreas()[i]->setBossCleared(true);
            // 已通关区域：清空 Boss 房间的敌人
            for (auto& r : w.getAreas()[i]->getRooms())
                if (r->hasBoss()) r->getEnemies().clear();
        }
    }

    // 恢复背包
    if (!kv["inv"].empty()) {
        for (auto& s : split(kv["inv"], ';')) {
            auto it = textToItem(s);
            if (it) p.addItem(it);
        }
    }
    // 恢复装备（重新加入背包再装备）
    if (!kv["weapon"].empty()) {
        auto it = textToItem(kv["weapon"]);
        if (it) { p.addItem(it); p.equipItem(p.getInventory().size() - 1); }
    }
    if (!kv["armor"].empty()) {
        auto it = textToItem(kv["armor"]);
        if (it) { p.addItem(it); p.equipItem(p.getInventory().size() - 1); }
    }
    return true;
}

// ============================================================
// 战斗系统实现（回合制）
// ============================================================
bool Combat::fight(Game* game, const std::shared_ptr<Enemy>& enemy) {
    return game->runCombat(enemy);
}

// ============================================================
// 游戏引擎实现
// ============================================================
Game::Game() { world_.build(); }

void Game::print(const std::string& color, const std::string& s) const {
    std::cout << color << s << Color::RESET << std::endl;
}

void Game::showTitle() {
    print(Color::YELLOW,
          "╔════════════════════════════════════════════════╗");
    print(Color::YELLOW,
          "║          剑 指 苍 穹 · 单机文字 MUD           ║");
    print(Color::YELLOW,
          "╚════════════════════════════════════════════════╝");
    print(Color::CYAN,
          " 江湖动荡，群魔乱舞。你，一位初出茅庐的少侠，");
    print(Color::CYAN,
          " 将从新手村出发，历经五重关卡，最终讨伐魔教教主！");
    std::cout << std::endl;
}

void Game::mainMenu() {
    while (true) {
        printBold("── 主菜单 ──");
        std::cout << "  1. 开始新游戏\n"
                  << "  2. 读取存档\n"
                  << "  3. 退出游戏\n";
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        std::string cmd = toLower(trim(line));
        if (cmd == "1" || cmd == "new") {
            createCharacter();
            startGame(true);
            return;
        } else if (cmd == "2" || cmd == "load") {
            player_.reset(new Player("", ""));
            if (SaveLoad::load(*player_, world_, SaveLoad::SAVE_PATH)) {
                print(GREEN, "存档读取成功！欢迎回来，" + player_->getName() + "。");
                startGame(false);
                return;
            } else {
                print(RED, "未找到有效的存档文件。");
            }
        } else if (cmd == "3" || cmd == "quit" || cmd == "q") {
            running_ = false;
            return;
        } else {
            print(RED, "请输入 1 / 2 / 3 选择。");
        }
    }
}

void Game::createCharacter() {
    std::string name, prof;
    while (name.empty()) {
        std::cout << "请输入你的名字：> ";
        std::getline(std::cin, name);
        name = trim(name);
        if (name.empty()) print(RED, "名字不能为空。");
    }
    printBold("选择职业：");
    std::cout << "  1. 剑客   —— 攻守兼备，力量与生命占优\n"
              << "  2. 刺客   —— 身法灵动，高闪避高暴击，生命较低\n"
              << "  3. 道士   —— 均衡发展，依靠药水与装备续航\n";
    while (prof.empty()) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        std::string c = toLower(trim(line));
        if (c == "1" || c == "剑客") prof = "剑客";
        else if (c == "2" || c == "刺客") prof = "刺客";
        else if (c == "3" || c == "道士") prof = "道士";
        else print(RED, "请输入 1 / 2 / 3。");
    }
    player_.reset(new Player(name, prof));
    // 新手装备
    player_->addItem(ItemFactory::create("W", "木剑", 0, Rarity::COMMON, 4, 1));
    player_->equipItem(0);
    player_->addItem(ItemFactory::create("A", "粗布衣", 0, Rarity::COMMON, 2));
    player_->equipItem(0);
    player_->addItem(ItemFactory::makePotion(1));
    print(GREEN, "欢迎你，" + name + "（" + prof + "）！江湖之路由此开始。");
}

void Game::startGame(bool fresh) {
    running_ = true;
    if (fresh) world_.enterArea(0);
    print(GREEN, world_.getCurrentArea()->getEntryDesc());
    refreshRoom();
}

void Game::refreshRoom() {
    auto room = world_.getCurrentRoom();
    room->respawnIfEmpty();
    print(Color::CYAN, room->describe());
    if (room->getNpc()) {
        std::cout << Color::GREEN << "【" << room->getNpc()->getTitle() << "】"
                  << room->getNpc()->getName() << "：" << room->getNpc()->getGreeting()
                  << Color::RESET << std::endl;
    }
    if (world_.isAreaCleared(world_.getCurrentAreaIdx()) &&
        static_cast<int>(world_.getCurrentAreaIdx()) + 1 <
            static_cast<int>(world_.getAreas().size())) {
        print(Color::MAGENTA, "【提示】本区域已通关，输入 next 可前往下一区域。");
    }
}

void Game::enterArea(int idx, bool announce) {
    world_.enterArea(idx);
    if (announce) {
        print(YELLOW, "── 进入新区域：" + world_.getCurrentArea()->getName()
              + "（" + world_.getCurrentArea()->getLevelDesc() + "）──");
        print(GREEN, world_.getCurrentArea()->getEntryDesc());
    }
    refreshRoom();
}

void Game::moveTo(char dir) {
    auto room = world_.getCurrentRoom();
    auto next = room->getExit(dir);
    if (!next) {
        print(RED, "此路不通。");
        return;
    }
    // 找到目标房间在当前区域的索引
    const auto& rooms = world_.getCurrentArea()->getRooms();
    for (size_t i = 0; i < rooms.size(); ++i) {
        if (rooms[i] == next) {
            world_.setCurrentRoomIdx(static_cast<int>(i));
            break;
        }
    }
    refreshRoom();
}

void Game::doLook() {
    print(Color::CYAN, world_.getCurrentRoom()->describe());
}

void Game::doStatus() {
    print(Color::WHITE, player_->statusText());
    if (player_->getAttributePoints() > 0)
        print(Color::YELLOW, "有未分配属性点，可用 train 力量/健康/敏捷 加点。");
}

void Game::doInventory() {
    printBold("── 背包 ──");
    const auto& inv = player_->getInventory();
    if (inv.empty()) {
        print(Color::DIM, "（背包空空如也）");
    } else {
        for (size_t i = 0; i < inv.size(); ++i)
            std::cout << "  " << (i + 1) << ". " << inv[i]->describe() << std::endl;
    }
    std::cout << "金钱：" << Color::YELLOW << player_->getMoney()
              << Color::RESET << " 两\n";
    if (player_->getWeapon()) std::cout << "当前武器：" << player_->getWeapon()->getName() << "\n";
    if (player_->getArmor()) std::cout << "当前盔甲：" << player_->getArmor()->getName() << "\n";
}

int Game::parseIndex(const std::string& arg) const {
    if (arg.empty()) return 0;
    try { return std::stoi(arg); }
    catch (...) { return 0; }
}

void Game::doEquip(const std::string& arg) {
    int idx = parseIndex(arg);
    if (idx <= 0) { print(RED, "用法：equip <背包编号>"); return; }
    if (player_->equipItem(static_cast<size_t>(idx - 1))) {
        print(GREEN, "装备成功！");
    } else {
        print(RED, "装备失败：编号无效或该物品不能装备。");
    }
}

void Game::doUnequip(const std::string& arg) {
    std::string a = toLower(arg);
    if (a.find("weapon") != std::string::npos || a.find("武器") != std::string::npos) {
        if (player_->unequipWeapon()) print(GREEN, "已卸下武器。");
        else print(RED, "当前没有佩戴武器。");
    } else if (a.find("armor") != std::string::npos || a.find("盔甲") != std::string::npos) {
        if (player_->unequipArmor()) print(GREEN, "已卸下盔甲。");
        else print(RED, "当前没有佩戴盔甲。");
    } else {
        print(RED, "用法：unequip weapon / unequip armor");
    }
}

void Game::doUse(const std::string& arg) {
    int idx = parseIndex(arg);
    if (idx <= 0) { print(RED, "用法：use <背包编号>"); return; }
    const auto& inv = player_->getInventory();
    if (idx > static_cast<int>(inv.size()) || inv[idx - 1]->getType() != ItemType::POTION) {
        print(RED, "请选择背包中的药水。");
        return;
    }
    int before = player_->getCurHp();
    if (player_->usePotion(static_cast<size_t>(idx - 1))) {
        print(GREEN, "使用药水，恢复 " + std::to_string(player_->getCurHp() - before)
              + " 点生命。当前生命 " + std::to_string(player_->getCurHp()) + "/"
              + std::to_string(player_->getMaxHp()));
    }
}

void Game::doDrop(const std::string& arg) {
    int idx = parseIndex(arg);
    if (idx <= 0) { print(RED, "用法：drop <背包编号>"); return; }
    const auto& inv = player_->getInventory();
    if (idx > static_cast<int>(inv.size())) { print(RED, "编号无效。"); return; }
    print(YELLOW, "你丢弃了 " + inv[idx - 1]->getName() + "。");
    player_->dropItem(static_cast<size_t>(idx - 1));
}

void Game::doPick(const std::string& arg) {
    auto& items = world_.getCurrentRoom()->getItems();
    if (items.empty()) { print(RED, "这里没有可以拾取的物品。"); return; }
    int idx = 1;
    if (!arg.empty()) idx = parseIndex(arg);
    if (idx <= 0 || idx > static_cast<int>(items.size())) { print(RED, "编号无效。"); return; }
    auto it = items[idx - 1];
    player_->addItem(it);
    items.erase(items.begin() + idx - 1);
    print(GREEN, "你拾取了 " + it->getName() + "。");
}

void Game::doAttack(const std::string& arg) {
    if (inBattle_) return;
    auto room = world_.getCurrentRoom();
    auto& enemies = room->getEnemies();
    if (enemies.empty()) { print(RED, "这里没有敌人。"); return; }

    int idx = 1;
    if (!arg.empty()) idx = parseIndex(arg);
    if (idx <= 0 || idx > static_cast<int>(enemies.size())) {
        print(RED, "敌人编号无效。使用 look 查看当前敌人。");
        return;
    }
    auto target = enemies[idx - 1];
    if (!target->isAlive()) { print(RED, "该敌人已被击败。"); return; }

    bool win = runCombat(target);
    room->removeDeadEnemies();

    if (!win) {
        gameOver();
    }
}

void Game::doDefend() { print(RED, "防御只在战斗中有效。"); }
void Game::doFlee() { print(RED, "逃跑只在战斗中有效。"); }

void Game::doShopList() {
    auto npc = world_.getCurrentRoom()->getNpc();
    if (!npc) { print(RED, "这里没有商店。"); return; }
    printBold("── " + npc->getName() + "（" + npc->getTitle() + "）的货架 ──");
    const auto& stock = npc->getStock();
    if (stock.empty()) { print(Color::DIM, "（货架已售空）"); return; }
    for (size_t i = 0; i < stock.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << stock[i]->describe() << std::endl;
    print(Color::DIM, "使用 buy <编号> 购买，sell <背包编号> 出售。");
}

void Game::doBuy(const std::string& arg) {
    auto npc = world_.getCurrentRoom()->getNpc();
    if (!npc) { print(RED, "这里没有商店。"); return; }
    int idx = parseIndex(arg);
    const auto& stock = npc->getStock();
    if (idx <= 0 || idx > static_cast<int>(stock.size())) {
        print(RED, "用法：buy <货架编号>"); return;
    }
    auto it = stock[idx - 1];
    if (!player_->spendMoney(it->getPrice())) {
        print(RED, "金钱不足。"); return;
    }
    player_->addItem(it);
    print(GREEN, "你购买了 " + it->getName() + "，花费 " + std::to_string(it->getPrice()) + " 两。");
}

void Game::doSell(const std::string& arg) {
    auto npc = world_.getCurrentRoom()->getNpc();
    if (!npc) { print(RED, "这里没有商人收购。"); return; }
    int idx = parseIndex(arg);
    const auto& inv = player_->getInventory();
    if (idx <= 0 || idx > static_cast<int>(inv.size())) {
        print(RED, "用法：sell <背包编号>"); return;
    }
    auto it = inv[idx - 1];
    int price = it->getPrice() * 2 / 3;   // 半价回收
    if (price <= 0) price = 1;
    player_->addMoney(price);
    player_->dropItem(static_cast<size_t>(idx - 1));
    print(GREEN, "你出售了 " + it->getName() + "，获得 " + std::to_string(price) + " 两。");
}

void Game::doRest() {
    int heal = 20 + player_->getLevel() * 5;
    player_->heal(heal);
    print(GREEN, "你打坐调息，恢复 " + std::to_string(heal) + " 点生命。");
}

void Game::doTrain(const std::string& arg) {
    std::string a = toLower(trim(arg));
    bool ok = false;
    if (a == "力量" || a == "str" || a == "strength") ok = player_->spendAttrPoint("力量");
    else if (a == "健康" || a == "hel" || a == "health") ok = player_->spendAttrPoint("健康");
    else if (a == "敏捷" || a == "agi" || a == "agility") ok = player_->spendAttrPoint("敏捷");
    else {
        print(RED, "用法：train 力量 / train 健康 / train 敏捷"); return;
    }
    if (ok) { print(GREEN, "加点成功！当前状态："); doStatus(); }
    else print(RED, "没有剩余属性点。升级可获得属性点。");
}

void Game::doSave() {
    if (SaveLoad::save(*player_, world_, SaveLoad::SAVE_PATH))
        print(GREEN, "存档成功！（" + SaveLoad::SAVE_PATH + "）");
    else
        print(RED, "存档失败。");
}

void Game::doLoad() {
    print(RED, "读档请退出到主菜单（quit 后选择 2）。战斗中无法读档。");
}

void Game::doNewGame() {
    print(RED, "开新档请退出到主菜单（quit 后选择 1）。");
}

void Game::doSay(const std::string& text) {
    if (text.empty()) { print(RED, "用法：say <想说的话>"); return; }
    print(Color::CYAN, "你说道：" + text);
}

void Game::doHelp() {
    printBold("── 游戏命令 ──");
    std::cout <<
        "【移动】north/n 北  south/s 南  east/e 东  west/w 西  look/l 查看\n"
        "【状态】status/st 状态  inventory/i 背包  who 玩家信息  time 时间\n"
        "【装备】equip <编号> 装备  unequip weapon/armor 卸下  use <编号> 用药\n"
        "        drop <编号> 丢弃  pick [编号] 拾取地面物品  train 力量/健康/敏捷 加点\n"
        "【战斗】attack <编号> 攻击  defend 防御  flee 逃跑\n"
        "【商店】list 查看货架  buy <编号> 购买  sell <编号> 出售\n"
        "【其他】rest 休息  save 存档  next 前往下一区域  say <话> 说话\n"
        "        help 帮助  quit/q 退出游戏\n";
    print(Color::DIM, "中文命令同样支持，例如：向北 / 攻击 / 查看背包。");
}

void Game::gameOver() {
    print(RED, "── 你倒下了…… ──");
    int lost = player_->getMoney() * 20 / 100;
    player_->addMoney(-lost);
    print(RED, "你损失了 " + std::to_string(lost) + " 两金钱（仅保留 80%）。");
    world_.enterArea(world_.getCurrentAreaIdx());
    player_->setCurHp(player_->getMaxHp());
    print(GREEN, "你从 " + world_.getCurrentArea()->getName() + " 的入口处苏醒，生命完全恢复。");
    refreshRoom();
}

// ============================================================
// 战斗实现
// ============================================================
void Game::printBattleOptions() const {
    std::cout << Color::YELLOW
              << "  1.攻击  2.防御  3.用药  4.逃跑" << Color::RESET << "\n";
}

bool Game::runCombat(const std::shared_ptr<Enemy>& enemy) {
    inBattle_ = true;
    print(RED, "⚔ 你与「" + enemy->getName() + "」(Lv" +
          std::to_string(enemy->getLevel()) + ") 展开了战斗！");

    bool fled = false;  // 标记玩家是否成功逃跑
    while (player_->isAlive() && enemy->isAlive()) {
        bool playerFirst = player_->getSpeed() >= enemy->getSpeed();

        // ---- 玩家回合 ----
        if (playerFirst) {
            if (!playerTurn(enemy)) { fled = true; break; }
            if (!enemy->isAlive()) break;
        }
        // ---- 敌人回合 ----
        enemyTurn(enemy);
        if (!player_->isAlive()) break;
        if (!playerFirst) {
            if (!playerTurn(enemy)) { fled = true; break; }
        }
    }

    inBattle_ = false;

    // 逃跑成功：不做击杀结算，直接结束战斗
    if (fled) {
        return true;
    }

    // 结算
    if (!player_->isAlive()) {
        print(RED, "你被「" + enemy->getName() + "」击败了……");
        return false;
    }

    // 胜利
    print(GREEN, "🎉 你击败了「" + enemy->getName() + "」！");
    int exp = enemy->getExpReward();
    int money = enemy->rollMoney();
    player_->gainExp(exp);
    player_->addMoney(money);
    print(YELLOW, "获得经验 " + std::to_string(exp)
          + "，金钱 " + std::to_string(money) + " 两。");

    // 掉落
    if (Utils::chance(enemy->getDropChance())) {
        auto loot = ItemFactory::randomLoot(player_->getLevel());
        player_->addItem(loot);
        print(Color::MAGENTA, "💎 掉落装备：" + loot->describe());
    } else {
        print(Color::DIM, "本次没有掉落装备。");
    }

    // Boss 通关判定
    if (enemy->isBoss()) {
        world_.getCurrentArea()->setBossCleared(true);
        int aIdx = world_.getCurrentAreaIdx();
        print(YELLOW, "★ 你击败了本区域首领！「" + world_.getCurrentArea()->getName()
              + "」已通关！");
        if (aIdx + 1 < static_cast<int>(world_.getAreas().size())) {
            print(MAGENTA, "输入 next 前往下一区域："
                  + world_.getAreas()[aIdx + 1]->getName() + "。");
        } else {
            print(Color::BOLD,
                  "╔══════════════════════════════════════╗");
            print(Color::BOLD,
                  "║   🎊 通关！你击败了魔教教主，名震天下！  ║");
            print(Color::BOLD,
                  "║       江湖从此太平，少侠功德无量！        ║");
            print(Color::BOLD,
                  "╚══════════════════════════════════════╝");
        }
    }
    return true;
}

// 玩家回合：返回是否继续战斗（false 表示逃跑成功结束战斗）
bool Game::playerTurn(const std::shared_ptr<Enemy>& enemy) {
    while (true) {
        std::cout << Color::YELLOW << "你的生命 " << player_->getCurHp() << "/"
                  << player_->getMaxHp() << "，敌人生命 " << enemy->getCurHp()
                  << "/" << enemy->getMaxHp() << Color::RESET << "\n";
        printBattleOptions();
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        auto parts = split(line);
        if (parts.empty()) continue;
        std::string cmd = toLower(parts[0]);

        if (cmd == "1" || cmd == "attack" || cmd == "a" || cmd == "攻击") {
            if (!hitCheck(player_->getAccuracy(), enemy->getDodge())) {
                print(Color::DIM, "你的攻击落空了！");
                return true;
            }
            int dmg = player_->getStrikeDamage() - enemy->getAbsorption();
            if (dmg < 1) dmg = 1;
            bool crit = false;
            int critChance = (player_->getProfession() == "刺客") ? 25 : 8;
            if (Utils::chance(critChance)) { dmg = dmg * 2; crit = true; }
            enemy->takeDamage(dmg);
            print(Color::RED, std::string(crit ? "💥 暴击！" : "你挥出") + "造成了 " +
                  std::to_string(dmg) + " 点伤害！");
            return true;
        }
        else if (cmd == "2" || cmd == "defend" || cmd == "d" || cmd == "防御") {
            defending_ = true;
            print(Color::CYAN, "你凝神防御，本回合受到的伤害减半。");
            return true;
        }
        else if (cmd == "3" || cmd == "use" || cmd == "u" || cmd == "用药" || cmd == "使用") {
            // 列出背包中药水
            const auto& inv = player_->getInventory();
            std::vector<size_t> potions;
            for (size_t i = 0; i < inv.size(); ++i)
                if (inv[i]->getType() == ItemType::POTION) potions.push_back(i);
            if (potions.empty()) { print(RED, "你身上没有药水！"); continue; }
            printBold("选择要使用的药水：");
            for (size_t k = 0; k < potions.size(); ++k)
                std::cout << "  " << (k + 1) << ". " << inv[potions[k]]->describe() << std::endl;
            std::cout << "> ";
            std::string ln;
            std::getline(std::cin, ln);
            int sel = parseIndex(trim(ln));
            if (sel <= 0 || sel > static_cast<int>(potions.size())) {
                print(RED, "无效选择。"); continue;
            }
            int before = player_->getCurHp();
            player_->usePotion(potions[sel - 1]);
            print(GREEN, "你使用了药水，恢复 "
                  + std::to_string(player_->getCurHp() - before) + " 点生命。");
            return true;
        }
        else if (cmd == "4" || cmd == "flee" || cmd == "f" || cmd == "逃跑") {
            int fleeChance = 55 + (player_->getLevel() - enemy->getLevel()) * 10;
            if (Utils::chance(fleeChance)) {
                print(Color::CYAN, "你成功脱离了战斗！");
                return false;
            } else {
                print(RED, "逃跑失败！敌人紧追不舍。");
                return true;
            }
        }
        else {
            print(RED, "战斗中只能使用：1 攻击 / 2 防御 / 3 用药 / 4 逃跑");
        }
    }
}

// 敌人回合
void Game::enemyTurn(const std::shared_ptr<Enemy>& enemy) {
    if (!enemy->isAlive()) return;
    if (!hitCheck(enemy->getAccuracy(), player_->getDodge())) {
        print(Color::DIM, enemy->getName() + " 的攻击落空了！");
    } else {
        int dmg = enemy->getStrikeDamage() - player_->getAbsorption();
        if (dmg < 1) dmg = 1;
        if (defending_) dmg /= 2;
        player_->takeDamage(dmg);
        print(Color::RED, enemy->getName() + " 对你造成 " + std::to_string(dmg)
              + " 点伤害！");
    }
    defending_ = false;
}

// 命中判定：命中率 vs 闪避率
bool Game::hitCheck(int accuracy, int dodge) const {
    if (Utils::randInt(1, 100) > accuracy) return false;
    if (Utils::randInt(1, 100) <= dodge) return false;
    return true;
}

// ============================================================
// 命令分发与主循环
// ============================================================
void Game::processCommand(const std::string& line) {
    if (line.empty()) return;
    auto parts = split(line);
    std::string cmd = toLower(parts[0]);
    std::string rest = line.substr(parts[0].size());
    rest = trim(rest);

    if (cmd == "north" || cmd == "n" || cmd == "北") moveTo('n');
    else if (cmd == "south" || cmd == "s" || cmd == "南") moveTo('s');
    else if (cmd == "east" || cmd == "e" || cmd == "东") moveTo('e');
    else if (cmd == "west" || cmd == "w" || cmd == "西") moveTo('w');
    else if (cmd == "look" || cmd == "l" || cmd == "查看") doLook();
    else if (cmd == "status" || cmd == "st" || cmd == "状态") doStatus();
    else if (cmd == "inventory" || cmd == "i" || cmd == "背包") doInventory();
    else if (cmd == "who" || cmd == "玩家") doStatus();
    else if (cmd == "equip" || cmd == "装备") doEquip(rest);
    else if (cmd == "unequip" || cmd == "卸下") doUnequip(rest);
    else if (cmd == "use" || cmd == "u" || cmd == "使用") doUse(rest);
    else if (cmd == "drop" || cmd == "丢弃") doDrop(rest);
    else if (cmd == "pick" || cmd == "拾取") doPick(rest);
    else if (cmd == "attack" || cmd == "a" || cmd == "攻击") doAttack(rest);
    else if (cmd == "defend" || cmd == "d" || cmd == "防御") doDefend();
    else if (cmd == "flee" || cmd == "f" || cmd == "逃跑") doFlee();
    else if (cmd == "list" || cmd == "商店") doShopList();
    else if (cmd == "buy" || cmd == "购买") doBuy(rest);
    else if (cmd == "sell" || cmd == "出售") doSell(rest);
    else if (cmd == "rest" || cmd == "休息") doRest();
    else if (cmd == "train" || cmd == "加点") doTrain(rest);
    else if (cmd == "save" || cmd == "存档") doSave();
    else if (cmd == "load" || cmd == "读档") doLoad();
    else if (cmd == "new" || cmd == "新游戏") doNewGame();
    else if (cmd == "next" || cmd == "下一区域") {
        int idx = world_.getCurrentAreaIdx();
        if (!world_.isAreaCleared(idx)) {
            print(RED, "尚未击败本区域首领，无法前往下一区域。");
        } else if (idx + 1 >= static_cast<int>(world_.getAreas().size())) {
            print(RED, "这已是最后一关，你已通关天下！");
        } else {
            enterArea(idx + 1, true);
        }
    }
    else if (cmd == "say" || cmd == "说") doSay(rest);
    else if (cmd == "time" || cmd == "时间") {
        print(Color::CYAN, "此刻江湖月明星稀，正是行走武林之时。");
    }
    else if (cmd == "help" || cmd == "h" || cmd == "帮助") doHelp();
    else if (cmd == "quit" || cmd == "q" || cmd == "退出") {
        running_ = false;
        print(Color::DIM, "少侠，后会有期！");
    }
    else {
        print(RED, "未知命令，输入 help 查看帮助。");
    }
}

void Game::run() {
    showTitle();
    mainMenu();
    if (!running_) return;

    while (running_) {
        std::string line;
        while (running_ && player_->isAlive()) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (!std::cin) { running_ = false; break; }
            processCommand(Utils::trim(line));
        }
        if (running_ && !player_->isAlive()) gameOver();
    }
}
