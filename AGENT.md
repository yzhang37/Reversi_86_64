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
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Clean
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Rebuild
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale zh-CN
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale all
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Mui -Locale all
powershell -ExecutionPolicy Bypass -File .\build.ps1 -KeepIntermediate
```

- Makefile shortcuts:

```bash
make
make clean
make rebuild
make mui
make mui-clean
make mui-rebuild
```

- Smoke tests:

```powershell
.\build\en-US\x86\REVERSI.exe --self-test
.\build\en-US\x64\REVERSI.exe --self-test
```

- Build and release details are in `docs/BUILD_AND_RELEASE.md`.
- MUI-style packaging details are in `docs/MUI_PACKAGING.md`.
- UI and localization terminology is in `docs/TERMINOLOGY.md`.
- Source layout and Maintainable Code Golf rules are in
  `docs/CODE_STRUCTURE.md`.
- Porting and behavior rules are in `docs/PORTING_NOTES.md`.
- Implementation points that may need later cleanup are tracked in
  `docs/maintenance/IMPLEMENTATION_RISK_REVIEW.md`.
- WinHelp-specific rules are in `docs/winhelp/`. Per-locale WinHelp terms and
  index candidates are generated into `docs/winhelp/HELP_TERMS.md`.
- Optional third-party tools are restored with:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1
```

- `tools/cache/` and `tools/vendor/` are ignored local state. Do not commit
  downloaded third-party tools; only commit the manifest/bootstrap scripts and
  repository-authored tools.
- `references/vendor/` is ignored local state for third-party reference files:
  Microsoft game samples, CHM/HLP/CNT files, WinHelp viewers, GID caches,
  comparison executables, and similar external material. Keep those out of the
  repository and use tracked notes/manifests when the knowledge matters.
- `decompiled/` is ignored local state for reverse-engineering artifacts. Keep
  files like `REVERSI.asm` there rather than in source root.

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
  `build.ps1` selects one locale with a `REVERSI_LANG_*` define and writes
  outputs under `build/<locale>/x86` and `build/<locale>/x64`.
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
- `REVERSI.asm` and related disassembly artifacts should be stored under
  `decompiled/` for audit only; they are not part of released binaries or
  tracked repo content.
- Keep icons, version metadata, menus, about text, and string resources in the
  resource file. Do not hard-code normal user-facing text in C.
- Before changing any RC resource, string table, menu, version metadata,
  WinHelp/CHM file, help generator, or help text, check `docs/TERMINOLOGY.md`
  for the canonical project terms. If a term looks suspicious, inconsistent,
  or absent from the glossary, pause and reconcile the terminology instead of
  guessing locally.
- The standard release build uses per-locale builtin RC resources. Each normal
  release EXE should contain only its selected locale. Windows 95-era resource
  loading cannot be trusted to fall back to en-US when a localized
  `STRINGTABLE` block exists but omits a string id. Keep invariant resource
  names, file names, registry paths, and registry value names duplicated in
  each language rcinc.
- `build.ps1 -Mui` is a separate application-local MUI-style package. It builds
  one English fallback EXE per architecture plus resource-only
  `REVERSI.exe.mui` satellites under `build/MUI/<arch>/<locale>/`, with
  XP-style `MUI/<hex-langid>` mirrors. Runtime MUI loading must stay dynamic:
  never statically import Vista MUI APIs or make Win95/XP depend on them.
- If a MUI satellite is loaded, LTR/RTL layout follows the satellite locale,
  not the fallback EXE's compile-time locale. This prevents Arabic/Hebrew
  fallback EXEs from forcing RTL onto zh-CN, en-US, or other LTR satellites.
- RTL board input has a deliberate coordinate boundary. Board painting is
  forced to LTR HDC coordinates for faithful drawing, while `WS_EX_LAYOUTRTL`
  mirrors client mouse coordinates, `ClientToScreen`, and invalidation rects.
  Convert X only at that boundary; do not reverse board data or opening pieces.
- Normal builds remove compiler/resource intermediates from output folders by
  default. Use `-KeepIntermediate` when `.obj`, `.res`, `obj/`, or copied
  `.rcinc` files are needed for debugging or resource inspection.
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
- Mouse hover over any board cell updates the selected board cell. Keyboard
  movement starts from the current visible hover cell, not only from the last
  clicked cell. Direction keys and Space also resync from the current cursor
  position before acting, matching the original REVERSI.asm current-cursor
  model. Keep this true for both LTR and RTL layouts, and treat off-window or
  off-board cursor positions as "no new cell" rather than invalid row/col data.
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
  - `HELP_TERMS.md` for generated per-locale topic/index terms.
  - `TROUBLESHOOTING.md` for Win95/WinHelp errors and cache issues.
  - `WORKFLOW.md` for build/test flow.
- Decompile the original `REVERSI.HLP` into a reviewable output folder and keep
  generated output available under the x86 build/reference area as requested.
- Export every RTF topic/source into Markdown, including original WinHelp
  markup information, so the text and formatting can be reviewed manually.
- Rebuild `REVERSI.HLP` as final WinHelp 4.0 format for every supported
  locale. Use `help/make_hlp_sources.py --locale all`; the old
  `help/zh-CN/make_hlp_sources.py` file is only a compatibility wrapper.
- Build outputs keep shared locale assets at `build/<locale>/reversi.hlp`,
  `build/<locale>/reversi.cnt`, and `build/<locale>/reversi.res`; do not copy
  duplicate HLP/CNT files into both `x86` and `x64`.
- WinHelp is ANSI per locale. Keep code page, HPJ LCID, RTF `\ansicpg`, and
  font charset aligned with the target language.
- Compare against official Microsoft game help (`WINMINE.HLP`, `FREECELL.HLP`,
  `SOL.HLP`) instead of guessing.
- Help lookup should not be hard-coded only to the current directory. Prefer
  the same style of system-help discovery used by Microsoft games: app copy
  when appropriate, `%WINDIR%\Help`, and MUI/language-specific help locations
  when supported by the OS.
- Keep help integration simple and compatible with Windows 95-era systems.
  For Windows 98/2000+ prefer CHM when a matching system CHM is available; on
  Vista+ force CHM for relevant system help viewer behavior.
