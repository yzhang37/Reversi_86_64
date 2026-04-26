# Reversi WinHelp Agent Notes

This folder documents the WinHelp 4.0 work for `REVERSI.HLP` and
`REVERSI.CNT`.

The goal is to rebuild Reversi help in the style of Windows 95-era Microsoft
games while preserving clean, editable source material.

## Quick Map

- `WORKFLOW.md`: recommended build and test workflow.
- `TOOL.md`: tool commands and source file map.
- `FORMAT.md`: RTF, HPJ, CNT, topic footnotes, popup links, and macro buttons.
- `INDEXING.md`: K indexes, ALinks, and related-topic strategy.
- `TROUBLESHOOTING.md`: Win95/WinHelp errors, `.GID` cache behavior, and font
  issues.

## Primary Goals

- Produce `REVERSI.HLP` and matching `REVERSI.CNT`.
- Start with Simplified Chinese, then keep the structure ready for other
  languages.
- Match official Microsoft game help style where possible.
- Compare against `WINMINE.HLP`, `FREECELL.HLP`, and `SOL.HLP` when available.
- Avoid a modern web-help tone. Keep pages short, practical, and system-like.

## Content Structure

The contents tree should stay close to this hierarchy:

```text
Reversi
  Reversi overview
  Playing Reversi
    Playing Reversi overview
    Choosing skill
    Starting a new game
    Game turns
    Using the keyboard
  Rules
    Rules overview
    Board and pieces
    Legal moves
    Flipping pieces
    Game end and scoring
  Getting a hint
```

WinHelp `.cnt` book nodes should not jump directly to a topic. Top-level book
nodes such as Reversi, Playing Reversi, and Rules should contain an overview
page under the book.

## Writing Rules

- Tell the user what the page helps with before giving details.
- Keep sentences short.
- Prefer friendly system-help wording over tutorial prose.
- Chinese help text should use polite, system-like wording. Prefer terms
  equivalent to "you", "the computer", "opponent", and "both sides".
- Match actual menu commands. For example, if the program menu uses the Chinese
  term for Pass, do not replace it with a different synonym such as Skip.
- Do not make entire pages bold. Bold headings and important labels only.
- Use bullets where official Microsoft game help would use bullets.

## Popup Help

Wolai context help maps to WinHelp popup topics.

Rules:

- A popup explains one term or one small difficulty.
- The linked term must appear in nearby body text.
- Popup topics do not appear in `.cnt`.
- Popup text stays short.

Known popup topics:

```text
clamp / outflank      -> POP_CLAMP
multiple directions   -> POP_MULTI_DIRECTION
```

## Related Topics

Use WinHelp macro buttons for related topics.

- Prefer `AL()` for hidden related-topic groups.
- Use `K` keywords for public index words.
- Target topics need matching `A` or `K` footnotes.
- Multiple matches show a Topics Found list. One match may jump directly.

## Indexing Strategy

Do not index only page titles. Follow the style of Notepad and classic game
help with useful K keywords.

Examples:

```text
move
pass
hint
legal move, outflank
hint, practice game
keyboard, moving the pointer
cannot move
where to move
why a square is legal
```

## WinHelp 4.0 Requirements

- `.HLP` and `.CNT` should be kept together.
- Delete `.GID` before testing changes.
- Use `HELP_FINDER` for Help -> Contents behavior.
- Simplified Chinese RTF uses:

```rtf
\ansicpg936
\fcharset134
\fs18
```

- Use Songti/SimSun-style 9 point body text for Simplified Chinese.
- Reset body style after headings with `\plain`, or the whole page can inherit
  bold style.
- Use `WINHLP95.EXE` from the repository when testing classic behavior.

## Known WinHelp Pitfalls

- A same-name `REVERSI.HLP` in `C:\WINDOWS` or `C:\WINDOWS\HELP` can interfere
  with app-local tests.
- `.GID` caches can preserve stale contents/index/ALink behavior.
- HelpDeco is useful but does not perfectly reconstruct every WinHelp 4.0
  system record. Do not blindly trust decompiled HPJ output.

## Commit Discipline

WinHelp changes may include:

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

Do not commit generated `.GID` caches or unrelated external reference files.
