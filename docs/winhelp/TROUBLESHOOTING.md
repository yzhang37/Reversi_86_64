# WinHelp 4.0 Troubleshooting

本文记录本项目今天遇到过、确认过的 WinHelp 4.0 问题。调试时先看这里。

## 不要用系统 winhlp32.exe 调试

本仓库根目录有可用的本地工具：

```text
WINHLP95.EXE
winhlp32.exe
```

需要模拟 Win95 行为时，优先使用仓库里的 `WINHLP95.EXE`。不要无意中调用 `C:\Windows\winhlp32.exe`。

## GID 缓存

WinHelp 会生成 `.GID` 缓存，例如：

```text
REVERSI.GID
help/zh-CN/REVERSI.GID
build/x86/REVERSI.GID
```

现象：

- 修改 HLP 后，索引或目录仍像旧版本。
- ALink 调试结果前后不一致。
- no-CNT fallback 行为看起来没有刷新。

处理：

```powershell
Remove-Item .\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\help\zh-CN\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x86\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x64\REVERSI.GID -Force -ErrorAction SilentlyContinue
```

不要提交 `.GID`。

## 1032 错误

截图中的 1032 通常表示 WinHelp 找不到或无法打开帮助主题列表。

常见原因：

- 没有 `.CNT`，但 HLP 也没有有效 `CONTENTS` fallback topic。
- 程序从“帮助 > 目录”调用了不合适的命令。
- `.CNT` 和 `.HLP` 没有放在同一目录。
- `.CNT` 的 `:Base` 指向不对。

本项目处理方式：

- HPJ 设置 `CONTENTS=JP.RLA`。
- RTF 生成 fallback topic `JP.RLA`。
- HPJ 设置 `CNT=REVERSI.cnt`。
- 程序打开目录时使用 `HELP_FINDER`。

fallback topic 内容：

```text
单击“帮助主题”返回到主题列表。
```

## 没有 CNT 时出现空窗口

没有 `.CNT` 时，WinHelp 可能打开一个空白主窗口，同时弹出索引窗口。这不是理想行为。

更接近 Notepad 的做法：

- `CONTENTS` 指向一个很小的 topic。
- topic 只显示“单击‘帮助主题’返回到主题列表。”。
- topic 进入时可以执行 `NS();FD()`，让用户看到主题列表。

这个 fallback 不是正常使用路径。正常发布时仍然应该把 `.CNT` 放在 `.HLP` 旁边。

## Windows 95 上 AL 报 141

141 表示没有可用的附加信息或找不到相关主题。

本项目确认过一个隐蔽原因：

- 当前目录的 `REVERSI.HLP` 没问题。
- 仓库里的 `WINHLP95.EXE` 在 Win11 上打开同一份 HLP 也没问题。
- 但真实 Win95 上 `AL()` 报 141。
- 最终发现 `C:\WINDOWS` 或 `C:\WINDOWS\HELP` 中有另一个同名 `REVERSI.HLP`。
- WinHelp 查找 A-keyword 时受同名 help family 影响，解析到了错误文件。

排查顺序：

1. 删除同目录 `.GID`。
2. 确认 `.HLP` 和 `.CNT` 同目录。
3. 搜索 `C:\WINDOWS` 和 `C:\WINDOWS\HELP` 中是否有同名 `REVERSI.HLP`。
4. 再检查 RTF 里的 `A` footnote 和 `AL()` key 是否一致。

## 索引乱码

可能原因：

- RTF 没有使用 `\ansicpg936`。
- 字体表没有 `\fcharset134`。
- HPJ 默认字体不对。
- 帮助编译器或反编译器显示方式误导。

本项目选择：

- RTF 文本用 CP936 转义。
- RTF 头使用 `\ansicpg936` 和 `\fcharset134`。
- HPJ 使用 `DEFFONT=宋体,9,134`。

Notepad 的历史 HLP 有混合迹象：关键词是 GBK，但 raw SYSTEM record 里有别的 charset 记录。不要照搬它的所有字节。

## 正文全部变粗

原因：

- 标题或小节标题使用 `\b` 后没有恢复普通样式。

处理：

- 每个标题结束后追加 `\plain\f1\fs18`。
- 正文段落从 `\pard\plain\f1\fs18` 开始。

## 目录层级不对

WinHelp `.CNT` 的书节点和 topic 节点是两回事。

如果希望目录像这样：

```text
翻转棋
  “翻转棋”概述
  玩翻转棋
    “玩翻转棋”概述
```

需要写：

```text
1 翻转棋
2 “翻转棋”概述=TOPIC_OVERVIEW
2 玩翻转棋
3 “玩翻转棋”概述=TOPIC_PLAY
```

不要把顶级「翻转棋」直接写成唯一 topic，否则目录会失去 Windows 帮助那种书本层级。

## 调试原版帮助

可参考的原版文件：

- `NOTEPAD.HLP` / `NOTEPAD.CNT`
- `WINMINE.HLP` / `WINMINE.CNT`
- `FREECELL.HLP`
- `SOL.HLP` / `SOL.CNT`

注意：

- 反编译输出只能作为线索。
- WinHelp 运行时行为要用真实 WinHelp 测。
- `.GID` 会污染测试结果。
