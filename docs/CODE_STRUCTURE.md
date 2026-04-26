# Code Structure And MCG

This project follows Maintainable Code Golf (MCG): keep code compact, but never
at the expense of readability, compatibility, or future maintenance.

## MCG Rules

- Names must explain intent.
- Prefer early returns and direct control flow.
- Do not use clever side effects, dense ternaries, or hidden shared state just
  to save lines.
- Keep functions focused. Split helpers when a reader otherwise has to keep too
  many facts in their head.
- Make state visible. Globals are allowed in this Win32 port, but group related
  state and keep mutation paths easy to find.
- Comments explain compatibility traps, invariants, and original Reversi
  behavior. They should not restate obvious code.

## C Layout

`src/reversi_win32.c` is the top-level Win32 program and keeps startup,
WndProc, input dispatch, game flow, and the main include order visible.

Included shards are still compiled as one translation unit. This keeps old
MSVC/Win32 assumptions simple while making the source easier to navigate.

```text
src/reversi_win32.c        Entry point, WndProc, input flow, turn flow.
src/reversi_platform.inc   ANSI/Wide wrappers, OS probes, DPI, GDI+, colors.
src/reversi_help.inc       String/path helpers and WinHelp/CHM dispatch.
src/reversi_settings.inc   Registry settings, dark mode, saved window rects.
src/reversi_rules.inc      Board rules, legal moves, scoring, AI search.
src/reversi_test_menu.inc  Hidden developer context menu and scenarios.
src/reversi_paint.inc      Board drawing, cell cache, pieces, animations.
src/reversi_cpu_x86.c      Legacy-safe x86 CPU feature helper.
src/reversi_modern_x86.c   SSE2/modern x86 helper code.
```

When adding code, put it near the responsibility it belongs to. Avoid adding a
new shard unless it removes real navigation cost.

## RC Layout

`src/reversi.rc` is an index file for common resources and includes. Keep it
short.

```text
src/reversi.rc                 Icon, manifest, accelerator, include list.
src/reversi_version.rcinc      VERSIONINFO and VarFileInfo translations.
src/lang/reversi_en-US.rcinc   English menu and string table.
src/lang/reversi_zh-CN.rcinc   Simplified Chinese menu and string table.
src/lang/reversi_zh-TW.rcinc   Traditional Chinese menu and string table.
src/lang/reversi_*.rcinc       Other localized resources.
```

Normal user-facing strings belong in RC resources. Debug-only text may stay in
C when the feature is intentionally hidden and not localized.

## Compatibility Boundaries

- x86 startup, imports, WndProc, and main code must remain legacy IA32-safe.
- Optional modern APIs are loaded dynamically.
- x64 can be simpler, but shared code must not accidentally break x86/Win95.
- The `.inc` split is organizational only. Do not introduce cross-object static
  imports without revisiting the old-system compatibility contract.

## Refactoring Checklist

Before finishing a cleanup:

1. Confirm behavior was not intentionally changed.
2. Build both x86 and x64.
3. Run both `--self-test` binaries.
4. Check that resource includes still compile with UTF-8.
5. Keep release/debug toggles intact; normal RC files should not define the
   hidden debug string id 4919.
