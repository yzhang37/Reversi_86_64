# MUI Packaging Notes

This project supports two localization packaging modes.

## Standard Per-Locale Builds

The normal release build keeps one language inside each EXE:

```text
build/<locale>/x86/REVERSI.exe
build/<locale>/x64/REVERSI.exe
```

This is the safest model for Windows 95-era systems and remains the default.

## Application-Local MUI-Style Builds

`build.ps1 -Mui` creates one English fallback EXE per architecture and one
resource-only satellite per selected locale:

```text
build/MUI/x86/REVERSI.exe
build/MUI/x86/<locale>/REVERSI.exe.mui
build/MUI/x86/<locale>/REVERSI.HLP
build/MUI/x86/<locale>/REVERSI.CNT
build/MUI/x86/MUI/<hex-langid>/REVERSI.exe.mui
build/MUI/x86/MUI/<hex-langid>/REVERSI.HLP
build/MUI/x86/MUI/<hex-langid>/REVERSI.CNT
build/MUI/x64/REVERSI.exe
build/MUI/x64/<locale>/REVERSI.exe.mui
build/MUI/x64/<locale>/REVERSI.HLP
build/MUI/x64/<locale>/REVERSI.CNT
build/MUI/x64/MUI/<hex-langid>/REVERSI.exe.mui
```

The satellite file name follows the Windows MUI convention of appending
`.mui` to the language-neutral file name: `REVERSI.exe.mui`.

The named locale folders (`en-US`, `zh-CN`, `zh-TW`) match the Vista and later
application-local layout. The `MUI/0409`, `MUI/0804`, and similar mirrors match
the older XP-era hex LANGID convention.

At runtime the program:

1. Reads the user's UI language with `GetUserDefaultUILanguage` when present,
   falling back to `GetUserDefaultLangID` on old systems.
2. Looks for `<locale>/REVERSI.exe.mui` beside the EXE.
3. Also accepts `MUI/<hex-langid>`, `MUI/<locale>`, `<hex-langid>`, and
   decimal LCID folders, and repeats those checks from the parent folder for
   repacked trees.
4. Loads the module as a resource file with `LoadLibraryEx`.
5. Falls back to primary-language resources, then to English resources in the
   main EXE.

Help lookup follows the loaded satellite directory first, then checks the same
locale/hex layouts for `reversi.hlp`, `reversi.cnt`, or `reversi.chm`. It also
checks Windows-style help paths such as `%WINDIR%/Help/MUI/0804`.

Vista and later use `LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE`.
Downlevel Windows uses only `LOAD_LIBRARY_AS_DATAFILE`, so Win95/XP do not need
new MUI loader APIs to be present.

This is deliberately an application-local resource mechanism. It mirrors the
MUI directory/name convention where useful, but it does not require MUIRCT
checksum binding or a Vista-only resource loader path.
