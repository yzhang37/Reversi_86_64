# Reversi 86/64

Reversi 86/64 is a native Win32 revival of the classic 16-bit Microsoft Windows Reversi. It deliberately keeps the Windows 3.1-era icon, menu layout, resources, board style, and compact visual language as a tribute to that period, while rebuilding the program for modern Windows.

The key point: this project was generated and iteratively refined with an LLM. It is both a classic Windows game port and a small signal of a new era in software preservation and development.

## Highlights

- Native x86 and x64 Win32 builds.
- Broad adaptive compatibility, from Windows 95-era systems to Windows 11.
- Classic 3D board rendering using system colors, shadows, highlights, and low-color fallback behavior.
- Modern Windows features when available: visual styles, high-DPI support, per-monitor DPI scaling, multi-monitor scale changes, and optional antialiased piece rendering.
- Legacy-safe x86 startup and imports, with runtime dispatch to modern CPU helpers only when the OS and CPU support them.
- Per-locale release builds for English, Chinese, Japanese, Korean, French, German, Spanish, Swedish, Finnish, Portuguese, Italian, Russian, Ukrainian, Arabic, and Hebrew.
- WinHelp 4.0 help sources and build workflow.

## Downloads

Prebuilt packages are published through GitHub Releases:

<https://github.com/yzhang37/Reversi_86_64/releases>

Recommended release artifacts:

- `Reversi-<locale>-x86.zip` - 32-bit compatibility build for Windows 95/NT-era systems and later 32-bit Windows.
- `Reversi-<locale>-x64.zip` - 64-bit build for XP x64, Server 2003 x64, and later 64-bit Windows.
- `Reversi-source.zip` - source snapshot, resources, and help sources.

## Build

Requirements:

- Windows
- Visual Studio 2022 with MSVC x86/x64 tools
- PowerShell

Optional WinHelp tools can be downloaded or prepared locally with:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1
```

Build all supported locale/architecture pairs:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
# or, explicitly:
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale all
```

One-click clean and rebuild:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Clean
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Rebuild
```

List available locales or build only the ones you want:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -ListLocales
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale zh-CN
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Locale en-US,zh-CN,ja-JP
```

Build production binaries with the hidden development menu disabled by default:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Production
```

Keep compiler/resource intermediates for debugging:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -KeepIntermediate
```

Common tasks via Makefile:

```bash
make                 # same as default build
make clean           # clean current locale scope outputs
make rebuild         # clean then build
make mui             # build MUI-style outputs
make mui-clean       # clean MUI outputs
make mui-rebuild     # clean then build MUI outputs
```

Build an application-local MUI-style package:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Mui -Locale all
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Mui -Locale zh-CN,ja-JP
```

Outputs:

```text
build\<locale>\x86\REVERSI.exe
build\<locale>\x86\REVERSI.HLP
build\<locale>\x86\REVERSI.CNT
build\<locale>\x64\REVERSI.exe
build\<locale>\x64\REVERSI.HLP
build\<locale>\x64\REVERSI.CNT
```

Each executable contains only its selected locale resources. For example,
`build\zh-CN\x86\REVERSI.exe` contains Simplified Chinese resources, while
`build\en-US\x64\REVERSI.exe` contains English resources.
The matching help files live beside each EXE so a user can copy one
architecture folder directly. Use `-KeepIntermediate` if you also want copied
`.res` and `.rcinc` inspection files.

MUI-style output uses one English fallback executable per architecture plus
application-local satellite resource files:

```text
build\MUI\x86\REVERSI.exe
build\MUI\x86\<locale>\REVERSI.exe.mui
build\MUI\x86\<locale>\REVERSI.HLP
build\MUI\x86\<locale>\REVERSI.CNT
build\MUI\x86\MUI\<hex-langid>\REVERSI.exe.mui
build\MUI\x64\REVERSI.exe
build\MUI\x64\<locale>\REVERSI.exe.mui
```

At startup, the program looks first for Vista-style local locale folders such
as `zh-CN\REVERSI.exe.mui`, then for XP-style `MUI\0804` folders. If nothing
matches, it falls back to the resources built into the main EXE.

Reference reverse-engineering snapshots (for example `REVERSI.asm`) are kept under
`decompiled/` in local working trees and are excluded from release artifacts and
tracked distribution. The build and release pipeline uses source files, resource
shards, and help inputs from tracked files only.

Optional self-test:

```powershell
.\build\en-US\x86\REVERSI.exe --self-test
.\build\en-US\x64\REVERSI.exe --self-test
```

If `make` is not installed, use any `nmake`-compatible wrapper and
replace `make` with `nmake`.

## Build WinHelp

Generate WinHelp source files for one locale or all locales:

```powershell
python help\make_hlp_sources.py --locale zh-CN
python help\make_hlp_sources.py --locale all
```

Compile a WinHelp file with Microsoft Help Workshop:

```powershell
.\tools\vendor\hcw\hcw.exe /C /E help\zh-CN\REVERSI.hpj
```

Generated files:

```text
help\<locale>\REVERSI.rtf
help\<locale>\REVERSI.hpj
help\<locale>\REVERSI.cnt
help\<locale>\REVERSI.HLP
```

The build script generates and compiles matching locale help automatically
when Help Workshop is available, then copies it to `build\<locale>`.

See `docs\winhelp\WORKFLOW.md` for the full help-authoring notes.
See `docs\MUI_PACKAGING.md` for the optional MUI-style package layout.

## Compatibility

The x86 build is designed for the widest range of Windows versions, including legacy systems. The x64 build targets modern 64-bit Windows and omits compatibility paths that are only useful to old 32-bit systems.

The program keeps a classical Windows look on purpose. Newer APIs are loaded dynamically and used only when they are available, so the same project can preserve an old Windows feel while still behaving well on high-resolution modern desktops.
