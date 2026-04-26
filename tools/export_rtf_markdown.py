from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "help" / "markdown_exports"

FOOTNOTE_RE = re.compile(
    r"\{\\up ([^}]+)\}\{\\footnote\\pard\\plain\{\\up \1\} (.*?)\}",
    re.DOTALL,
)


def codec_for(path, text):
    match = re.search(r"\\ansicpg(\d+)", text)
    if match:
        return "cp" + match.group(1)

    rel = str(path.relative_to(ROOT)).replace("\\", "/").lower()
    if "help/zh-cn/" in rel:
        return "cp936"
    if any(name in rel for name in ("winmine_hlp", "freecell_hlp", "sol_hlp")):
        return "cp936"
    return "cp1252"


def decode_bytes(data, codec):
    try:
        return data.decode(codec)
    except UnicodeDecodeError:
        return data.decode(codec, errors="replace")


def rtf_text(raw, codec):
    out = []
    bytebuf = bytearray()
    i = 0

    def flush_bytes():
        if bytebuf:
            out.append(decode_bytes(bytes(bytebuf), codec))
            bytebuf.clear()

    while i < len(raw):
        ch = raw[i]
        if ch == "\\":
            if i + 3 < len(raw) and raw[i + 1] == "'":
                token = raw[i + 2 : i + 4]
                try:
                    bytebuf.append(int(token, 16))
                    i += 4
                    continue
                except ValueError:
                    pass

            if i + 1 < len(raw) and raw[i + 1] in "{}\\":
                flush_bytes()
                out.append(raw[i + 1])
                i += 2
                continue

            match = re.match(r"\\([A-Za-z]+)(-?\d+)? ?", raw[i:])
            if match:
                word = match.group(1)
                number = match.group(2)
                if word in ("par", "line"):
                    flush_bytes()
                    out.append("\n")
                elif word == "tab":
                    flush_bytes()
                    out.append("\t")
                elif word == "page":
                    flush_bytes()
                    out.append("\n[PAGE]\n")
                elif word == "u" and number is not None:
                    flush_bytes()
                    value = int(number)
                    if value < 0:
                        value += 65536
                    out.append(chr(value))
                i += len(match.group(0))
                continue

            flush_bytes()
            i += 1
            continue

        if ch in "{}":
            flush_bytes()
            i += 1
            continue

        flush_bytes()
        out.append(ch)
        i += 1

    flush_bytes()
    text = "".join(out)
    text = re.sub(r"[ \t]+\n", "\n", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    return text.strip()


def clean_body(raw):
    body = FOOTNOTE_RE.sub("", raw)
    body = re.sub(r"^\s*\{\\rtf1.*?\}\\pard\\plain", "", body, flags=re.DOTALL)
    body = body.strip()
    if body.endswith("}"):
        body = body[:-1].rstrip()
    return body


def split_topics(text):
    parts = re.split(r"\\page\b", text)
    topics = []
    for part in parts:
        first = FOOTNOTE_RE.search(part)
        if not first:
            continue
        topics.append(part[first.start() :].strip())
    return topics


def md_escape_inline(text):
    return text.replace("\\", "\\\\").replace("`", "\\`").replace("|", "\\|")


def emit_footnotes(topic, codec):
    footnotes = []
    for match in FOOTNOTE_RE.finditer(topic):
        mark = rtf_text(match.group(1), codec)
        raw_value = match.group(2)
        footnotes.append((mark, rtf_text(raw_value, codec), raw_value))
    return footnotes


def export_one(path):
    raw_text = path.read_text(encoding="latin1")
    codec = codec_for(path, raw_text)
    topics = split_topics(raw_text)
    rel = path.relative_to(ROOT)
    out_path = OUT / rel.with_suffix(rel.suffix + ".md")
    out_path.parent.mkdir(parents=True, exist_ok=True)

    lines = []
    lines.append(f"# {rel.as_posix()}")
    lines.append("")
    lines.append(f"- Source: `{rel.as_posix()}`")
    lines.append(f"- Text codec for hex escapes: `{codec}`")
    lines.append(f"- Topic count: `{len(topics)}`")
    lines.append("")

    for index, topic in enumerate(topics, 1):
        footnotes = emit_footnotes(topic, codec)
        title = next((value for mark, value, _ in footnotes if mark == "$"), "")
        topic_id = next((value for mark, value, _ in footnotes if mark == "#"), "")
        heading = title or topic_id or f"Topic {index}"

        lines.append(f"## {index}. {heading}")
        lines.append("")
        lines.append("### WinHelp Footnotes")
        lines.append("")
        if footnotes:
            for mark, value, raw_value in footnotes:
                lines.append(f"- Marker `{md_escape_inline(mark)}`")
                lines.append(f"  - Decoded: `{md_escape_inline(value)}`")
                lines.append("  - Raw:")
                lines.append("    ```rtf")
                for raw_line in raw_value.splitlines() or [""]:
                    lines.append("    " + raw_line)
                lines.append("    ```")
        else:
            lines.append("(none)")
        lines.append("")

        body = clean_body(topic)
        decoded_body = rtf_text(body, codec)
        lines.append("### Decoded Body")
        lines.append("")
        lines.append("```text")
        lines.append(decoded_body)
        lines.append("```")
        lines.append("")
        lines.append("### Raw RTF Body")
        lines.append("")
        lines.append("```rtf")
        lines.append(body)
        lines.append("```")
        lines.append("")

    out_path.write_text("\n".join(lines), encoding="utf-8-sig")
    return rel, out_path.relative_to(ROOT), len(topics)


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    rtfs = []
    for path in ROOT.rglob("*.rtf"):
        if ".git" in path.parts:
            continue
        if OUT in path.parents:
            continue
        rtfs.append(path)

    exports = [export_one(path) for path in sorted(rtfs)]

    index_lines = ["# RTF Markdown Exports", ""]
    for source, output, count in exports:
        index_lines.append(
            f"- `{source.as_posix()}` -> `{output.as_posix()}` ({count} topics)"
        )
    (OUT / "INDEX.md").write_text(
        "\n".join(index_lines) + "\n",
        encoding="utf-8-sig",
    )

    for source, output, count in exports:
        print(f"{source.as_posix()} -> {output.as_posix()} ({count} topics)")


if __name__ == "__main__":
    main()
