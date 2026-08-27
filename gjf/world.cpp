// ============================================================
// world.cpp  世界地图构建：五大区域关卡
// 每个区域是一条线性的房间链：入口房 → 探索房 → Boss房
// ============================================================
#include "world.h"

World::World() { build(); }

void World::build() {
    // ============ 第 1 关：新手村（建议 1-3 级） ============
    {
        auto area = std::make_shared<Area>(0, "新手村",
                                           "建议等级：1-3 级");
        area->setEntryDesc("这里是宁静祥和的桃源村，村口的老树见证了一代代少侠的成长。"
                           "村中有药店和铁匠铺，可以补给装备。村外野狗出没，山贼在村外小路徘徊。");

        // 入口广场（有药店 NPC）
        auto r0 = std::make_shared<Room>(0, "村口广场",
            "青石板铺就的广场，人来人往。北边是药铺，西边传来叮叮当当的打铁声，东边通向村外。");
        auto potionNpc = std::make_shared<NPC>("孙掌柜", "药铺掌柜",
            "少侠，金疮药保命要紧，来一瓶吧？");
        potionNpc->addStock(ItemFactory::makePotion(1));
        potionNpc->addStock(ItemFactory::makePotion(2));
        potionNpc->addStock(ItemFactory::makePotion(3));
        r0->setNpc(potionNpc, ShopType::POTION_SHOP);

        // 铁匠铺（武器店 NPC）
        auto r1 = std::make_shared<Room>(1, "村中铁匠铺",
            "炉火正旺，墙上挂满了刀剑。铁匠大叔见你进来，热情地招呼。");
        auto blacksmith = std::make_shared<NPC>("铁匠老周", "铁匠铺老板",
            "小兄弟，选把趁手的兵器再上路吧！");
        blacksmith->addStock(ItemFactory::create("W", "铁剑", 40, Rarity::COMMON, 6, 0));
        blacksmith->addStock(ItemFactory::create("W", "短斧", 55, Rarity::COMMON, 8, -1));
        blacksmith->addStock(ItemFactory::create("W", "木弓", 70, Rarity::FINE, 9, 2));
        blacksmith->addStock(ItemFactory::create("A", "布衣", 30, Rarity::COMMON, 4));
        blacksmith->addStock(ItemFactory::create("A", "皮甲", 80, Rarity::FINE, 7));
        r1->setNpc(blacksmith, ShopType::WEAPON_SHOP);

        // 练武场（野狗）
        auto r2 = std::make_shared<Room>(2, "村外练武场",
            "平整的黄土场，几只野狗在树下徘徊，见人就吠。");
        r2->addEnemy(std::make_shared<Enemy>("野狗", 1, 8, 9, 12,
                                             8, 0, 20, 3, 8, 40, false));
        r2->addEnemy(std::make_shared<Enemy>("野狗", 1, 8, 9, 12,
                                             8, 0, 20, 3, 8, 40, false));

        // 村外小路（野猪、山贼喽啰）
        auto r3 = std::make_shared<Room>(3, "村外小路",
            "蜿蜒的土路通向远方，野猪在路边拱食，远处似乎有山贼的身影。");
        r3->addEnemy(std::make_shared<Enemy>("野猪", 2, 12, 14, 8,
                                             11, 1, 35, 6, 15, 45, false));
        r3->addEnemy(std::make_shared<Enemy>("山贼喽啰", 3, 13, 12, 10,
                                             12, 1, 50, 10, 25, 50, false));

        // 山贼营地（Boss）
        auto r4 = std::make_shared<Room>(4, "山贼营地",
            "营地中央立着一面破旗，山贼头目正坐在虎皮椅上大口喝酒。");
        r4->addEnemy(std::make_shared<Enemy>("山贼喽啰", 3, 13, 12, 10,
                                             12, 1, 45, 8, 20, 50, false));
        r4->addEnemy(std::make_shared<Enemy>("山贼头目", 4, 17, 16, 11,
                                             15, 3, 120, 30, 60, 100, true));

        // 连接：广场(0) ↔ 铁匠铺(1)；广场(0) → 练武场(2) → 小路(3) → 营地(4)
        r0->setExit('w', r1);
        r1->setExit('e', r0);
        r0->setExit('e', r2);
        r2->setExit('w', r0);
        r2->setExit('e', r3);
        r3->setExit('w', r2);
        r3->setExit('e', r4);
        r4->setExit('w', r3);

        area->addRoom(r0);
        area->addRoom(r1);
        area->addRoom(r2);
        area->addRoom(r3);
        area->addRoom(r4);
        areas_.push_back(area);
    }

    // ============ 第 2 关：迷雾森林（建议 4-6 级） ============
    {
        auto area = std::make_shared<Area>(1, "迷雾森林",
                                           "建议等级：4-6 级");
        area->setEntryDesc("幽暗的森林被浓雾笼罩，林中传来低沉的兽吼。"
                           "传说森林深处住着凶猛的狼王，守护着通往黑风寨的密道。");

        auto r0 = std::make_shared<Room>(0, "林间入口",
            "参天古木遮天蔽日，雾气在林间缓缓流动，几缕阳光艰难地透过树冠。");
        auto r1 = std::make_shared<Room>(1, "落叶小径",
            "厚厚的落叶铺满小径，踩上去沙沙作响。灰狼的绿色眼睛在暗处闪烁。");
        r1->addEnemy(std::make_shared<Enemy>("灰狼", 4, 15, 13, 14,
                                             14, 2, 60, 15, 35, 50, false));
        r1->addEnemy(std::make_shared<Enemy>("灰狼", 4, 15, 13, 14,
                                             14, 2, 60, 15, 35, 50, false));

        auto r2 = std::make_shared<Room>(2, "毒瘴沼泽",
            "泥泞的沼泽散发着腐臭，毒蛇吐着信子潜伏在草丛中。");
        r2->addEnemy(std::make_shared<Enemy>("毒蛇", 5, 13, 10, 18,
                                             16, 1, 80, 20, 40, 55, false));
        r2->addEnemy(std::make_shared<Enemy>("森林狼王", 6, 18, 17, 15,
                                             18, 4, 110, 25, 50, 55, false));

        auto r3 = std::make_shared<Room>(3, "狼穴深处",
            "巨大的狼穴中白骨累累，狼王伏卧在石台上，缓缓睁开了猩红的双眼。");
        r3->addEnemy(std::make_shared<Enemy>("迷雾狼王", 6, 21, 20, 17,
                                             20, 6, 260, 60, 110, 100, true));

        // 连接
        r0->setExit('e', r1);
        r1->setExit('w', r0);
        r1->setExit('e', r2);
        r2->setExit('w', r1);
        r2->setExit('e', r3);
        r3->setExit('w', r2);

        area->addRoom(r0);
        area->addRoom(r1);
        area->addRoom(r2);
        area->addRoom(r3);
        areas_.push_back(area);
    }

    // ============ 第 3 关：黑风寨（建议 7-9 级） ============
    {
        auto area = std::make_shared<Area>(2, "黑风寨",
                                           "建议等级：7-9 级");
        area->setEntryDesc("盘踞山头的黑风寨，寨墙高筑，戒备森严。"
                           "寨主黑旋风凶名在外，是附近百姓的心头大患。");

        auto r0 = std::make_shared<Room>(0, "山寨大门",
            "沉重的木门上钉着铁皮，两名喽啰在门口巡逻。");
        auto r1 = std::make_shared<Room>(1, "演武场",
            "寨中喽啰在此操练，刀光剑影，喊杀声不绝。");
        r1->addEnemy(std::make_shared<Enemy>("山寨喽啰", 7, 19, 18, 12,
                                             20, 4, 100, 30, 55, 55, false));
        r1->addEnemy(std::make_shared<Enemy>("寨中刀客", 7, 21, 17, 14,
                                             22, 3, 110, 32, 60, 55, false));
        auto r2 = std::make_shared<Room>(2, "聚义厅前院",
            "院内火把通明，山寨精英持刀而立，等待闯寨者。");
        r2->addEnemy(std::make_shared<Enemy>("山寨精英", 8, 23, 20, 15,
                                             24, 6, 140, 40, 75, 60, false));
        r2->addEnemy(std::make_shared<Enemy>("山寨精英", 8, 23, 20, 15,
                                             24, 6, 140, 40, 75, 60, false));
        auto r3 = std::make_shared<Room>(3, "聚义厅",
            "大厅正中，黑风寨主黑旋风端坐交椅，一手按在鬼头刀上。");
        r3->addEnemy(std::make_shared<Enemy>("黑风寨主·黑旋风", 9, 27, 25, 18,
                                             28, 9, 480, 110, 200, 100, true));

        r0->setExit('n', r1);
        r1->setExit('s', r0);
        r1->setExit('n', r2);
        r2->setExit('s', r1);
        r2->setExit('n', r3);
        r3->setExit('s', r2);

        area->addRoom(r0);
        area->addRoom(r1);
        area->addRoom(r2);
        area->addRoom(r3);
        areas_.push_back(area);
    }

    // ============ 第 4 关：幽暗古墓（建议 10-12 级） ============
    {
        auto area = std::make_shared<Area>(3, "幽暗古墓",
                                           "建议等级：10-12 级");
        area->setEntryDesc("地下古墓阴风阵阵，墙上火把忽明忽暗。"
                           "传说墓中葬着一位将军，连同他的宝物一同长眠于此。");

        auto r0 = std::make_shared<Room>(0, "墓道入口",
            "甬道两侧的石壁上刻着古老的符文，寒气从深处涌出。");
        auto r1 = std::make_shared<Room>(1, "陪葬石室",
            "石室内摆着陶俑与棺椁，几具骷髅缓缓立起，发出咔咔的声响。");
        r1->addEnemy(std::make_shared<Enemy>("骷髅兵", 10, 24, 20, 13,
                                             26, 7, 150, 45, 85, 60, false));
        r1->addEnemy(std::make_shared<Enemy>("骷髅兵", 10, 24, 20, 13,
                                             26, 7, 150, 45, 85, 60, false));
        auto r2 = std::make_shared<Room>(2, "阴魂长廊",
            "长廊尽头，亡灵法师吟唱着晦涩的咒语，幽蓝的鬼火悬浮四周。");
        r2->addEnemy(std::make_shared<Enemy>("亡灵法师", 11, 22, 19, 20,
                                             28, 5, 200, 55, 100, 60, false));
        auto r3 = std::make_shared<Room>(3, "将军主墓室",
            "巨大的石棺缓缓打开，骷髅将军身穿残破铠甲，拔出锈剑。");
        r3->addEnemy(std::make_shared<Enemy>("古墓守卫", 11, 26, 24, 16,
                                             30, 10, 220, 60, 110, 60, false));
        r3->addEnemy(std::make_shared<Enemy>("骷髅将军", 12, 32, 30, 19,
                                             34, 13, 700, 160, 300, 100, true));

        r0->setExit('s', r1);
        r1->setExit('n', r0);
        r1->setExit('s', r2);
        r2->setExit('n', r1);
        r2->setExit('s', r3);
        r3->setExit('n', r2);

        area->addRoom(r0);
        area->addRoom(r1);
        area->addRoom(r2);
        area->addRoom(r3);
        areas_.push_back(area);
    }

    // ============ 第 5 关：魔教总坛（建议 13-15 级，最终关） ============
    {
        auto area = std::make_shared<Area>(4, "魔教总坛",
                                           "建议等级：13-15 级");
        area->setEntryDesc("魔教总坛建在孤峰之巅，黑云压顶。"
                           "这是最后的决战之地，击败教主，还天下太平。");

        auto r0 = std::make_shared<Room>(0, "总坛山门",
            "黑石砌成的山门高大狰狞，两名魔教弟子持刀而立。");
        auto r1 = std::make_shared<Room>(1, "魔教大殿",
            "大殿内烛火幽暗，魔教弟子层层把守。");
        r1->addEnemy(std::make_shared<Enemy>("魔教弟子", 13, 30, 27, 18,
                                             34, 10, 260, 80, 150, 65, false));
        r1->addEnemy(std::make_shared<Enemy>("魔教护法", 13, 32, 29, 20,
                                             36, 11, 300, 90, 170, 65, false));
        auto r2 = std::make_shared<Room>(2, "四大天王殿",
            "四座狰狞神像分立两侧，殿中站着魔教四大天王中的两位。");
        r2->addEnemy(std::make_shared<Enemy>("天王·赤炎", 14, 35, 32, 21,
                                             38, 14, 420, 120, 220, 70, false));
        r2->addEnemy(std::make_shared<Enemy>("天王·玄冰", 14, 33, 34, 22,
                                             36, 16, 400, 110, 200, 70, false));
        auto r3 = std::make_shared<Room>(3, "教主宝座",
            "魔教教主一袭黑袍，负手立于宝座之前，眼中寒光如电。"
            "这是整个江湖的最后一战。");
        r3->addEnemy(std::make_shared<Enemy>("魔教教主", 15, 42, 40, 26,
                                             44, 18, 1500, 400, 800, 100, true));

        r0->setExit('n', r1);
        r1->setExit('s', r0);
        r1->setExit('n', r2);
        r2->setExit('s', r1);
        r2->setExit('n', r3);
        r3->setExit('s', r2);

        area->addRoom(r0);
        area->addRoom(r1);
        area->addRoom(r2);
        area->addRoom(r3);
        areas_.push_back(area);
    }
}
