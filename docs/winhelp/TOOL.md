# Reversi WinHelp Tooling

这里记录从 Wolai 内容生成 WinHelp 4.0 帮助文件的工具流程和 RTF 写法。

## 文件地图

- `help/zh-CN/make_hlp_sources.py`：中文帮助源生成器。页面内容、目录树、popup topic、RTF 样式都从这里生成。
- `help/zh-CN/REVERSI.rtf`：生成后的 RTF topic 源。
- `help/zh-CN/REVERSI.hpj`：Help Workshop 工程文件。
- `help/zh-CN/REVERSI.cnt`：WinHelp 目录文件。
- `help/zh-CN/REVERSI.HLP`：Help Workshop 编译结果。
- `build.ps1`：构建程序时复制 `.HLP` 和 `.CNT` 到输出目录。
- `docs/winhelp/FORMAT.md`：WinHelp 4.0 格式细节。
- `docs/winhelp/INDEXING.md`：索引和相关主题按钮。
- `docs/winhelp/TROUBLESHOOTING.md`：常见错误和 Win95 实机坑。
- `docs/winhelp/WORKFLOW.md`：推荐构建和测试流程。

## 常用命令

生成 RTF、HPJ、CNT：

```powershell
python help\zh-CN\make_hlp_sources.py
```

编译 WinHelp：

```powershell
.\tools\hcw\hcw.exe /C /E help\zh-CN\REVERSI.hpj
```

把帮助文件复制到程序旁边：

```powershell
Copy-Item help\zh-CN\REVERSI.HLP REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt REVERSI.CNT -Force
Copy-Item help\zh-CN\REVERSI.HLP build\x86\REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x86\REVERSI.CNT -Force
Copy-Item help\zh-CN\REVERSI.HLP build\x64\REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x64\REVERSI.CNT -Force
```

反编译检查：

```powershell
.\tools\helpdeco-2.1.4\helpdeco.exe help\zh-CN\REVERSI.HLP /p
```

测试前清 WinHelp 缓存：

```powershell
Remove-Item .\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\help\zh-CN\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x86\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x64\REVERSI.GID -Force -ErrorAction SilentlyContinue
```

提交前检查：

```powershell
git diff --check
git diff --cached --check
git status --short
```

## RTF 基础

中文 WinHelp RTF 头部应使用 GBK 代码页和宋体：

```rtf
{\rtf1\ansi\ansicpg936\deff0
{\fonttbl{\f0\fnil\fcharset134 \'cb\'ce\'cc\'e5;}{\f1\fnil\fcharset134 \'cb\'ce\'cc\'e5;}}
{\stylesheet{\s0\fs18 Normal;}}
```

正文用 9 号字：

```rtf
\pard\plain\f1\fs18 正文内容\par
```

标题可以加粗，但标题后必须恢复普通样式：

```rtf
\pard\plain\f1\fs24\b 页面标题\par
\pard\plain\f1\fs18 正文内容\par
```

## Topic Footnotes

普通 topic 需要 `#` footnote 作为 topic id，`$` footnote 作为标题：

```rtf
{\footnote \pard\plain # TOPIC_OVERVIEW}
{\footnote \pard\plain $ “翻转棋”概述}
```

索引词使用 `K` footnote：

```rtf
{\footnote \pard\plain K 翻转棋;游戏规则;合法走法, 夹住}
```

`AL()` 相关主题使用 `A` footnote：

```rtf
{\footnote \pard\plain A REL_PLAY}
```

## 普通跳转链接

跳到另一个 topic：

```rtf
\uldb 游戏规则\plain\f1\fs18 {\v TOPIC_RULES}
```

## Popup 上下文帮助

popup 链接使用单下划线，隐藏文本写 popup topic id：

```rtf
\ul 夹住\plain\f1\fs18 {\v POP_CLAMP}
```

popup topic 本身仍然需要 `#` 和 `$` footnote，但不要写进 `.cnt`。

## 相关主题菜单

WinHelp 4.0 支持类似「相关主题」弹出列表。Help Workshop 自带帮助使用 authorable button 运行 `AL()` 或 `KL()` 宏。

推荐用 `AL()`：

```rtf
\{button ,AL("REL_PLAY")\} \uldb 相关主题\plain\f1\fs18 {\v %!AL("REL_PLAY")}
```

目标 topic 加同一个 A-keyword：

```rtf
{\footnote \pard\plain A REL_PLAY}
```

多个分组可以用分号：

```rtf
\{button ,AL("REL_PLAY;REL_RULES")\} \uldb 相关主题\plain\f1\fs18 {\v %!AL("REL_PLAY;REL_RULES")}
```

注意：

- `AL()` 只找 A-keywords。
- `KL()` 只找 K-keywords。
- A-keywords 不显示在索引里，适合本项目的「相关主题」。
- 多个匹配时 WinHelp 显示 Topics Found 列表。一个匹配时可能直接跳转。
- 如果要固定显示一组选项，确保目标 topic 都有对应 A-keyword。
- Win95 报 141 时，先检查 `C:\WINDOWS` 和 `C:\WINDOWS\HELP` 是否有同名 `REVERSI.HLP`。同名 help family 会影响 ALink 解析。

## CNT 写法

`.cnt` 用书节点模拟层级。书节点本身不负责打开 topic。

```text
:Base REVERSI.HLP>proc4
:Title “翻转棋”帮助
1 翻转棋
2 “翻转棋”概述=TOPIC_OVERVIEW
2 玩翻转棋
3 “玩翻转棋”概述=TOPIC_PLAY
```

程序和帮助文件目录里必须同时有：

```text
REVERSI.HLP
REVERSI.CNT
```

没有 `.CNT` 时，应由 `CONTENTS=JP.RLA` 打开 fallback topic：

```text
单击“帮助主题”返回到主题列表。
```

## 程序调用

菜单「帮助 > 目录」应调用：

```c
WinHelp(hwnd, helpPath, HELP_FINDER, 0);
```

不要用旧的 `HELP_CONTENTS`。有 `.CNT` 时，`HELP_FINDER` 能打开 WinHelp 4.0 的目录/索引界面。

调试 Win95 行为时，使用仓库根目录的 `WINHLP95.EXE`，不要误调用系统目录下的 `winhlp32.exe`。

## 提交范围

推荐只提交这些 WinHelp 相关文件：

- `help/zh-CN/make_hlp_sources.py`
- `help/zh-CN/REVERSI.rtf`
- `help/zh-CN/REVERSI.hpj`
- `help/zh-CN/REVERSI.cnt`
- `help/zh-CN/REVERSI.HLP`
- `REVERSI.HLP`
- `REVERSI.CNT`
- `build.ps1`
- `src/reversi_win32.c` 中仅帮助调用相关 hunk
- `docs/winhelp/*`

不要提交：

- `*.GID`
- 反编译输出
- 外部游戏文件
- 用户正在实验的 exe 或源码改动
