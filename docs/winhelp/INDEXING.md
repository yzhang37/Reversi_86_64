# WinHelp Indexing And Related Topics

本文记录 WinHelp 4.0 的索引、A-keyword、相关主题菜单和上下文帮助写法。

## K Footnote 是索引

`K` footnote 会进入“索引”页：

```rtf
{\up K}{\footnote\pard\plain{\up K} 翻转棋;游戏规则;合法走法}
```

多个词用分号分隔。不要只放页面标题。Windows 自带帮助会为一个 topic 放很多入口，让用户按不同说法都能找到同一页。

## 组合索引

WinHelp 支持类似 Notepad 的组合索引：

```rtf
{\up K}{\footnote\pard\plain{\up K} 棋子, 翻转;合法走法, 夹住;提示, 练习赛}
```

显示效果类似：

```text
棋子
  翻转
合法走法
  夹住
提示
  练习赛
```

写索引时要从用户的查找语言出发，而不是从页面结构出发。

## 本项目索引策略

当前多语言索引候选词由 `help/make_hlp_sources.py` 生成，并输出到
`docs/winhelp/HELP_TERMS.md`。修改 topic 标题、正文术语或 K 词之前，先检查
这个文件，避免 UI、帮助正文和索引互相打架。

每个 topic 至少覆盖这些入口类型：

- 页面标题：例如 `游戏回合`。
- 用户动作：例如 `落子`、`让棋`、`选择难度`。
- 程序界面词：例如 `游戏菜单`、`选项菜单`、`Enter`。
- 规则术语：例如 `合法走法`、`夹住`、`翻转棋子`。
- 用户疑问：例如 `不能落子`、`不知道怎么走`、`为什么可以落子`。
- 组合词：例如 `让棋, 没有合法走法`。

例子：

```python
[
    "游戏回合",
    "回合, 可以落子",
    "回合, 不能落子",
    "落子",
    "指针, 十字形",
    "让棋, 没有合法走法",
    "游戏菜单, 让棋",
]
```

## A Footnote 是隐藏相关主题

`A` footnote 不显示在索引里。它适合“相关主题”按钮：

```rtf
{\up A}{\footnote\pard\plain{\up A} a-reversi-turn}
```

然后用 `AL()` 查找：

```rtf
\{button ,AL("a-reversi-turn;a-reversi-legal")\} \uldb 相关主题\plain\f1\fs18 {\v %!AL("a-reversi-turn;a-reversi-legal")}
```

选择 `A` 而不是 `K` 的原因：

- 相关主题分组不污染索引。
- 可以为每个 topic 放稳定的 ASCII key。
- 同一个 topic 可以同时有多个 A-keyword。

## AL 和 KL

WinHelp 4.0 常用相关主题宏：

- `AL("key")`：按 A-keyword 找 topic。
- `KL("key")`：按 K-keyword 找 topic。

经验：

- 多个 key 用分号分隔。
- 多个匹配会弹出 Topics Found 列表。
- 只有一个匹配时，WinHelp 可能直接跳转。
- Windows 95 支持这种按钮。Notepad 等系统帮助也使用类似机制。

## Windows 95 上的 141 错误

Win95 上 `AL()` 报 141 不一定是宏写错。

本项目确认过一个关键原因：

- 如果 `C:\WINDOWS` 或 `C:\WINDOWS\HELP` 里存在另一个同名 `REVERSI.HLP`，WinHelp 可能解析到错误的 help family。
- 结果是本地当前目录的 `REVERSI.HLP` 看起来能打开，但 `AL()` 查找相关主题时报 141。
- 删除或改名系统目录里的同名 HLP 后，同一份 HLP 的 `AL()` 可以工作。

排查 ALink 时，先查同名文件冲突，再查 RTF 宏。

## 上下文帮助

Wolai 的 reference block 或上下文帮助，在 WinHelp 里应转成 popup topic。

规则：

- popup 解释一个术语。
- popup 词必须出现在正文附近。
- 不要把 popup 写成长页面。
- popup 不进入 CNT。

例子：

```rtf
这个位置必须\ul 夹住\plain\f1\fs18 {\v POP_CLAMP}至少一个对手棋子。
```

对应 popup：

```rtf
{\up #}{\footnote\pard\plain{\up #} POP_CLAMP}
\pard\plain\f1\fs18 夹住 是指对手棋子位于您新落的棋子和您已有的棋子之间。\par
```

## 相关主题不要重复

页面正文里可以有小节标题“相关主题”，但如果下面只有一个 `AL()` 按钮，视觉上会显得重复。

可选做法：

- 保留小节标题，并把按钮做成类似 Windows 帮助的“相关主题”入口。
- 或者像部分系统帮助那样，用一根横线加一个 authorable button，让按钮自己显示“相关主题”。

当前项目为了稳定和可读，先保留标题加按钮的结构。后续若要更接近系统帮助，可改成横线按钮样式。
