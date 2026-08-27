// ============================================================
// world.h  世界地图：区域（关卡）序列与房间的连接
// 通过 World 统一管理玩家当前所处区域与房间
// ============================================================
#ifndef MUD_WORLD_H
#define MUD_WORLD_H

#include "room.h"

class World {
public:
    World();
    // 构建全部区域与房间
    void build();

    const std::vector<std::shared_ptr<Area>>& getAreas() const { return areas_; }

    int getCurrentAreaIdx() const { return currentAreaIdx_; }
    int getCurrentRoomIdx() const { return currentRoomIdx_; }

    std::shared_ptr<Area> getCurrentArea() const { return areas_[currentAreaIdx_]; }
    std::shared_ptr<Room> getCurrentRoom() const {
        return areas_[currentAreaIdx_]->getRooms()[currentRoomIdx_];
    }

    // 设置当前房间（按索引，保证区域内部导航）
    void setCurrentRoomIdx(int idx) { currentRoomIdx_ = idx; }
    // 进入指定区域（从入口房间开始）
    void enterArea(int idx) {
        currentAreaIdx_ = idx;
        currentRoomIdx_ = 0;
    }

    // 区域是否已通关（Boss 已击败）
    bool isAreaCleared(int idx) const { return areas_[idx]->isBossCleared(); }

private:
    std::vector<std::shared_ptr<Area>> areas_;
    int currentAreaIdx_ = 0;
    int currentRoomIdx_ = 0;
};

#endif // MUD_WORLD_H
