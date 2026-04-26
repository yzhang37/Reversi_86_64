# Build And Release Notes

This file records the current build contract for the native Reversi port.

## Outputs

```text
build/x86/REVERSI.exe
build/x64/REVERSI.exe
```

Help files are copied into build outputs when matching root files exist:

```text
REVERSI.HLP
REVERSI.CNT
REVERSI.CHM
```

## Standard Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

The build uses MSVC directly through `vcvarsall.bat`, not a project file.

## Production Build

For a production build without the hidden debug menu:

1. Exclude the debug resource key, for example by commenting out every
   `IDS_BACKGROUND_KEY "shift xyzzy"` line in `src/lang/*.rcinc`.
2. Build with the production compiler define:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Production
```

The `-Production` switch defines `REVERSI_PRODUCTION`, which keeps the hidden
test menu strings and password out of the compiled C object.

## Source And Resource Layout

The main C file includes responsibility-focused `.inc` shards. They are still
one translation unit:

```text
reversi_platform.inc
reversi_help.inc
reversi_settings.inc
reversi_rules.inc
reversi_test_menu.inc
reversi_paint.inc
```

`src/reversi.rc` is also an index file. Localized menus/string tables are under
`src/lang/*.rcinc`; version metadata is in `src/reversi_version.rcinc`.

## Self-Test

```powershell
.\build\x86\REVERSI.exe --self-test
.\build\x64\REVERSI.exe --self-test
```

Run both after code or resource changes.

## x86 Build Contract

- Broad compatibility build.
- Defines:

```text
WINVER=0x0400
_WIN32_WINNT=0x0400
_WIN32_WINDOWS=0x0400
```

- Main object is compiled IA32-safe.
- Legacy CPU helper: `/arch:IA32`
- Modern helper: `/arch:SSE2`
- Runtime dispatch decides whether modern helper code may run.
- Do not statically import modern APIs. Use dynamic lookup.
- Subsystem is linked as Windows GUI 5.01, then the PE OS/subsystem version is
  patched to 4.0 so Win95-era systems can load it.

## x64 Build Contract

- Modern Unicode build.
- Defines:

```text
UNICODE
_UNICODE
WINVER=0x0600
_WIN32_WINNT=0x0600
```

- x64 does not need the Win95 ANSI/486/Pentium compatibility path.
- Current size-oriented options:

```text
cl:   /O1 /Gy /Gw
link: /OPT:REF /OPT:ICF
```

- Subsystem is Windows GUI 6.0.
- Binary size changes with localized resources and bundled metadata. Do not use
  a stale byte count as a release criterion; compare size only when evaluating
  a specific optimization.

## CRT Avoidance

The program links with `/NODEFAULTLIB` and custom entry point
`/ENTRY:ReversiEntry`.

Keep local CRT stubs minimal. Current local helpers include:

```c
memset
memcpy
_fltused
```

If a compiler option introduces a new CRT reference, either remove the option
or add the smallest compatible local helper after confirming it is safe for
both x86 and x64.

## Version Info

`src/reversi_version.rcinc` currently reports:

```text
FileVersion    4.0.0.0
ProductVersion 4.0.0.0
```

This is intended to match the Windows 95 / Windows NT 4.0 generation without
claiming a specific Win95 build number or NT build number.

## Debug Menu Release Toggle

Development builds keep:

```rc
IDS_BACKGROUND_KEY "shift xyzzy"
```

Formal release or production builds must comment out every
`IDS_BACKGROUND_KEY "shift xyzzy"` line in `src/lang/*.rcinc` before building.
If the string is empty or different, the hidden debug context menu is not
created at startup. Production builds should also use `.\build.ps1
-Production` so the debug password and labels are not compiled into the EXE.

## Git Discipline

- Run git commands with escalated permissions in this workspace.
- Stage only the files related to the current task.
- Do not stage unrelated dirty files. If the user explicitly asks to submit the
  entire repository, include tracked/generated reference assets as requested.
- Generated `.GID` caches stay ignored.

## Build Blockers

If `REVERSI.exe` is running and blocks rebuild output, kill it:

```powershell
Get-Process REVERSI -ErrorAction SilentlyContinue | Stop-Process -Force
```
