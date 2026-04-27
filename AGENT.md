# Agent Notes

This repository ports the classic 16-bit Windows Reversi to native Win32
x86 and x64 builds while keeping the original program's behavior, visuals,
resources, and era feel as much as possible.

## Non-Negotiable Workflow

- Run git commands with escalated permissions in this workspace. The sandbox
  can block `.git/index.lock`; use elevation for status, add, commit, diff,
  log, push, and similar git operations.
- Never revert user changes. The local tree may contain ignored reference
  binaries, WinHelp experiments, `.GID` caches, and documentation edits.
- If `REVERSI.exe` is running and blocks a build, kill it directly.
- Do not run `idaw.exe -?`, `idaw -?`, `idaq.exe -?`, or equivalent GUI IDA
  help-probing commands. IDA 6.8 may open a blocking GUI process instead of
  printing usage text. Use existing docs, known noninteractive commands, or
  inspect files instead.
- Prefer `rg`/`Select-String` for search and `apply_patch` for manual edits.

## Build And Test

- Main build command:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

- Smoke tests:

```powershell
.\build\x86\REVERSI.exe --self-test
.\build\x64\REVERSI.exe --self-test
```

- Build and release details are in `docs/BUILD_AND_RELEASE.md`.
- Source layout and Maintainable Code Golf rules are in
  `docs/CODE_STRUCTURE.md`.
- Porting and behavior rules are in `docs/PORTING_NOTES.md`.
- WinHelp-specific rules are in `docs/winhelp/`.
- Optional third-party tools are restored with:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1
```

- `tools/cache/` and `tools/vendor/` are ignored local state. Do not commit
  downloaded third-party tools; only commit the manifest/bootstrap scripts and
  repository-authored tools.

## Maintainable Code Golf

- This project uses "Maintainable Code Golf" (MCG): make code as compact as
  possible without making it harder to understand, modify, or extend.
- Prefer clear names, early returns, short single-purpose helpers, and visible
  state. Do not use clever short-circuit side effects, dense ternary chains, or
  hidden shared state to save lines.
- Split large code by responsibility. Keep include shards obvious and boring:
  platform glue, settings, rules, paint, help, test menu, resources.
- Comments should explain invariants, compatibility traps, and original-code
  behavior. Do not narrate obvious assignments.

## Compatibility Contract

- x86 is the broad compatibility build. Keep the main program, imports, entry
  point, and window procedure legacy IA32-safe. Do not add static imports for
  modern APIs. Detect newer OS features dynamically at runtime.
- x86 may dispatch to modern/SSE2 helper code only after runtime CPU and OS
  checks. Win95 must never execute modern instruction paths.
- x64 is the modern Unicode build. The first x86-64-capable Windows target is
  XP x64/Server 2003 x64 era, so it does not need Win95 ANSI/486 baggage.
  Prefer simpler x64 code paths when it is safe and proven by build/test.
- Use dynamic detection for optional OS features: DPI APIs, dark mode, GDI+,
  CHM/HTML Help, visual styles, and similar newer behavior.

## Resources And Localization

- `src/reversi.rc` is an index file. Language resources live in
  `src/lang/*.rcinc`; version metadata lives in `src/reversi_version.rcinc`.
- UI strings belong in resource files unless a feature is explicitly debug
  only. Supported resource languages are:
  - English, United States: 1033 / `0409`
  - Chinese, China: 2052 / `0804`
  - Chinese, Taiwan: 1028 / `0404`
  - Japanese: `0411`
  - Korean: `0412`
  - French: `040c`
  - German: `0407`
  - Spanish: `040a`
  - Swedish: `041d`
  - Finnish: `040b`
  - Portuguese, Portugal: `0816`
  - Italian: `0410`
  - Russian: `0419`
  - Ukrainian: `0422`
  - Arabic: `0401`
  - Hebrew: `040d`
- German (Switzerland) / `0807` was explicitly cancelled. Do not add it unless
  the user asks again.
- Version info currently targets the Windows 95 / Windows NT 4.0 generation:
  `4.0.0.0`.
- Keep icons, version metadata, menus, about text, and string resources in the
  resource file. Do not hard-code normal user-facing text in C.
- Hidden debug context menu exception: the debug menu labels are hardcoded in
  English in C and are not localized.

## Hidden Debug Context Menu

- The background debug menu is created at runtime, not as an RC `MENU`.
- Do not define the debug key in normal RC files. The program tries to read
  string id 4919 at startup; if it is missing, it is treated as empty and the
  menu is not created.
- A post-build Resource Hacker edit can add string id 4919 to unlock the menu.
  The menu is enabled only when that resource string is:

```text
shift xyzzy
```

- The C code must not store the unlock phrase or resource id 4919 as obvious
  plaintext/literals in the EXE. The current code builds the phrase and id at
  runtime, compares the resource value, then wipes the temporary buffers.
- If the user explicitly says to publish a formal release or production build,
  leave string id 4919 absent from RC, then build with `build.ps1 -Production`.
- The debug menu opens from right-click on blank client area. Current entries
  include pass/result scenarios and `Scratch Board`.

## UI And Gameplay Rules

- Match original 16-bit Reversi behavior before inventing new behavior. Use
  `REVERSI.asm`, IDA output, and visual comparison when behavior is disputed.
- Board drawing should retain the original 3D look, system colors, piece
  highlights, shadows, and low-color/mono fallback behavior.
- Input must be locked during computer move, placement flash, hint flash, and
  flip animation so users cannot click ahead and corrupt state.
- Computer placement flashes before flips, even when animation is otherwise
  disabled.
- Direction keys move the board cursor one square at a time and wrap across
  edges. They are not limited to legal moves.
- Message boxes use native MessageBox style, with owner-centered placement for
  game-window-owned prompts.
- Registry settings live under:

```text
HKEY_CURRENT_USER\Software\Microsoft\reversi
```

- Validate registry values on read. Out-of-range values for skill, animation,
  DPI, or window rectangle should be repaired by falling back to defaults.

## WinHelp Notes

- Detailed WinHelp 4.0 knowledge is in `docs/winhelp/`:
  - `AGENT.md` for WinHelp project rules and page structure.
  - `TOOL.md` for commands and source files.
  - `FORMAT.md` for RTF/HPJ/CNT syntax.
  - `INDEXING.md` for K indexes, ALinks, and related topics.
  - `TROUBLESHOOTING.md` for Win95/WinHelp errors and cache issues.
  - `WORKFLOW.md` for build/test flow.
- Decompile the original `REVERSI.HLP` into a reviewable output folder and keep
  generated output available under the x86 build/reference area as requested.
- Export every RTF topic/source into Markdown, including original WinHelp
  markup information, so the text and formatting can be reviewed manually.
- Rebuild `REVERSI.HLP` as final WinHelp 4.0 format, starting with Simplified
  Chinese.
- Compare against official Microsoft game help (`WINMINE.HLP`, `FREECELL.HLP`,
  `SOL.HLP`) instead of guessing.
- Help lookup should not be hard-coded only to the current directory. Prefer
  the same style of system-help discovery used by Microsoft games: app copy
  when appropriate, `%WINDIR%\Help`, and MUI/language-specific help locations
  when supported by the OS.
- Keep help integration simple and compatible with Windows 95-era systems.
  For Windows 98/2000+ prefer CHM when a matching system CHM is available; on
  Vista+ force CHM for relevant system help viewer behavior.
