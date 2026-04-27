# Tool Bootstrap

Only repository-authored tooling belongs directly in this folder. Third-party
tools are downloaded or installed into ignored local folders:

```text
tools/cache/    downloaded archives and installer payloads
tools/vendor/   extracted or installed third-party tools
```

Prepare optional WinHelp tools:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap-tools.ps1
```

The bootstrapper uses `tools/tools-manifest.json` for URLs, SHA256 checksums,
licenses, and expected install paths.

## Included Project Tools

- `export_rtf_markdown.py` - project-authored helper for reviewing WinHelp RTF
  sources as Markdown.

## Third-Party Tools

- Microsoft Help Workshop 4.03 - required to compile `.HLP` files. The
  bootstrapper downloads and verifies the Microsoft installer, but the tool is
  licensed by Microsoft and is not redistributed from this repository.
- HelpDeco 2.1.4 - useful for inspecting legacy `.HLP` files. The bootstrapper
  downloads the GPL source archive and attempts to build `helpdeco.exe` when a
  C compiler is available.

Expected paths after bootstrap:

```text
tools/vendor/hcw/hcw.exe
tools/vendor/helpdeco-2.1.4/helpdeco.exe
```
