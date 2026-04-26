# Agent Notes

- Do not run `idaw.exe -?`, `idaw -?`, `idaq.exe -?`, or equivalent GUI IDA help-probing commands. IDA 6.8 may open a blocking GUI process instead of printing usage text.
- If IDA behavior or command-line options are needed, inspect existing documentation files or use already-known noninteractive commands. Kill any accidentally opened IDA GUI process before continuing.
- Run git commands with escalated permissions in this workspace. The sandbox may block `.git/index.lock`, so use elevation for staging, committing, and similar git operations.

## WinHelp Notes

- Decompile the original `REVERSI.HLP` into a reviewable output folder, and keep the generated output available under the x86 build/reference area as requested.
- Export every RTF topic/source into Markdown, including the original WinHelp markup information, so the text and formatting can be edited and reviewed manually.
- Rebuild `REVERSI.HLP` as final WinHelp 4.0 format, starting with a Simplified Chinese version.
- Match official Microsoft game help style where possible. Compare against `WINMINE.HLP`, `FREECELL.HLP`, and `SOL.HLP` when available instead of guessing.
- Fix the current help layout differences: avoid the wrong white-body/menu-toolbar look, use the classic game-help presentation with the expected toolbar/buttons, richer paragraph formatting, and bullet/list styling.
- Investigate how official WinMine positions the contents/index dialog and topic window, including behavior after deleting `.GID`; Reversi should not hard-code awkward window positions.
- Help lookup should not be hard-coded only to the current directory. Prefer the same kind of system-help discovery used by Microsoft games: current app copy when appropriate, `%WINDIR%\Help`, and MUI/language-specific help locations when supported by the OS.
- Keep the help integration simple and compatible with Windows 95-era systems. For Windows 98/2000+ prefer CHM when a matching system CHM is available; on Vista+ force CHM for the relevant system help viewer behavior.
