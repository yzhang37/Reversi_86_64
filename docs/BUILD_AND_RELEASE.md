# Build And Release Notes

This file records the current build contract for the native Reversi port.

## Outputs

```text
build/<locale>/x86/REVERSI.exe
build/<locale>/x86/REVERSI.HLP
build/<locale>/x86/REVERSI.CNT
build/<locale>/x64/REVERSI.exe
build/<locale>/x64/REVERSI.HLP
build/<locale>/x64/REVERSI.CNT
```

Each executable contains only one locale's resources. Supported locale folders:

```text
en-US zh-CN zh-TW ja-JP ko-KR fr-FR de-DE es-ES
sv-SE fi-FI pt-PT it-IT ru-RU uk-UA ar-SA he-IL
```

Help files live beside the architecture-specific EXE so a user can copy one
folder directly. Build intermediates are removed by default; `-KeepIntermediate`
keeps `obj/`, `.obj`, `.res`, and copied `.rcinc` files for debugging.

The build script generates WinHelp sources from `help/make_hlp_sources.py` and
compiles `REVERSI.HLP` when Help Workshop is available locally.

Optional MUI-style output is separate from the per-locale release tree:

```text
build/MUI/x86/REVERSI.exe
build/MUI/x86/<locale>/REVERSI.exe.mui
build/MUI/x86/<locale>/REVERSI.HLP
build/MUI/x86/<locale>/REVERSI.CNT
build/MUI/x86/MUI/<hex-langid>/REVERSI.exe.mui
build/MUI/x64/REVERSI.exe
build/MUI/x64/<locale>/REVERSI.exe.mui
build/MUI/x64/<locale>/REVERSI.HLP
build/MUI/x64/<locale>/REVERSI.CNT
build/MUI/x64/MUI/<hex-langid>/REVERSI.exe.mui
```

The main EXE is an English fallback. Each `.mui` file is a resource-only PE
module named after the language-neutral file (`REVERSI.exe.mui`) and stored
under a Vista-style application-local `<locale>` folder. The build also mirrors
the files to XP-style `MUI/<hex-langid>` folders, such as `MUI/0804`.

## Standard Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale all
```

The default build emits all supported locale/architecture pairs. To build only
one or more locales during development, or to list the supported locale names:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -ListLocales
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale zh-CN
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale en-US,zh-CN,ja-JP
```

The build uses MSVC directly through `vcvarsall.bat`, not a project file.
By default, build intermediates (`obj/`, `.obj`, `.res`, copied `.rcinc`) are
removed from output folders. Use `-KeepIntermediate` when you need them for
resource inspection or debugging.

## MUI-Style Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Mui -Locale all
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Mui -Locale zh-CN,ja-JP
```

This does not replace the standard per-locale release build. It creates one
x86 and one x64 executable under `build/MUI/`, then builds selected locale
satellites and help files under each architecture's local locale directories,
with XP-style `MUI/<hex-langid>` mirrors. At runtime the program picks resources
from the user's UI language, then falls back by primary language and finally to
the English resources in the main EXE.

The implementation intentionally does not statically import Vista MUI APIs.
Windows Vista and later can load PE resource modules with
`LoadLibraryEx(..., LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)`.
Older systems use the safe downlevel subset, `LOAD_LIBRARY_AS_DATAFILE`, and
continue to work without any MUI APIs being present.

See `docs/MUI_PACKAGING.md` for the runtime search order and fallback rules.

## Optional Tool Bootstrap

Third-party tools are not tracked in Git. Download or prepare local WinHelp
tools, including Microsoft Help Workshop, with:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1
```

The bootstrapper reads `tools/tools-manifest.json`, downloads archives into
`tools/cache/`, and prepares extracted tools under `tools/vendor/`. Both
directories are ignored. Repository-authored tools such as
`tools/export_rtf_markdown.py` remain tracked.

## Production Build

For a production build with the hidden debug menu disabled by default:

1. Leave string id 4919 absent from the normal RC files. The program treats a
   missing string as empty, so the debug menu is not created.
2. Build with the production switch:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Production
```

The debug menu is created only if string id 4919 is added or edited to the
unlock phrase with Resource Hacker. The phrase and id are generated at runtime
in C rather than stored as obvious plaintext/literals in the EXE.

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
`build.ps1` selects one locale with a `REVERSI_LANG_*` define so release
executables do not carry unrelated language resources.

## Self-Test

```powershell
.\build\en-US\x86\REVERSI.exe --self-test
.\build\en-US\x64\REVERSI.exe --self-test
```

Run the affected locale/architecture pairs after code or resource changes. For
full release verification, run `--self-test` across every `build/<locale>`
output.

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

The normal RC files do not define string id 4919. If a tester adds or edits
that string id to the unlock phrase with Resource Hacker, the right-click debug
menu is created at startup. If the string is missing, empty, or different, the
hidden debug context menu is not created.

Formal release or production builds must leave string id 4919 absent from RC
and use `.\build.ps1 -Production`.

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
