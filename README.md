# SrvProj

`SrvProj` 是一个使用 C++ 编写的游戏服务端模拟器，目标是模拟《Stella Sora / Stargazer》客户端所需的登录服、HTTP 路由、加密协议、资源表加载、会话管理、玩家存档与游戏数据返回。

## 项目结构

```text
SrvProj/
├── SrvProj.slnx                        # Visual Studio 解决方案
├── vcpkg.json                          # vcpkg 依赖清单
├── vcpkg-configuration.json            # vcpkg 配置
├── README.md
├── SrvProj/                            # 主 C++ 服务端工程
│   ├── SrvProj.cpp                     # 程序入口 main()
│   ├── AccountServer.*                 # 手写 HTTP/TCP 服务器与路由分发
│   ├── AccountController.*             # 登录服/平台服 HTTP 路由处理
│   ├── GatewayController.*             # 游戏协议入口与包分发
│   ├── GameServices.*                  # 全局游戏服务，会话表
│   ├── GameSession.*                   # 单个客户端会话、密钥、玩家对象
│   ├── DbMgr.*                         # SQLite 账号和玩家存档
│   ├── AeadTool.*                      # AEAD、ECDH、AES/ChaCha 加解密工具
│   ├── Config.*                        # Config.json 配置读写
│   ├── Logger.*                        # 日志系统
│   ├── HttpMessage.*                   # HTTP 请求/响应结构与解析辅助
│   ├── GameTime.*                      # 游戏时间/服务器时间
│   ├── GameConstants.h                 # 逆向得到的游戏常量
│   ├── ResultCode.h                    # 错误码
│   ├── Util.*                          # 编码转换等工具
│   ├── AsyncTask.h / IOCPAwaiter.*     # IOCP 异步框架
│   ├── HttpClient.*                    # HTTP 客户端
│   ├── Command/                        # 控制台命令行
│   ├── Game/                           # 玩家与游戏子模块
│   │   ├── ManagerBase.*               # 玩家子模块基类
│   │   ├── Player.*                    # 玩家主体
│   │   ├── CharacterMgr.*              # 角色/光盘（Phone 复用其联系人存档）
│   │   ├── InventoryMgr.*              # 背包
│   │   ├── MallMgr.*                   # 商城
│   │   ├── GachaMgr.*                  # 抽卡
│   │   ├── MailMgr.*                   # 邮件
│   │   ├── QuestMgr.*                  # 任务
│   │   ├── ActivityMgr.*               # 活动
│   │   ├── BattlePassMgr.*             # 战斗通行证
│   │   ├── AchievementMgr.*            # 成就
│   │   ├── StoryMgr.*                  # 剧情
│   │   ├── AgentMgr.*                  # 每日委托
│   │   ├── FormationMgr.*              # 编队
│   │   └── StarTower*                  # 星塔/爬塔玩法
│   ├── Handlers/                       # 游戏协议 Handler
│   ├── Resources/                      # 资源表加载和资源类
│   └── proto/                          # protobuf 定义与生成代码
└── csarcx/                             # arcx 归档读取/资源工具相关工程
```

## 编译与运行

工程使用 Visual Studio / MSBuild，依赖 vcpkg（SQLite3、OpenSSL、protobuf、nlohmann-json、cpprestsdk）。

编译（PowerShell，必须使用此命令）：

```powershell
cmd /c "\"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat\" x64_x86 && msbuild ./SrvProj.slnx /p:Configuration=Debug /p:Platform=x86"
```

运行还需要资源文件，在 `Config.json` 中选择 `arcx` 资源（`data.arcx`）或 `json` 资源目录（`bin/`），缺少资源会启动失败。

运行：从 `SrvProj/Debug/` 目录启动 `SrvProj.exe`，服务将监听 `0.0.0.0:21000`，`GET /` 返回 HTTP 200。

## 配置

`Config.json` 可配置：HTTP 服务 IP/端口/线程数、资源加载类型与路径（`arcx` 或 `json`）、数据库路径、时区、服务器时间伪造、默认权限、是否解锁全部星塔/副本/剧情 CG。

## 当前已实现玩法

以下玩法系统均已落地最小可玩流程，可在本地 1.13 客户端进服体验：

- **账号与登录**：账号注册、UID/Token 登录、首次创角与存档恢复。
- **基础资料**：昵称、签名、性别、头像、皮肤、称号前缀/后缀、看板、世界等级与经验、体力与恢复。
- **角色**：拥有、升级、Affinity、宝石镶嵌、宝石副本、角色约会、预设改名。
- **光盘（Disc）**：拥有、强化、突破、套装效果；Phone 联系人复用其存档。
- **背包**：道具拥有、堆叠、使用、礼包开包。
- **商城**：礼包列表、月卡、商店、订单、限时购买条件、玩家订单进度。
- **抽卡**：普通池、新手池、保底计数。
- **任务**：每日任务、每周任务、活动任务、任务进度、任务奖励。
- **活动**：活动列表、登录奖励、周期奖励、商店、剧情、关卡、挖矿、小游戏、CG 阅读。
- **战斗通行证**：通行证经验、等级、阶段奖励。
- **成就**：进度、达成、解锁触发器（含星塔成就、连续登录等）。
- **剧情**：剧情 CG、剧情阅读进度、剧情奖励、NPC 剧情奖励。
- **邮件**：邮件列表、领取附件、过期清理。
- **委托（Agent）**：每日委托、申请、奖励、刷新。
- **编队**：编队槽位、激活/编辑、首发角色、支援角色。
- **星塔（StarTower）**：建层、申请、交互、战斗结算、潜能选择（普通/特殊/稀有）、稀有度提升、副音符掉落、NPC 事件、强化机、商人 reroll、回血房、HP 同步、Build 存档、图鉴收集、fate 卡、局内结算与重连。
- **NPC 好感图鉴**：NPC 列表、亲和度、事件解锁、奖励记录。
