// ============================================================
// creature.h  生物系统：抽象基类 Creature + Player/Enemy/NPC
// 设计模式：模板方法（Creature 定义伤害结算骨架，
//           子类覆写 getStrikeDamage 等钩子函数）
// 面向对象：继承、多态、虚函数、组合
// ============================================================
#ifndef MUD_CREATURE_H
#define MUD_CREATURE_H

#include "item.h"

// ---------- 生物抽象基类 ----------
class Creature {
public:
    Creature(const std::string& name, int level);
    virtual ~Creature() = default;

    std::string getName() const { return name_; }
    int getLevel() const { return level_; }

    int getStrength() const { return strength_; }
    int getHealth() const { return health_; }
    int getAgility() const { return agility_; }

    int getMaxHp() const { return maxHp_; }
    int getCurHp() const { return curHp_; }
    bool isAlive() const { return curHp_ > 0; }

    // 受击：根据伤害吸收减免
    void takeDamage(int dmg);

    // 虚函数钩子：攻击伤害（受力量与武器影响）
    virtual int getStrikeDamage() const;
    // 命中率（受敏捷影响）
    virtual int getAccuracy() const;
    // 闪避率（受敏捷影响）
    virtual int getDodge() const;
    // 伤害吸收（受健康与盔甲影响）
    virtual int getAbsorption() const;
    // 速度（决定出手顺序，受敏捷与武器影响）
    virtual int getSpeed() const;

    // 补充生命
    void heal(int amount) { curHp_ = std::min(maxHp_, curHp_ + amount); }
    void setCurHp(int hp) { curHp_ = hp; }

protected:
    std::string name_;
    int level_ = 1;
    int strength_ = 10;   // 力量
    int health_ = 10;     // 健康
    int agility_ = 10;    // 敏捷
    int maxHp_ = 100;     // 最大生命点 MHP
    int curHp_ = 100;     // 当前生命点 CHP
    int baseAttack_ = 10; // 基础攻击
    int baseAbsorb_ = 0;  // 基础伤害吸收
};

// ---------- 玩家 ----------
class Player : public Creature {
public:
    Player(const std::string& name, const std::string& profession);

    std::string getProfession() const { return profession_; }
    int getExp() const { return exp_; }
    int getExpToLevel() const { return expToLevel_; }
    int getMoney() const { return money_; }
    void setMoney(int m) { money_ = m; }
    void addMoney(int m) { money_ = std::max(0, money_ + m); }
    bool spendMoney(int m) {
        if (money_ < m) return false;
        money_ -= m;
        return true;
    }

    // 经验与升级
    void gainExp(int amount);
    void levelUp();
    int getAttributePoints() const { return attrPoints_; }
    // 手动分配属性点
    bool spendAttrPoint(const std::string& attr);

    // 读档恢复：覆盖成长状态（由存档数据驱动）
    void restoreSave(int level, int exp, int money, int attrPoints,
                     int str, int hel, int agi, int curHp);

    // 背包与装备
    const std::vector<std::shared_ptr<Item>>& getInventory() const { return inventory_; }
    void addItem(const std::shared_ptr<Item>& it) { inventory_.push_back(it); }
    bool removeItemAt(size_t idx);

    std::shared_ptr<Weapon> getWeapon() const { return weapon_; }
    std::shared_ptr<Armor> getArmor() const { return armor_; }

    bool equipItem(size_t idx);        // 装备武器/盔甲
    bool unequipWeapon();
    bool unequipArmor();
    bool usePotion(size_t idx);        // 使用药水
    void dropItem(size_t idx);         // 丢弃物品（不回收）

    // 覆写钩子
    int getStrikeDamage() const override;
    int getDodge() const override;
    int getAbsorption() const override;
    int getSpeed() const override;

    std::string statusText() const;    // 角色状态面板

private:
    std::string profession_;
    int exp_ = 0;
    int expToLevel_ = 100;
    int money_ = 100;
    int attrPoints_ = 0;               // 升级获得的自由属性点
    std::vector<std::shared_ptr<Item>> inventory_;
    std::shared_ptr<Weapon> weapon_;   // 当前武器
    std::shared_ptr<Armor> armor_;     // 当前盔甲
};

// ---------- 敌人 ----------
class Enemy : public Creature {
public:
    Enemy(const std::string& name, int level,
          int strength, int health, int agility,
          int baseAttack, int baseAbsorb,
          int expReward, int moneyLo, int moneyHi,
          int dropChance, bool isBoss);

    int getExpReward() const { return expReward_; }
    int rollMoney() const { return Utils::randInt(moneyLo_, moneyHi_); }
    int getDropChance() const { return dropChance_; }
    bool isBoss() const { return isBoss_; }

    // 克隆一份同配置的敌人（用于房间怪物刷新）
    std::shared_ptr<Enemy> clone() const;

private:
    int expReward_;
    int moneyLo_, moneyHi_;
    int dropChance_;   // 0~100，掉落概率（Boss 为 100）
    bool isBoss_;
    // 记录初始属性，供克隆使用
    int initStrength_ = 10, initHealth_ = 10, initAgility_ = 10;
    int initBaseAttack_ = 10, initBaseAbsorb_ = 0;
};

// ---------- 非玩家角色（商人/店主等） ----------
class NPC {
public:
    NPC(const std::string& name, const std::string& title,
        const std::string& greeting);

    std::string getName() const { return name_; }
    std::string getTitle() const { return title_; }
    std::string getGreeting() const { return greeting_; }

    // 商店货物清单
    const std::vector<std::shared_ptr<Item>>& getStock() const { return stock_; }
    void addStock(const std::shared_ptr<Item>& it) { stock_.push_back(it); }

private:
    std::string name_;
    std::string title_;
    std::string greeting_;
    std::vector<std::shared_ptr<Item>> stock_;
};

#endif // MUD_CREATURE_H
