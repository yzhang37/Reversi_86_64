# WinHelp 4.0 Format Notes

本文记录本项目生成中文 WinHelp 4.0 文件时确认过的格式规则。目标是让后续 agent 不必重新猜 RTF、HPJ、CNT 和 WinHelp 宏的细节。

## 文件组成

一个可用的 WinHelp 4.0 帮助通常至少包含：

- `.HLP`：编译后的帮助主体。
- `.CNT`：WinHelp 4.0 的目录文件。
- `.GID`：WinHelp 运行时缓存文件。它由系统生成，不要提交。

本项目的源文件在 `help/zh-CN/`：

- `REVERSI.rtf`：topic 源。
- `REVERSI.hpj`：Help Workshop 工程。
- `REVERSI.cnt`：目录。
- `REVERSI.HLP`：编译结果。

## HPJ 要点

当前中文工程需要这些选项：

```ini
[OPTIONS]
TITLE=“翻转棋”帮助
CONTENTS=JP.RLA
CNT=REVERSI.cnt
LCID=0x804 0x0 0x0
DEFFONT=宋体,9,134
COMPRESS=0
```

说明：

- `CONTENTS=JP.RLA` 指向一个 fallback topic。没有 `.CNT` 时，WinHelp 可以显示“单击‘帮助主题’返回到主题列表。”，而不是报 1032。
- `CNT=REVERSI.cnt` 让 `HELP_FINDER` 能打开目录和索引界面。
- `LCID=0x804` 表示简体中文。
- `DEFFONT=宋体,9,134` 是中文版默认显示字体。英文版才考虑保留 MS Sans Serif。
- `COMPRESS=0` 方便反编译和二进制检查。

不要盲目相信 HelpDeco 反编译出的 HPJ。它能还原大量结构，但 WinHelp 4.0 的部分 SYSTEM record 不一定能完整还原。

## RTF 头部

中文 RTF 使用 GBK 代码页和宋体：

```rtf
{\rtf1\ansi\ansicpg936\deff0
{\fonttbl{\f0\fnil\fcharset134 \'cb\'ce\'cc\'e5;}{\f1\fnil\fcharset134 \'cb\'ce\'cc\'e5;}}
{\stylesheet{\f0\fs18 \snext0 Normal;}}
\pard\plain\f0\fs18
```

规则：

- `\ansicpg936` 用于简体中文文本。
- `\fcharset134` 用于 GBK/简体中文字体。
- `\fs18` 是 9 号字。
- 标题、小节标题可以加粗，但每段结束后必须用 `\plain\f1\fs18` 恢复正文样式。
- 如果正文整篇看起来都加粗，通常是前一个 `\b` 没有被 `\plain` 或 `\b0` 正确清掉。

## Topic Footnotes

WinHelp topic 依靠脚注声明元数据：

```rtf
{\up $}{\footnote\pard\plain{\up $} “翻转棋”概述}
{\up #}{\footnote\pard\plain{\up #} TOPIC_OVERVIEW}
{\up >}{\footnote\pard\plain{\up >} proc4}
{\up K}{\footnote\pard\plain{\up K} 翻转棋;翻转棋, 概述}
{\up A}{\footnote\pard\plain{\up A} a-reversi-overview}
```

常用脚注：

- `$`：topic 标题。
- `#`：topic id。
- `>`：默认窗口名，例如 `proc4`。
- `K`：索引关键词。
- `A`：ALink 隐藏关键词。
- `!`：topic 进入时执行的宏。fallback topic 可用 `NS();FD()`。

Popup topic 可以只有 `#` 和正文。不要把 popup topic 写进 `.CNT`。

## 链接写法

跳到普通 topic：

```rtf
\uldb 游戏规则\plain\f1\fs18 {\v TOPIC_RULES}
```

打开 popup：

```rtf
\ul 夹住\plain\f1\fs18 {\v POP_CLAMP}
```

执行宏按钮：

```rtf
\{button ,AL("a-reversi-rules;a-reversi-legal")\} \uldb 相关主题\plain\f1\fs18 {\v %!AL("a-reversi-rules;a-reversi-legal")}
```

链接规则：

- `\uldb` 用于跳转链接。
- `\ul` 用于 popup 链接。
- 隐藏文本里的 `%!` 表示执行宏。
- popup 词必须真实出现在正文中。不要给正文中不存在的词加上下文帮助。

## CNT 写法

`.CNT` 负责 WinHelp 4.0 的目录树：

```text
:Base REVERSI.HLP>proc4
:Title “翻转棋”帮助
1 翻转棋
2 “翻转棋”概述=TOPIC_OVERVIEW
2 玩翻转棋
3 “玩翻转棋”概述=TOPIC_PLAY
```

经验：

- 书节点不一定跳 topic。顶级「翻转棋」可以只是书，下面的「“翻转棋”概述」才跳 topic。
- 如果希望书节点看起来更像 Windows 帮助目录，可以多放空书节点，再在书下面放概述页。
- `:Base REVERSI.HLP>proc4` 指定 help 文件和默认窗口。
- 不要随便加同名 `:Index REVERSI.HLP =REVERSI.HLP`。同名帮助文件冲突时，ALink 和索引行为会变得难排查。

## 原版文件观察

Notepad/WinMine/FreeCell 这些系统帮助可以作为参考，但它们也有历史包袱。

已确认：

- Notepad 的 `|KWBTREE` 关键词内容是 GBK。
- Notepad 的 raw SYSTEM record 里出现过宋体和非 134 的 charset 组合。
- HelpDeco 反编译 HPJ 时不会完整表达这些 WinHelp 4.0 record。
- 所以本项目优先使用可维护的 `\ansicpg936`、`\fcharset134`、`DEFFONT=宋体,9,134`。
