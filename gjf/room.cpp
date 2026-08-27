// ============================================================
// room.cpp  房间与区域系统的实现
// ============================================================
#include "room.h"

// ---------- Room ----------
Room::Room(int id, const std::string& name, const std::string& desc)
    : id_(id), name_(name), desc_(desc) {}

void Room::setExit(char dir, const std::shared_ptr<Room>& room) {
    switch (dir) {
        case 'n': north_ = room; break;
        case 's': south_ = room; break;
        case 'e': east_ = room; break;
        case 'w': west_ = room; break;
    }
}

std::shared_ptr<Room> Room::getExit(char dir) const {
    switch (dir) {
        case 'n': return north_;
        case 's': return south_;
        case 'e': return east_;
        case 'w': return west_;
    }
    return nullptr;
}

void Room::removeDeadEnemies() {
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
                       [](const std::shared_ptr<Enemy>& e) {
                           return !e->isAlive();
                       }),
        enemies_.end());
}

void Room::respawnIfEmpty() {
    if (hasBoss()) return;         // Boss 房间不刷新
    if (!enemies_.empty()) return; // 仍有敌人则不动
    enemies_.clear();
    for (const auto& e : initialEnemies_)
        enemies_.push_back(e->clone());
}

bool Room::hasBoss() const {
    for (const auto& e : initialEnemies_)
        if (e->isBoss()) return true;
    return false;
}

std::string Room::describe() const {
    std::ostringstream oss;
    oss << Color::CYAN << "【" << name_ << "】" << Color::RESET << "\n";
    oss << desc_ << "\n";

    // 出口
    std::vector<std::string> exits;
    if (north_) exits.push_back("北(n)");
    if (south_) exits.push_back("南(s)");
    if (east_) exits.push_back("东(e)");
    if (west_) exits.push_back("西(w)");
    oss << "出口：" << (exits.empty() ? "无" : [&]{
        std::string s;
        for (size_t i = 0; i < exits.size(); ++i)
            s += (i ? "、" : "") + exits[i];
        return s;
    }()) << "\n";

    // 敌人
    if (!enemies_.empty()) {
        oss << Color::RED << "敌人：" << Color::RESET;
        for (size_t i = 0; i < enemies_.size(); ++i) {
            oss << (i ? "、" : "") << enemies_[i]->getName()
                << "(Lv" << enemies_[i]->getLevel() << ")"
                << (enemies_[i]->isBoss() ? "★BOSS" : "");
        }
        oss << "\n";
    }
    // NPC
    if (npc_) {
        oss << Color::GREEN << "人物：" << npc_->getName()
            << "（" << npc_->getTitle() << "）" << Color::RESET << "\n";
    }
    // 地面物品
    if (!floorItems_.empty()) {
        oss << Color::YELLOW << "地面物品：" << Color::RESET;
        for (size_t i = 0; i < floorItems_.size(); ++i) {
            oss << (i ? "、" : "") << floorItems_[i]->getName();
        }
        oss << "（使用 pick 拾取）\n";
    }
    return oss.str();
}

// ---------- Area ----------
Area::Area(int id, const std::string& name, const std::string& levelDesc)
    : id_(id), name_(name), levelDesc_(levelDesc) {}
