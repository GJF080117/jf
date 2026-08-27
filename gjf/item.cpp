// ============================================================
// item.cpp  物品系统的实现
// ============================================================
#include "item.h"

// ---------- Item 基类 ----------
Item::Item(ItemType type, const std::string& name, int price, Rarity rarity)
    : type_(type), name_(name), price_(price), rarity_(rarity) {}

// ---------- Weapon ----------
Weapon::Weapon(const std::string& name, int price, Rarity rarity,
               int attack, int speed)
    : Item(ItemType::WEAPON, name, price, rarity),
      attack_(attack), speed_(speed) {}

std::string Weapon::describe() const {
    std::ostringstream oss;
    oss << rarityColor(rarity_) << "[" << rarityName(rarity_) << "]"
        << name_ << Color::RESET
        << "  攻击+" << attack_ << "  速度+" << speed_
        << "  价格:" << price_ << "两";
    return oss.str();
}

// ---------- Armor ----------
Armor::Armor(const std::string& name, int price, Rarity rarity, int defense)
    : Item(ItemType::ARMOR, name, price, rarity), defense_(defense) {}

std::string Armor::describe() const {
    std::ostringstream oss;
    oss << rarityColor(rarity_) << "[" << rarityName(rarity_) << "]"
        << name_ << Color::RESET
        << "  防御+" << defense_
        << "  价格:" << price_ << "两";
    return oss.str();
}

// ---------- Potion ----------
Potion::Potion(const std::string& name, int price, Rarity rarity, int heal)
    : Item(ItemType::POTION, name, price, rarity), heal_(heal) {}

std::string Potion::describe() const {
    std::ostringstream oss;
    oss << Color::MAGENTA << "[" << rarityName(rarity_) << "]"
        << name_ << Color::RESET
        << "  恢复生命+" << heal_
        << "  价格:" << price_ << "两";
    return oss.str();
}

// ---------- Money ----------
Money::Money(const std::string& name, int price, Rarity rarity, int amount)
    : Item(ItemType::MONEY, name, price, rarity), amount_(amount) {}

std::string Money::describe() const {
    std::ostringstream oss;
    oss << Color::YELLOW << name_ << " × " << amount_ << Color::RESET;
    return oss.str();
}

// ---------- 物品工厂 ----------
std::shared_ptr<Item> ItemFactory::create(const std::string& tag,
                                          const std::string& name,
                                          int price, Rarity rarity,
                                          int v1, int v2) {
    if (tag == "W") return std::make_shared<Weapon>(name, price, rarity, v1, v2);
    if (tag == "A") return std::make_shared<Armor>(name, price, rarity, v1);
    if (tag == "P") return std::make_shared<Potion>(name, price, rarity, v1);
    if (tag == "M") return std::make_shared<Money>(name, price, rarity, v1);
    return nullptr;
}

std::shared_ptr<Item> ItemFactory::randomLoot(int playerLevel) {
    // 品质概率：普通60%、精良28%、稀有10%、传说2%
    Rarity r;
    int rp = Utils::randInt(1, 100);
    if (rp <= 60)       r = Rarity::COMMON;
    else if (rp <= 88)  r = Rarity::FINE;
    else if (rp <= 98)  r = Rarity::RARE;
    else                r = Rarity::LEGENDARY;

    // 数值基准随玩家等级成长
    int base = 5 + playerLevel * 3;
    double mult = 1.0;
    switch (r) {
        case Rarity::COMMON:   mult = 0.8;  break;
        case Rarity::FINE:     mult = 1.0;  break;
        case Rarity::RARE:     mult = 1.4;  break;
        case Rarity::LEGENDARY:mult = 2.0;  break;
    }

    // 武器名库
    static const std::vector<std::string> swordNames = {
        "铁剑", "青锋剑", "碧血剑", "倚天残刃"
    };
    static const std::vector<std::string> axeNames = {
        "短斧", "宣花斧", "开山斧", "刑天巨斧"
    };
    static const std::vector<std::string> bowNames = {
        "木弓", "铁胎弓", "玄铁弓", "落日神弓"
    };
    static const std::vector<std::string> armorNames = {
        "布衣", "皮甲", "锁子甲", "玄铁重甲"
    };

    int roll = Utils::randInt(1, 4);
    if (roll <= 3) {  // 武器（剑/斧/弓）
        const std::vector<std::string>* names = &swordNames;
        if (roll == 2) names = &axeNames;
        if (roll == 3) names = &bowNames;
        int idx = (r == Rarity::LEGENDARY) ? 3 : Utils::randInt(0, 2);
        int atk = static_cast<int>(base * mult) + Utils::randInt(1, 4);
        int spd = Utils::randInt(0, 2);
        int price = static_cast<int>(atk * 4);
        return std::make_shared<Weapon>((*names)[idx], price, r, atk, spd);
    } else {  // 盔甲
        int idx = (r == Rarity::LEGENDARY) ? 3 : Utils::randInt(0, 2);
        int def = static_cast<int>(base * 0.7 * mult) + Utils::randInt(1, 3);
        int price = static_cast<int>(def * 4);
        return std::make_shared<Armor>(armorNames[idx], price, r, def);
    }
}

std::shared_ptr<Item> ItemFactory::makePotion(int level) {
    int heal = 30 + level * 15;
    int price = 10 + level * 5;
    return std::make_shared<Potion>("金疮药", price, Rarity::COMMON, heal);
}
