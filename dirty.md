# Dirty / Risky Implementation Notes

This file tracks implementation points that may be considered too clever, too internal, or worth cleaning up later.

The following are intentionally excluded from this audit for now:

- ANSI startup with runtime dispatch to Wide APIs on NT.
- Runtime CPU/OS dispatch to modern SSE/SSE2 code paths.

## Items To Review

1. `src/reversi_win32.c`

   `uxtheme.dll` ordinal `133/135/136`: `AllowDarkModeForWindow`, `SetPreferredAppMode` / `AllowDarkModeForApp`, and `FlushMenuThemes`.

   These are not public documented Win32 exports. They are dynamically loaded and guarded to Windows 10+, so they do not break old systems, but they are still internal-style usage.

2. `src/reversi_win32.c`

   `SetWindowTheme(hwnd, L"DarkMode_Explorer", NULL)`.

   This uses a theme class name convention rather than a clean official API. It is not a hook, but it is still a fragile theme-name dependency.

3. `src/reversi_win32.c`

   `DWMWA_USE_IMMERSIVE_DARK_MODE_OLD = 19`.

   This is the older Windows 10 dark-titlebar compatibility attribute. The documented value is now `20`. A cleaner option would be to support only the documented attribute and accept no dark title bar on older Windows 10 builds.

4. `src/reversi_win32.c`

   Reading `HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize\AppsUseLightTheme` directly.

   This is a common Win32 dark-mode approach, but it is still registry probing rather than a tidy public C Win32 theme API.

5. `src/reversi_win32.c` - cleared

   Custom `ShowInfoPopup` / `PopupWndProc` replacement for `MessageBox`.

   Cleared on 2026-04-26. The custom dialog surface was removed and the help fallback uses the system `MessageBox` again.

6. `build.ps1` and `src/reversi_win32.c`

   `/NODEFAULTLIB`, custom `/ENTRY:ReversiEntry`, `/GS-`, `/Zl`, hand-written `memset`, and `Set-PeVersion` directly patching PE version fields.

   This is build/runtime magic for old Windows compatibility. Removing it may hurt Windows 95 / legacy support.

7. Repository hygiene

   There are many reference and generated files in the working tree, such as `winhlp32.exe`, `wordpad.exe`, `tools/hcw/*`, `*.GID`, `decompiled/*`, IDA outputs, and Help Workshop extraction files.

   These do not affect the runtime behavior of `REVERSI.exe`, but should probably be moved under a reference directory, ignored, or cleaned before a tidy release.
