// ============================================================
// item.h  物品系统：抽象基类 + 具体物品 + 工厂
// 设计模式：工厂模式（ItemFactory 根据 id 生成物品对象）
//          抽象基类 Item，多态派生 Weapon/Armor/Potion/Money
// ============================================================
#ifndef MUD_ITEM_H
#define MUD_ITEM_H

#include "types.h"

// ---------- 物品抽象基类 ----------
class Item {
public:
    Item(ItemType type, const std::string& name, int price, Rarity rarity);
    virtual ~Item() = default;           // 虚析构，保证多态删除

    ItemType getType() const { return type_; }
    std::string getName() const { return name_; }
    int getPrice() const { return price_; }
    Rarity getRarity() const { return rarity_; }

    // 供存档用的类型标识（虚函数）
    virtual std::string typeTag() const = 0;
    // 物品完整描述（虚函数）
    virtual std::string describe() const = 0;

protected:
    ItemType type_;
    std::string name_;
    int price_;      // 基准价格
    Rarity rarity_;
};

// ---------- 武器 ----------
class Weapon : public Item {
public:
    Weapon(const std::string& name, int price, Rarity rarity,
           int attack, int speed);
    int getAttack() const { return attack_; }
    int getSpeed() const { return speed_; }
    std::string typeTag() const override { return "W"; }
    std::string describe() const override;
private:
    int attack_;   // 攻击力加成
    int speed_;    // 速度加成
};

// ---------- 盔甲 ----------
class Armor : public Item {
public:
    Armor(const std::string& name, int price, Rarity rarity, int defense);
    int getDefense() const { return defense_; }
    std::string typeTag() const override { return "A"; }
    std::string describe() const override;
private:
    int defense_;  // 防御力加成
};

// ---------- 药水（治疗用品） ----------
class Potion : public Item {
public:
    Potion(const std::string& name, int price, Rarity rarity, int heal);
    int getHeal() const { return heal_; }
    std::string typeTag() const override { return "P"; }
    std::string describe() const override;
private:
    int heal_;     // 恢复生命值
};

// ---------- 金钱 ----------
class Money : public Item {
public:
    Money(const std::string& name, int price, Rarity rarity, int amount);
    int getAmount() const { return amount_; }
    std::string typeTag() const override { return "M"; }
    std::string describe() const override;
private:
    int amount_;   // 金额
};

// ---------- 物品工厂（工厂模式） ----------
class ItemFactory {
public:
    // 根据类型标识和配置生成具体物品（多态返回基类指针）
    static std::shared_ptr<Item> create(const std::string& tag,
                                        const std::string& name,
                                        int price, Rarity rarity,
                                        int v1, int v2 = 0);

    // 随机生成一件与等级匹配的战利品（用于掉落系统）
    static std::shared_ptr<Item> randomLoot(int playerLevel);

    // 生成一件药水
    static std::shared_ptr<Item> makePotion(int level);
};

#endif // MUD_ITEM_H
