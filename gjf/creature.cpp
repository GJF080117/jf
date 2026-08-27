// ============================================================
// creature.cpp  生物系统的实现
// ============================================================
#include "creature.h"

// ---------- Creature 基类 ----------
Creature::Creature(const std::string& name, int level)
    : name_(name), level_(level) {}

void Creature::takeDamage(int dmg) {
    curHp_ -= dmg;
    if (curHp_ < 0) curHp_ = 0;
}

int Creature::getStrikeDamage() const {
    // 撞击伤害 = 基础攻击 + 力量 × 系数
    return baseAttack_ + strength_ * 2;
}

int Creature::getAccuracy() const {
    // 命中率随等级与敏捷提升，封顶 95
    return std::min(95, 70 + agility_ + level_ * 2);
}

int Creature::getDodge() const {
    // 闪避率 = 敏捷系数，封顶 60
    return std::min(60, agility_ / 2 + level_);
}

int Creature::getAbsorption() const {
    // 伤害吸收 = 基础吸收 + 健康 × 系数
    return baseAbsorb_ + health_ / 2;
}

int Creature::getSpeed() const {
    // 速度 = 敏捷 + 等级
    return agility_ + level_;
}

// ---------- Player ----------
Player::Player(const std::string& name, const std::string& profession)
    : Creature(name, 1), profession_(profession) {
    if (profession_ == "剑客") {
        // 剑客：力量与健康占优，攻守兼备
        strength_ = 14; health_ = 15; agility_ = 9;
        baseAttack_ = 12; baseAbsorb_ = 2;
    } else if (profession_ == "刺客") {
        // 刺客：敏捷占优，高闪避高速度，生命较低
        strength_ = 11; health_ = 9; agility_ = 16;
        baseAttack_ = 10; baseAbsorb_ = 0;
    } else { // 道士
        // 道士：均衡，属性点成长快，靠装备与药水续航
        strength_ = 10; health_ = 12; agility_ = 12;
        baseAttack_ = 9; baseAbsorb_ = 1;
    }
    maxHp_ = 80 + health_ * 8;
    curHp_ = maxHp_;
}

void Player::gainExp(int amount) {
    exp_ += amount;
    while (exp_ >= expToLevel_) {
        exp_ -= expToLevel_;
        levelUp();
    }
}

void Player::levelUp() {
    ++level_;
    expToLevel_ = 100 + (level_ - 1) * 60;   // 与初始公式一致
    // 升级成长：核心属性 +1，自由属性点 +3
    strength_ += 1;
    health_ += 1;
    agility_ += 1;
    attrPoints_ += 3;
    maxHp_ = 80 + health_ * 8;
    curHp_ = maxHp_;  // 升级回满
}

bool Player::spendAttrPoint(const std::string& attr) {
    if (attrPoints_ <= 0) return false;
    if (attr == "力量") strength_++;
    else if (attr == "健康") {
        health_++;
        maxHp_ = 80 + health_ * 8;
        curHp_ = std::min(maxHp_, curHp_ + 8);
    }
    else if (attr == "敏捷") agility_++;
    else return false;
    --attrPoints_;
    return true;
}

void Player::restoreSave(int level, int exp, int money, int attr,
                         int str, int hel, int agi, int hp) {
    level_ = level;
    exp_ = exp;
    expToLevel_ = 100 + (level - 1) * 60;
    money_ = money;
    attrPoints_ = attr;
    strength_ = str;
    health_ = hel;
    agility_ = agi;
    maxHp_ = 80 + hel * 8;
    curHp_ = std::min(hp, maxHp_);
}

bool Player::removeItemAt(size_t idx) {
    if (idx >= inventory_.size()) return false;
    inventory_.erase(inventory_.begin() + idx);
    return true;
}

bool Player::equipItem(size_t idx) {
    if (idx >= inventory_.size()) return false;
    const auto& it = inventory_[idx];
    if (it->getType() == ItemType::WEAPON) {
        auto w = std::dynamic_pointer_cast<Weapon>(it);
        if (!w) return false;
        if (weapon_) inventory_.push_back(weapon_);  // 旧武器放回背包
        weapon_ = w;
    } else if (it->getType() == ItemType::ARMOR) {
        auto a = std::dynamic_pointer_cast<Armor>(it);
        if (!a) return false;
        if (armor_) inventory_.push_back(armor_);
        armor_ = a;
    } else {
        return false;
    }
    inventory_.erase(inventory_.begin() + idx);
    return true;
}

bool Player::unequipWeapon() {
    if (!weapon_) return false;
    inventory_.push_back(weapon_);
    weapon_.reset();
    return true;
}

bool Player::unequipArmor() {
    if (!armor_) return false;
    inventory_.push_back(armor_);
    armor_.reset();
    return true;
}

bool Player::usePotion(size_t idx) {
    if (idx >= inventory_.size()) return false;
    auto p = std::dynamic_pointer_cast<Potion>(inventory_[idx]);
    if (!p) return false;
    heal(p->getHeal());
    inventory_.erase(inventory_.begin() + idx);
    return true;
}

void Player::dropItem(size_t idx) {
    if (idx >= inventory_.size()) return;
    inventory_.erase(inventory_.begin() + idx);
}

int Player::getStrikeDamage() const {
    int weaponAtk = weapon_ ? weapon_->getAttack() : 0;
    return baseAttack_ + strength_ * 2 + weaponAtk;
}

int Player::getDodge() const {
    // 刺客更擅长闪避
    int bonus = (profession_ == "刺客") ? 6 : 0;
    return std::min(65, Creature::getDodge() + bonus);
}

int Player::getAbsorption() const {
    int armorDef = armor_ ? armor_->getDefense() : 0;
    return Creature::getAbsorption() + armorDef;
}

int Player::getSpeed() const {
    int weaponSpd = weapon_ ? weapon_->getSpeed() : 0;
    return Creature::getSpeed() + weaponSpd;
}

std::string Player::statusText() const {
    std::ostringstream oss;
    oss << Color::BOLD << "── 角色状态 ──" << Color::RESET << "\n";
    oss << "姓名    ：" << name_ << "（" << profession_ << "） 等级 " << level_ << "\n";
    oss << "生命    ：" << Color::GREEN << curHp_ << "/" << maxHp_ << Color::RESET << "\n";
    oss << "经验    ：" << exp_ << " / " << expToLevel_
        << "（剩余自由属性点：" << attrPoints_ << "）\n";
    oss << "金钱    ：" << Color::YELLOW << money_ << Color::RESET << " 两\n";
    oss << "属性    ：力量 " << strength_ << " / 健康 " << health_
        << " / 敏捷 " << agility_ << "\n";
    oss << "命中    ：" << getAccuracy() << "%   闪避：" << getDodge() << "%\n";
    oss << "攻击伤害：" << getStrikeDamage()
        << "   伤害吸收：" << getAbsorption()
        << "   速度：" << getSpeed() << "\n";
    oss << "武器    ：" << (weapon_ ? weapon_->getName() : "（空）") << "\n";
    oss << "盔甲    ：" << (armor_ ? armor_->getName() : "（空）");
    return oss.str();
}

// ---------- Enemy ----------
Enemy::Enemy(const std::string& name, int level,
             int strength, int health, int agility,
             int baseAttack, int baseAbsorb,
             int expReward, int moneyLo, int moneyHi,
             int dropChance, bool isBoss)
    : Creature(name, level),
      expReward_(expReward), moneyLo_(moneyLo), moneyHi_(moneyHi),
      dropChance_(dropChance), isBoss_(isBoss) {
    strength_ = strength;
    health_ = health;
    agility_ = agility;
    baseAttack_ = baseAttack;
    baseAbsorb_ = baseAbsorb;
    maxHp_ = 60 + health_ * 7;
    curHp_ = maxHp_;
    initStrength_ = strength;
    initHealth_ = health;
    initAgility_ = agility;
    initBaseAttack_ = baseAttack;
    initBaseAbsorb_ = baseAbsorb;
}

std::shared_ptr<Enemy> Enemy::clone() const {
    return std::make_shared<Enemy>(name_, level_,
                                   initStrength_, initHealth_, initAgility_,
                                   initBaseAttack_, initBaseAbsorb_,
                                   expReward_, moneyLo_, moneyHi_,
                                   dropChance_, isBoss_);
}

// ---------- NPC ----------
NPC::NPC(const std::string& name, const std::string& title,
         const std::string& greeting)
    : name_(name), title_(title), greeting_(greeting) {}
