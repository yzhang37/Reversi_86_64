# Terminology

This document is the project glossary for UI resources, help text, README
content, and future localization work. Prefer these terms before adding or
changing strings.

## Core Names

| Concept | en-US | zh-CN | zh-TW | Notes |
| --- | --- | --- | --- | --- |
| Application title | Reversi | 翻转棋 | 翻轉棋 | Keep the executable/resource name as `REVERSI.EXE`. |
| Game name in prose | Reversi | 翻转棋 | 翻轉棋 | Avoid switching to Othello. |
| Practice title | Reversi Practice Game | 翻转棋练习赛 | 翻轉棋練習賽 | Used for the original practice-window title behavior. |
| Player pieces | red pieces / red circles | 红色棋子 | 紅色棋子 | The human player is red. |
| Computer pieces | blue pieces / blue circles | 蓝色棋子 | 藍色棋子 | The computer is blue. |
| Legal cursor | cross cursor | 十字准心 | 十字準心 | Use for the cursor shape, not a generic plus sign. |

## Original Resource Baseline

The en-US column follows the original 16-bit Reversi wording where known. The
zh-CN column is the Simplified Chinese target wording for this port. Basic menu
group names such as `Game`, `Options`, and `Help` are intentionally omitted.
When reviewing another locale, translate from the en-US source wording and keep
any documented locale-specific exceptions explicit.

| Resource id | en-US source | zh-CN target | Notes |
| --- | --- | --- | --- |
| `IDM_HINT` / 50 | Hint | 提示 | Shortcut: Backspace. |
| `IDM_PASS` / 20 | Pass | 让棋 | Use when the player has no legal move. |
| `IDM_NEW` / 25 | New | 开局 | Shortcut: F2. |
| `IDM_EXIT` / 52 | Exit | 退出 | Menu command. |
| `IDM_BEGINNER` / 41 | Beginner | 初级 | Difficulty. |
| `IDM_NOVICE` / 42 | Novice | 中级 | zh-CN is the only intentional `中级` exception. |
| `IDM_EXPERT` / 44 | Expert | 高级 | Difficulty. |
| `IDM_MASTER` / 46 | Master | 大师 | Do not use `大师级`. |
| `IDM_ANIM_NONE` / 80 | Don't Animate | 无动画 | Original English uses verb phrase. |
| `IDM_ANIM_SLOW` / 81 | Slow Animate | 慢动画 | Original English uses verb phrase. |
| `IDM_ANIM_FAST` / 82 | Fast Animate | 快动画 | Original English uses verb phrase. |
| `IDM_HELP_CONTENTS` / 30 | Contents | 目录 | Shortcut: F1. |
| `IDM_HELP_SEARCH` / 31 | Search for Help On... | 搜索帮助内容... | WinHelp menu wording. |
| `IDM_HELP_USING` / 32 | How to Use Help | 如何使用帮助 | System help viewer command. |
| `IDM_ABOUT` / 33 | About Reversi... | 关于翻转棋... | Native Windows About dialog command. |
| `IDS_APP_TITLE` / 3 | Reversi | 翻转棋 | Window title. |
| `IDS_PRACTICE_TITLE` / 4 | Reversi Practice Game | 翻转棋练习赛 | Original practice-title behavior. |
| `IDS_PASS_TEXT` / 5 | Pass | 让棋 | Short status text. |
| `IDS_MUST_PASS` / 6 | You have no open square to move to and must pass. | 您没有空位下棋，必须让棋 | Formal player-pass message. |
| `IDS_DRAW` / 7 | Tie Game | 平局 | Original en-US says `Tie Game`, not `Draw`. |
| `IDS_LOST_FMT` / 8 | You lost by %d | 您以 %d 子落败 | Score difference format. |
| `IDS_WON_FMT` / 9 | You won by %d | 您以 %d 子获胜 | Score difference format. |
| `IDS_ABOUT_MENU` / 10 | About... | 关于... | About menu helper string. |
| `IDS_INVALID_MOVE` / 13 | You may only move to a space where the cursor is a cross. | 您只能在鼠标光标显示成十字准心的位置下棋。 | First illegal-move dialog. |
| `IDS_CANNOT_PASS` / 14 | You may not pass. Move where the cursor is a cross. | 您不能让棋。请将鼠标光标移动到显示十字准心的位置。 | The original has two spaces after the period; preserve the wording, not necessarily the spacing in docs. |
| `IDS_HELP_FILE` / 15 | reversi.hlp | reversi.hlp | File name is invariant. |
| `IDS_HELP_SEARCH_TEXT` / 101 | The Reversi help file could not be found.\n\nMake sure the help file is installed, and then try again. | 无法找到翻转棋帮助文件。\n\n请确认帮助文件已经安装，然后重试。 | Keep the paragraph break for Win95 message boxes. |
| `IDS_HELP_USING_TEXT` / 102 | Help for the Help viewer could not be opened.\n\nMake sure Windows Help is installed, and then try again. | 无法打开“如何使用帮助”的说明。\n\n请确认 Windows 帮助已经安装，然后重试。 | Keep the paragraph break for Win95 message boxes. |

## Difficulty Levels

The original English Reversi help uses the sequence `Beginner`, `Novice`,
`Expert`, `Master`. The command name is now `IDM_NOVICE`; the numeric
command value remains `42` for compatibility with old settings and code paths.

This is a product-specific difficulty ladder, not a general dictionary
translation. In ordinary prose, English `Novice` is closer to `新手` or
`初学者`; for this Reversi command, Simplified Chinese is the only intentional
exception and uses `中级` to match the classic four-level UI sequence
`初级 / 中级 / 高级 / 大师`. Do not copy that `中级` choice into other languages.

| Command id | en-US | zh-CN | zh-TW | ja-JP | ko-KR |
| --- | --- | --- | --- | --- | --- |
| `IDM_BEGINNER` | Beginner | 初级 | 初級 | 初級 | 초급 |
| `IDM_NOVICE` | Novice | 中级 | 新手 | 初心者 | 초보자 |
| `IDM_EXPERT` | Expert | 高级 | 高級 | 上級 | 고급 |
| `IDM_MASTER` | Master | 大师 | 大師 | 達人 | 마스터 |

| Command id | fr-FR | de-DE | es-ES | sv-SE | fi-FI |
| --- | --- | --- | --- | --- | --- |
| `IDM_BEGINNER` | Débutant | Anfänger | Principiante | Nybörjare | Aloittelija |
| `IDM_NOVICE` | Novice | Neuling | Novato | Novis | Noviisi |
| `IDM_EXPERT` | Expert | Experte | Experto | Expert | Asiantuntija |
| `IDM_MASTER` | Maître | Meister | Maestro | Mästare | Mestari |

| Command id | pt-PT | it-IT | ru-RU | uk-UA | ar-SA | he-IL |
| --- | --- | --- | --- | --- | --- | --- |
| `IDM_BEGINNER` | Principiante | Principiante | Начинающий | Початківець | مبتدئ | מתחיל |
| `IDM_NOVICE` | Novato | Novizio | Новичок | Новак | مستجد | טירון |
| `IDM_EXPERT` | Perito | Esperto | Эксперт | Експерт | خبير | מומחה |
| `IDM_MASTER` | Mestre | Maestro | Мастер | Майстер | محترف | אמן |

Source note for `IDM_NOVICE`: start from the English source term `Novice`, not
from the former internal name `Intermediate`. Most languages should use a local
equivalent of "novice", "newcomer", or "new player"; Simplified Chinese is the
explicit project exception and stays `中级`.

Checked references include:

- Cambridge Dictionary for French `novice`, German `Neuling`, Spanish `novato`,
  Swedish `novis`, Portuguese `novato`, Japanese `初心者`, Korean `초보자`,
  Ukrainian `новак`, Italian `principiante/apprendista/esordiente`, Russian
  `начинающий/новичок`, Arabic `مبتدئ`, and Chinese `新手/初学者`.
- Collins and WordReference for the Italian distinction between general
  `principiante` and extended/religious `novizio`; `Novizio` is kept here to
  avoid a duplicate `Principiante` menu item.
- Reverso/Hebrew dictionary sources for Hebrew `טירון`.
- Arabic sources list `مبتدئ` as the direct novice/beginner term; `مستجد` is used
  here as a UI distinction for `Novice`, while `مبتدئ` remains `Beginner`.

## Game Commands

| Concept | en-US | zh-CN | zh-TW | Notes |
| --- | --- | --- | --- | --- |
| New game | New | 开局 | 開局 | Shortcut: F2. |
| Pass | Pass | 让棋 | 讓棋 | Use when the side has no legal move. |
| Hint | Hint | 提示 | 提示 | Shortcut: Backspace. |
| Exit | Exit | 退出 | 結束 | Menu command only. |
| Draw | Tie Game | 平局 | 平局 | End-of-game result. |
| Invalid move | You may only move to a space where the cursor is a cross. | 您只能在鼠标光标显示成十字准心的位置下棋。 | 您只能在滑鼠游標顯示成十字準心的位置下棋。 | First illegal-move dialog. |
| Player cannot pass | You may not pass. Move where the cursor is a cross. | 您不能让棋。请将鼠标光标移动到显示十字准心的位置。 | 您不能讓棋。請將滑鼠游標移到顯示十字準心的位置。 | The player still has a legal move. |
| Player must pass | You have no open square to move to and must pass. | 您没有空位下棋，必须让棋 | 您沒有空位下棋，必須讓棋 | Keep the tone direct and formal. |
| Computer must pass | The computer has no open square to move to and must pass. | 计算机没有空位下棋，必须让棋 | 電腦沒有空位下棋，必須讓棋 | Prefer computer/opponent, not AI. |

## Options And Help

| Concept | en-US | zh-CN | zh-TW | Notes |
| --- | --- | --- | --- | --- |
| Options menu | Options | 选项 | 選項 | Difficulty and animation settings live here. |
| Disabled animation | Don't Animate | 无动画 | 無動畫 | Original menu concept. |
| Slow animation | Slow Animate | 慢动画 | 慢動畫 | Original menu concept. |
| Fast animation | Fast Animate | 快动画 | 快動畫 | Original menu concept. |
| Help contents | Contents | 目录 | 目錄 | Shortcut: F1. |
| Help search | Search for Help On... | 搜索帮助内容... | 搜尋說明內容... | WinHelp wording. |
| How to use help | How to Use Help | 如何使用帮助 | 如何使用說明 | System help viewer command. |
| About command | About Reversi... | 关于翻转棋... | 關於翻轉棋... | Uses the native Windows About dialog. |

## Implementation Names

These names are intentionally not localized in code or documentation prose:

- `REVERSI.EXE`
- `reversi.hlp`
- `reversi.chm`
- `windows.hlp`
- `windows.chm`
- `NTHelp.chm`
- `HKEY_CURRENT_USER\Software\Microsoft\reversi`
- `Animation`
- `Skill`
- `WindowX`, `WindowY`, `WindowWidth`, `WindowHeight`, `WindowDpi`

## WinHelp Terms

Localized WinHelp topic titles and K-footnote index candidates are generated
from `help/make_hlp_sources.py` and recorded in
`docs/winhelp/HELP_TERMS.md`. Review that file before changing help prose,
topic titles, popup terms, or index keywords.

Term source priorities:

- Match existing Reversi UI resources in `src/lang/*.rcinc`.
- Use Microsoft Language Resources / Microsoft Terminology for Windows help
  and UI wording where the term exists.
- Use World Othello Federation rules vocabulary for gameplay concepts such as
  board, square, disc/piece, move, outflank, flip, pass, and game end.
- Keep Simplified Chinese's intentional `Novice -> 中级` exception limited to
  zh-CN UI/help resources.
