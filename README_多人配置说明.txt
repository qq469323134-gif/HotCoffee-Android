HotCoffee v28 F2 多人物配置版（源码包）
====================================

本版本以你上传的 F2 张蓓蓓版源码为基础升级。注意：这是源码和云端编译配置，不是已经编译好的 ASI 成品。

新增功能
--------
1. 人物名单从 HotCoffee_Peds.ini 读取，不再固定写死为 7 人。
2. 每行分别填写中文显示名称和英文模型 spawn name：中文名|model_name。
3. 当前配置容量为最多 128 条有效记录（并非数学意义上的无限）；通常增加人物只需编辑 INI，不需要改 C++。
4. 菜单显示“中文名称 (英文模型名)”，F2 打开，方向键上下切换，Enter/K 确认，Esc 返回。
5. 列表超过 9 人时，菜单自动滚动显示当前选中项附近的 9 人。
6. 保留原 HotCoffee 核心流程和张蓓蓓默认条目。

如何添加人物
------------
用文本编辑器打开 HotCoffee_Peds.ini，在末尾新增一行，例如：
新人物|my_custom_ped

其中 my_custom_ped 必须是游戏实际可请求加载的模型 spawn name。先把该 Add-on Ped 正确安装到游戏中。仅添加配置行不会自动安装模型。
保存后重启 GTA5/ASI 脚本即可重新读取配置。

配置文件搜索位置（按顺序）
--------------------------
1. Mods\scripthook\scripts\HotCoffee_Peds.ini
2. scripts\HotCoffee_Peds.ini
3. HotCoffee_Peds.ini（游戏当前工作目录）
建议优先放在与 HotCoffee.asi 同一个 scripts 目录，并确保该目录可由上述路径访问。

编译（GitHub Actions）
----------------------
1. 把本压缩包解压后的全部内容上传到仓库根目录。
2. 确认 .github/workflows/main.yml 也上传了。
3. GitHub 仓库 -> Actions -> Build HotCoffee F2 Multi -> Run workflow。
4. 成功后从 Artifacts 下载 HotCoffee-F2-Multi-ASI。
5. 备份旧 ASI，再安装新的 HotCoffee_F2.asi。

重要说明
--------
- 目前仅做了源码级修改和 Linux g++ 语法检查；尚未在 Windows MinGW/GTA5 中实际编译、加载或游戏内测试。
- 你原来的 Add-on 模型必须能被游戏按 spawn name 流式加载。
- 不同 Ped 的骨架、组件和动画兼容性仍需逐个进游戏测试。
- 若 GitHub Actions 报错，请把 Build 步骤的完整错误日志截图发来，不要先覆盖成功可用的旧 ASI。
