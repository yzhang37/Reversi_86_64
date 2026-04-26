# Reversi 86/64

Reversi 86/64 is a native Win32 revival of the classic 16-bit Microsoft Windows Reversi. It deliberately keeps the Windows 3.1-era icon, menu layout, resources, board style, and compact visual language as a tribute to that period, while rebuilding the program for modern Windows.

The key point: this project was generated and iteratively refined with an LLM. It is both a classic Windows game port and a small signal of a new era in software preservation and development.

## Highlights

- Native x86 and x64 Win32 builds.
- Broad adaptive compatibility, from Windows 95-era systems to Windows 11.
- Classic 3D board rendering using system colors, shadows, highlights, and low-color fallback behavior.
- Modern Windows features when available: visual styles, high-DPI support, per-monitor DPI scaling, multi-monitor scale changes, and optional antialiased piece rendering.
- Legacy-safe x86 startup and imports, with runtime dispatch to modern CPU helpers only when the OS and CPU support them.
- Localized resources for English, Chinese, Japanese, Korean, French, German, Spanish, Swedish, Finnish, Portuguese, Italian, Russian, Ukrainian, Arabic, and Hebrew.
- WinHelp 4.0 help sources and build workflow.

## Downloads

Prebuilt packages are published through GitHub Releases:

<https://github.com/yzhang37/Reversi_86_64/releases>

Recommended release artifacts:

- `Reversi-x86.zip` - 32-bit compatibility build for Windows 95/NT-era systems and later 32-bit Windows.
- `Reversi-x64.zip` - 64-bit build for XP x64, Server 2003 x64, and later 64-bit Windows.
- `Reversi-source.zip` - source snapshot, resources, and help sources.

## Build

Requirements:

- Windows
- Visual Studio 2022 with MSVC x86/x64 tools
- PowerShell

Build both x86 and x64:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Build production binaries with the hidden development menu disabled by default:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Production
```

Outputs:

```text
build\x86\REVERSI.exe
build\x64\REVERSI.exe
```

Optional self-test:

```powershell
.\build\x86\REVERSI.exe --self-test
.\build\x64\REVERSI.exe --self-test
```

## Build WinHelp

Generate the Simplified Chinese WinHelp source files:

```powershell
python help\zh-CN\make_hlp_sources.py
```

Compile the WinHelp file with Microsoft Help Workshop:

```powershell
.\tools\hcw\hcw.exe /C /E help\zh-CN\REVERSI.hpj
```

Generated files:

```text
help\zh-CN\REVERSI.rtf
help\zh-CN\REVERSI.hpj
help\zh-CN\REVERSI.cnt
help\zh-CN\REVERSI.HLP
```

Copy the compiled help files beside the built executables:

```powershell
Copy-Item help\zh-CN\REVERSI.HLP build\x86\reversi.hlp -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x86\reversi.cnt -Force
Copy-Item help\zh-CN\REVERSI.HLP build\x64\reversi.hlp -Force
Copy-Item help\zh-CN\REVERSI.cnt build\x64\reversi.cnt -Force
```

See `docs\winhelp\WORKFLOW.md` for the full help-authoring notes.

## Compatibility

The x86 build is designed for the widest range of Windows versions, including legacy systems. The x64 build targets modern 64-bit Windows and omits compatibility paths that are only useful to old 32-bit systems.

The program keeps a classical Windows look on purpose. Newer APIs are loaded dynamically and used only when they are available, so the same project can preserve an old Windows feel while still behaving well on high-resolution modern desktops.
