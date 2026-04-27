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
- `HELP_TERMS.md`: generated per-locale WinHelp topic/index terms.
- `TROUBLESHOOTING.md`: Win95/WinHelp errors, `.GID` cache behavior, and font
  issues.
- Repository-level source layout and Maintainable Code Golf rules live in
  `../CODE_STRUCTURE.md`.
- Optional WinHelp tools are prepared with
  `powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1`.
  Third-party tool payloads live under ignored `tools/cache/` and
  `tools/vendor/` directories.

## Primary Goals

- Produce `REVERSI.HLP` and matching `REVERSI.CNT` for every supported locale.
- Keep one shared source generator at `help/make_hlp_sources.py`; per-locale
  output lives under `help/<locale>/`.
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
- Localized terms must stay aligned with `docs/TERMINOLOGY.md`,
  `docs/winhelp/HELP_TERMS.md`, and `src/lang/*.rcinc`.
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
- WinHelp is ANSI, not UTF-8. Each locale must use a matching legacy code page,
  HPJ `LCID`, RTF `\ansicpg`, and font charset. Examples:

```rtf
zh-CN: \ansicpg936,  \fcharset134
zh-TW: \ansicpg950,  \fcharset136
ja-JP: \ansicpg932,  \fcharset128
ko-KR: \ansicpg949,  \fcharset129
ru/uk: \ansicpg1251, \fcharset204
ar-SA: \ansicpg1256, \fcharset178
he-IL: \ansicpg1255, \fcharset177
```

- Use Songti/SimSun-style 9 point body text for Simplified Chinese.
- Reset body style after headings with `\plain`, or the whole page can inherit
  bold style.
- Use local `WINHLP95.EXE` when testing classic behavior, if it is present.
- Use `tools/vendor/hcw/hcw.exe` for Help Workshop after bootstrapping tools.

## Known WinHelp Pitfalls

- A same-name `REVERSI.HLP` in `C:\WINDOWS` or `C:\WINDOWS\HELP` can interfere
  with app-local tests.
- `.GID` caches can preserve stale contents/index/ALink behavior.
- HelpDeco is useful but does not perfectly reconstruct every WinHelp 4.0
  system record. Do not blindly trust decompiled HPJ output.

## Commit Discipline

WinHelp changes usually include:

```text
help/make_hlp_sources.py
help/<locale>/REVERSI.rtf
help/<locale>/REVERSI.hpj
help/<locale>/REVERSI.cnt
docs/winhelp/HELP_TERMS.md
docs/winhelp/*.md
```

Generated `.HLP`, `.GID` caches, build outputs, decompiled output, and
third-party tools should not be committed unless the user explicitly changes
repository policy.

Official Microsoft game HLP/CHM/EXE files, Help Workshop files, and decompiled
outputs may be present locally as reference material, but generated/decompiled
payloads and third-party tool binaries are ignored. Do not commit them unless
the user explicitly changes the repository policy.
