// ============================================================
// room.h  房间与区域系统
// MUD 以房间为单元的地图：房间包含出口、物品、敌人、NPC
// 区域对实体分组，构成关卡
// ============================================================
#ifndef MUD_ROOM_H
#define MUD_ROOM_H

#include "creature.h"

// ---------- 商店类型 ----------
enum class ShopType {
    NONE, WEAPON_SHOP, POTION_SHOP
};

// ---------- 房间 ----------
class Room {
public:
    Room(int id, const std::string& name, const std::string& desc);

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    std::string getDesc() const { return desc_; }

    // 出口（东南西北）
    void setExit(char dir, const std::shared_ptr<Room>& room);
    std::shared_ptr<Room> getExit(char dir) const;

    // 房间内物品（掉落在地面上的）
    void addItem(const std::shared_ptr<Item>& it) { floorItems_.push_back(it); }
    std::vector<std::shared_ptr<Item>>& getItems() { return floorItems_; }
    void clearItems() { floorItems_.clear(); }

    // 房间内敌人
    void addEnemy(const std::shared_ptr<Enemy>& e) {
        enemies_.push_back(e);
        initialEnemies_.push_back(e);  // 同时记录模板
    }
    std::vector<std::shared_ptr<Enemy>>& getEnemies() { return enemies_; }

    // 怪物刷新：非 Boss 房间且当前无敌人时，从模板重新生成（可反复刷怪练级）
    void respawnIfEmpty();
    // 是否包含 Boss（Boss 不刷新，击败即通关）
    bool hasBoss() const;

    // 房间内 NPC（商店）
    void setNpc(const std::shared_ptr<NPC>& npc, ShopType type) {
        npc_ = npc; shopType_ = type;
    }
    std::shared_ptr<NPC> getNpc() const { return npc_; }
    ShopType getShopType() const { return shopType_; }

    // 房间完整描述（用于 look 命令）
    std::string describe() const;

    // 清理已死亡的敌人
    void removeDeadEnemies();

private:
    int id_;
    std::string name_;
    std::string desc_;
    std::shared_ptr<Room> north_, south_, east_, west_;
    std::vector<std::shared_ptr<Item>> floorItems_;
    std::vector<std::shared_ptr<Enemy>> enemies_;
    std::vector<std::shared_ptr<Enemy>> initialEnemies_;  // 初始模板
    std::shared_ptr<NPC> npc_;
    ShopType shopType_ = ShopType::NONE;
};

// ---------- 区域（关卡） ----------
class Area {
public:
    Area(int id, const std::string& name, const std::string& levelDesc);

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    std::string getLevelDesc() const { return levelDesc_; }
    const std::vector<std::shared_ptr<Room>>& getRooms() const { return rooms_; }
    void addRoom(const std::shared_ptr<Room>& r) { rooms_.push_back(r); }

    bool isBossCleared() const { return bossCleared_; }
    void setBossCleared(bool b) { bossCleared_ = b; }

    std::string getEntryDesc() const { return entryDesc_; }
    void setEntryDesc(const std::string& s) { entryDesc_ = s; }

private:
    int id_;
    std::string name_;
    std::string levelDesc_;   // 建议等级区间
    std::string entryDesc_;   // 进入区域的提示
    std::vector<std::shared_ptr<Room>> rooms_;
    bool bossCleared_ = false;
};

#endif // MUD_ROOM_H
