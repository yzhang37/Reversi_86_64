# Porting Notes

This document captures the current behavior contract for the native Win32
Reversi port.

## Original Program Baseline

- Treat the 16-bit `REVERSI.EXE`, `decompiled/REVERSI.asm`, and IDA database as
  behavioral references.
- When visuals or interaction differ, inspect the original code path instead
  of guessing from screenshots.
- The goal is not a modern reinterpretation. It is a faithful native port with
  dynamic use of newer OS capabilities where safe.

## Source Map

```text
src/reversi_win32.c        Entry point, WndProc, input flow, turn flow.
src/reversi_platform.inc   ANSI/Wide wrappers, OS probes, DPI, GDI+, colors.
src/reversi_help.inc       String/path helpers and WinHelp/CHM dispatch.
src/reversi_settings.inc   Registry settings, dark mode, saved window rects.
src/reversi_rules.inc      Board rules, legal moves, scoring, AI search.
src/reversi_test_menu.inc  Hidden developer context menu and scenarios.
src/reversi_paint.inc      Board drawing, cell cache, pieces, animations.
src/reversi_cpu_x86.c      x86 legacy CPU feature helpers.
src/reversi_modern_x86.c   x86 SSE2/modern helper implementation.
src/reversi.rc             Common resources and include list.
src/lang/*.rcinc           Localized menus and string tables.
src/reversi_version.rcinc  Version info and translation table.
src/resource.h             Resource and command identifiers.
build.ps1                  MSVC build script.
```

The `.inc` files are organizational shards included by `reversi_win32.c`, not
separate translation units. This preserves old-system assumptions while keeping
the source navigable.

## Maintainable Code Golf

Use Maintainable Code Golf (MCG) for cleanup work:

- Compact code by removing noise, not by hiding meaning.
- Keep names descriptive and logic direct.
- Prefer short helpers with clear boundaries over very long functions.
- Do not use clever short-circuit side effects, dense ternary chains, or
  implicit shared state to save lines.
- Put code in the responsibility shard where a future maintainer would look
  first.
- Document only important compatibility traps, invariants, original behavior,
  and boundary conditions.

## Runtime Feature Detection

Use runtime detection whenever a feature might not exist on old systems.

Examples:

- Wide APIs on NT-family systems for x86 ANSI builds.
- DPI awareness:
  - Windows 10 1703+: try `SetProcessDpiAwarenessContext`.
  - Windows 8.1+: fall back to `SetProcessDpiAwareness`.
  - Vista/7: fall back to `SetProcessDPIAware`.
  - XP/2000/95: do not call newer DPI APIs.
- GDI+ is optional and loaded dynamically. Old systems continue with GDI.
- Dark mode is optional and must not break Windows 95/2000/XP.
- Visual styles are enabled only where supported.

## RTL Layout

- Arabic and Hebrew use native Windows RTL layout for system-managed UI.
- If an application-local MUI satellite is loaded, LTR/RTL follows the
  satellite locale instead of the fallback EXE's compile-time locale.
- The board is painted in LTR HDC coordinates so the visible grid and opening
  layout stay stable. In a `WS_EX_LAYOUTRTL` window, Windows mirrors client
  mouse coordinates, `ClientToScreen`, and invalidation rectangles; convert X
  only at that API boundary.
- Do not reverse the board array or swap visible opening pieces for RTL.
- Mouse hover over a board cell updates the selected cell used by keyboard
  navigation. Direction keys must continue from the currently visible hover
  square in both LTR and RTL layouts. Before direction keys or Space act, sync
  from the current cursor position if it is still over the board; off-window or
  off-board positions must not overwrite the last valid selected cell.

## x86 Modern Dispatch

The x86 EXE must remain loadable on old systems.

- Main program, imports, entry point, and window procedure stay legacy IA32.
- Modern helper code may be compiled separately with SSE2.
- Runtime checks decide whether modern helper functions can run:
  - OS must be NT/modern enough.
  - CPU must support SSE2.
  - AVX paths, if added later, must also check OSXSAVE and XGETBV.
- Win95 must never execute modern instruction code.

## x64 Simplification

x64 is Unicode and begins in the XP x64 / Server 2003 x64 era.

- Do not carry Win95 ANSI compatibility wrappers into x64 unless needed for a
  shared source abstraction.
- It is acceptable to use x64-specific build options that reduce size, as long
  as x86 behavior and compatibility are not changed.
- Current x64 build uses size-oriented optimization and linker folding.

## Drawing

- Preserve the original 3D board look: raised cells, right/bottom shadow, and
  system-color-derived grays.
- Pieces have highlights in the original. Keep antialiasing only for round
  pieces/animation where GDI+ is available.
- Straight board lines and 45-degree edges should remain crisp.
- Cache rendered cell variants when possible:
  - empty
  - red
  - blue
  - red flip frames
  - blue flip frames
- Invalidate drawing caches when:
  - window size changes
  - DPI changes
  - system colors/theme changes
  - light/dark mode changes
  - GDI+ availability or resize fallback mode changes
- While resizing, use GDI fallback if cache is invalid and the user is inside
  the sizing loop.
- Preserve mono/low-color fallback for old systems. Newer systems do not need
  special mono-only behavior unless the device reports it.

## Dark Mode

- Dark mode is best-effort and dynamic.
- React to system/theme change messages and repaint.
- Shadows should remain normal; only surface colors become darker.
- Traditional `HMENU` menu bars and native MessageBox/AboutBox cannot be made
  fully dark with a simple supported HMENU flag. Do not use hook/subclass
  tricks for that.
- DWM title-bar dark attributes may be used dynamically on supported systems.

## DPI

- Support system/per-monitor DPI dynamically according to OS capability.
- Handle `WM_DPICHANGED`, `WM_MOVE`, `WM_WINDOWPOSCHANGED`,
  `WM_DISPLAYCHANGE`, `WM_THEMECHANGED`, and size move messages where relevant.
- Reload window icons after DPI changes, including monitor changes on modern
  Windows.
- Registry window rectangles are stored in DPI-compatible logical form and
  validated before use.

## Input And Game Flow

- Users cannot interact with the board while the computer is moving, a hint is
  flashing, a placement flash is active, or flip animation is running.
- Computer moves flash before flips, even if normal animation is disabled.
- Hint flash temporarily disables normal board clicks and may temporarily hide
  the cursor, matching original behavior.
- Direction keys move the selected board coordinate one cell at a time and wrap
  around edges. They are not restricted to legal moves.
- Space plays the selected square in normal mode.
- Mouse cursor changes to cross only over legal moves in normal play.
- Invalid move prompts use native MessageBox with owner-centered placement.
- Repeated invalid moves:
  - first illegal move after a new game shows the prompt
  - later illegal moves ding
  - repeated mistakes can re-enter prompt mode
  - successful move, pass, new game, or hint resets the run

## Pass And Endgame Logic

Keep these cases distinct:

- Player can pass only on the opening pass case or when the player has no
  legal move and the computer does have a legal move.
- If the player cannot pass, show the native information prompt.
- If the player must pass, show the must-pass message and require Game -> Pass.
- If the computer has no legal move but the player does, show the computer-pass
  message and return control to the player.
- If neither side has a legal move, end the game and show the score result.
- Result text is localized with units through `IDS_WON_FMT` and
  `IDS_LOST_FMT`. English uses "won/lost by N"; Chinese resources should use
  a natural "by N stones/discs" wording rather than a bare number.

## Registry

Settings are stored under:

```text
HKEY_CURRENT_USER\Software\Microsoft\reversi
```

Tracked values include:

- window position and size
- saved DPI
- skill
- animation

Validate every value. Invalid values should be ignored and repaired with
defaults rather than preserved.

## Hidden Debug Menu

The hidden debug menu is normally disabled and can be unlocked by editing the
string table after build.

- Created at runtime, not as an RC menu.
- The normal RC files do not define string id 4919. Missing means empty.
- If string id 4919 is added or changed to the unlock phrase with Resource
  Hacker, the active language resource enables the menu at startup.
- The C code builds the unlock phrase and resource id at runtime and wipes the
  temporary buffers after comparison.
- Opened by right-clicking blank client area.
- Labels are hardcoded English and intentionally not localized.
- `Scratch Board` is a checkbox. When enabled, clicking any board square cycles
  `empty -> red -> blue -> empty`. It must not trigger flips, AI, pass logic,
  win/loss/draw logic, sounds, or prompts.

## Help Integration

- Preserve original Help menu commands.
- Prefer CHM when supported and appropriate:
  - Windows 98/2000+ may prefer matching CHM if present.
  - Vista+ forces CHM/system help behavior.
- Legacy systems use WinHelp/HLP.
- Help discovery should include app-local files, system help directories, and
  MUI/language-specific locations where supported.
- In MUI-style builds, local `<locale>/reversi.hlp`, `MUI/<hex-langid>`, and
  Windows-style `Help/MUI/<hex-langid>` locations are checked before the EXE
  directory and parent locale directory.
- If no help file can be opened, show a short built-in native MessageBox
  fallback.
- Help Search and Help-on-Help fallback failures use an exclamation/warning
  MessageBox icon. The Contents fallback is informational.

## Message Boxes And About

- Use native MessageBox for simple prompts.
- Game-owned MessageBox prompts are centered over the program window.
- Startup failure prompts with no owner remain system-positioned.
- About uses the Windows Shell About dialog where possible.
