# 《剑指苍穹》1单机版文字 MUD 游戏

程序设计基础实践课程设计 —— 面向对象软件开发（C++17）

## 一、游戏简介

玩家创建角色（剑客 / 刺客 / 道士），从新手村出发，通过房间地图探索，以回合制战斗击败各区域敌人与 Boss，随机掉落装备强化属性，历经五大关卡最终讨伐魔教教主。

- 五大关卡：新手村 → 迷雾森林 → 黑风寨 → 幽暗古墓 → 魔教总坛
- 回合制战斗：攻击 / 防御 / 用药 / 逃跑，含命中、闪避、暴击、伤害吸收
- 装备系统：武器 / 盔甲 / 药水，品质分普通 / 精良 / 稀有 / 传说，打怪随机掉落
- 养成系统：经验升级、自由属性点加点（力量 / 健康 / 敏捷）
- 商店系统：药铺、铁匠铺 NPC，买卖交易
- 存档系统：随时 save，退出后读档继续
- 中 / 英文双命令，彩色字符界面

## 二、在 Linux 虚拟机中运行

```bash
# 进入源码目录
cd mud_game

# 编译（需 g++，支持 C++17）
make

# 运行
./mud
# 或一步到位
make run

# 清理编译产物
make clean
```

## 三、在 Windows 系统中运行

本游戏已在 `main.cpp` 中做好 Windows 控制台兼容（自动启用 ANSI 彩色 + UTF-8 中文），可直接在 Windows 编译运行。

**方式一：直接运行已编译好的 `mud.exe`**

把 `mud.exe` 复制到任意目录，双击或在 cmd / PowerShell 中运行即可。

**方式二：使用一键编译脚本 `build_windows.bat`**

```bat
build_windows.bat
```

编译成功后在当前目录生成 `mud.exe`，运行 `mud.exe` 即可。

**方式三：手动编译（需 MinGW-w64 / w64devkit）**

```bat
:: 将 w64devkit\w64devkit\bin 加入 PATH 后执行：
g++ -std=c++17 -Wall -Wextra -O2 main.cpp item.cpp creature.cpp room.cpp world.cpp game.cpp -o mud.exe
```

> 提示：Windows 下建议使用 Windows Terminal（Win11 默认终端）或较新版本的 cmd 以获得最佳彩色显示效果。

## 四、快速上手

```
1. 主菜单选 1 开始新游戏
2. 输入姓名，选择职业（1 剑客 / 2 刺客 / 3 道士）
3. 新手村内：west/w 去铁匠铺（list / buy 编号购买），药铺买药水
4. east/e 出村到练武场，attack <编号> 战斗，输入 1 攻击（2 防御 / 3 用药 / 4 逃跑）
5. 生命不足可用 rest 休息，或 use <编号> 用药；打不过先练级、加点（train 力量）
6. 击败区域 Boss（★）后输入 next 进入下一区域
7. 随时 save 存档；quit 退出，主菜单选 2 读档
```

## 五、文件结构

| 文件 | 说明 |
|---|---|
| `types.h` | 公共类型、颜色、随机与字符串工具 |
| `item.h/.cpp` | 物品系统（基类 + 武器/盔甲/药水/金钱 + 工厂） |
| `creature.h/.cpp` | 生物系统（基类 + 玩家 + 敌人 + NPC） |
| `room.h/.cpp` | 房间与区域系统 |
| `world.h/.cpp` | 世界地图（五大关卡配置） |
| `game.h/.cpp` | 游戏引擎（战斗、存档、命令、主循环） |
| `main.cpp` | 程序入口（含 Windows 控制台兼容初始化） |
| `Makefile` | Linux 编译脚本 |
| `build_windows.bat` | Windows 一键编译脚本 |
| `课程设计文档.md` | 完整课程设计文档（WBS / 用例图 / UML / 流程图 / STL / 设计模式） |

## 六、Git 上传到 GitHub

本仓库已托管到 GitHub：`https://github.com/GJF080117/gjf`（默认展示分支 `main`，`master` 同步更新）。

修改代码后，按下面三步即可更新到 GitHub：

```bash
git add -A                 # 1. 暂存所有改动
git commit -m "本次更新说明" # 2. 提交（写清楚改了什么）
git push                   # 3. 推送到 GitHub
```

> 提示：
> - `.gitignore` 已自动排除 `save.dat`（游戏存档）与 `mud.exe` / `*.o`（编译产物），不会被误传。
> - 远程 `main` 与 `master` 默认保持一致；如需把 `master` 的更新同步到默认展示分支 `main`，执行 `git push origin master:main`。
> - 首次推送需登录 GitHub（弹窗输入账号密码或 Personal Access Token）。

## 七、备注

- 仅依赖标准 C++17 库与 ANSI 转义色码，Linux / macOS 终端直接支持；Windows 由 `main.cpp` 自动启用 ANSI 与 UTF-8，跨平台无第三方依赖。
- 存档文件 `save.dat` 生成于运行目录。
