# WinHelp Build Workflow

本文记录从 Wolai 内容到 `REVERSI.HLP` 的推荐工作流。

## 1. 编辑源头

优先编辑：

```text
help/zh-CN/make_hlp_sources.py
```

不要直接手改生成的 `REVERSI.rtf`，否则下一次运行生成器会覆盖。

生成器负责：

- topic 标题和正文。
- popup 上下文帮助。
- K 索引词。
- ALink 相关主题 key。
- RTF 字体和段落样式。
- HPJ 和 CNT 内容。

## 2. 生成源文件

```powershell
python help\zh-CN\make_hlp_sources.py
```

输出：

```text
help/zh-CN/REVERSI.rtf
help/zh-CN/REVERSI.hpj
help/zh-CN/REVERSI.cnt
```

## 3. 编译 HLP

```powershell
.\tools\hcw\hcw.exe /C /E help\zh-CN\REVERSI.hpj
```

输出：

```text
help/zh-CN/REVERSI.HLP
```

## 4. 同步到运行目录

```powershell
Copy-Item help\zh-CN\REVERSI.HLP REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt REVERSI.cnt -Force
Copy-Item help\zh-CN\REVERSI.HLP build\x86\REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x86\REVERSI.CNT -Force
Copy-Item help\zh-CN\REVERSI.HLP build\x64\REVERSI.HLP -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x64\REVERSI.CNT -Force
```

注意大小写：

- Windows 不区分大小写。
- 但为了跟旧工具和仓库习惯一致，根目录使用 `REVERSI.cnt`，build 输出使用 `REVERSI.CNT` 也可以。

## 5. 测试前清缓存

```powershell
Remove-Item .\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\help\zh-CN\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x86\REVERSI.GID -Force -ErrorAction SilentlyContinue
Remove-Item .\build\x64\REVERSI.GID -Force -ErrorAction SilentlyContinue
```

## 6. 测试重点

至少检查：

- 直接打开 `REVERSI.HLP`。
- 程序菜单“帮助 > 目录”。
- 没有 `.CNT` 时是否显示 fallback topic。
- 有 `.CNT` 时目录树是否正确。
- 索引页是否显示大量 K 关键词和组合索引。
- “相关主题”按钮是否弹出列表。
- popup 上下文帮助是否能打开。
- 中文字体是否为宋体 9 号。
- 正文是否没有大面积误加粗。

## 7. 提交范围

WinHelp 内容提交通常包括：

```text
help/zh-CN/make_hlp_sources.py
help/zh-CN/REVERSI.rtf
help/zh-CN/REVERSI.hpj
help/zh-CN/REVERSI.cnt
help/zh-CN/REVERSI.HLP
REVERSI.HLP
REVERSI.cnt
build/x86/REVERSI.HLP
build/x86/REVERSI.CNT
build/x64/REVERSI.HLP
build/x64/REVERSI.CNT
docs/winhelp/*.md
```

不要提交：

```text
*.GID
```

外部系统 HLP/CHM/EXE 参考文件、Help Workshop、反编译输出和 Markdown 导出
可以作为仓库参考资料存在。普通任务不要顺手改它们；只有用户要求同步整个仓库
或明确要求更新参考资料时才一起提交。

目录边界：

- `build/` 只放可运行构建输出，例如 `build/x86` 和 `build/x64`。
- `decompiled/` 放正式反编译结果，例如 `REVERSI_HLP`、`WINMINE_HLP`。
- `decompiled/analysis/` 放临时分析、对比和 WinHelp 行为实验，例如 `kwdebug`、`notepad_hlp_analysis`、`winhelp-alink-debug`。
- `help/markdown_exports/` 放 RTF 导出的 Markdown 视图，保留原始 WinHelp
  标记信息，方便人工审阅文字和格式。

## 8. 和 Wolai 内容对应

Wolai 中的页面关系应映射到 CNT：

```text
翻转棋
  “翻转棋”概述
  玩翻转棋
    “玩翻转棋”概述
    选择难度
    开始新游戏
    游戏回合
    使用键盘下棋
  游戏规则
    “游戏规则”概述
    棋盘和棋子
    合法走法
    翻转棋子
    游戏结束和胜负
  获得提示
```

Wolai 的上下文帮助映射到 WinHelp popup。Wolai 的页面引用映射到普通 topic jump 或 ALink 相关主题。
