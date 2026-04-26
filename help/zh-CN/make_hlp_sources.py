from pathlib import Path
from shutil import copyfile


OUT = Path(__file__).resolve().parent
ROOT = OUT.parents[1]
OFFICIAL_BULLET = ROOT / "decompiled" / "WINMINE_HLP" / "bm0.bmp"


TOPICS = {
    "overview": "TOPIC_OVERVIEW",
    "play": "TOPIC_PLAY",
    "difficulty": "TOPIC_DIFFICULTY",
    "new_game": "TOPIC_NEW_GAME",
    "turn": "TOPIC_TURN",
    "keyboard": "TOPIC_KEYBOARD",
    "hint": "TOPIC_HINT",
    "rules": "TOPIC_RULES",
    "board": "TOPIC_BOARD",
    "legal": "TOPIC_LEGAL",
    "flip": "TOPIC_FLIP",
    "end": "TOPIC_END",
    "clamp": "POP_CLAMP",
    "multi_direction": "POP_MULTI_DIRECTION",
}


def rtf_text(text):
    out = []
    for ch in text:
        if ch == "\t":
            out.append(r"\tab ")
        elif ch in "\\{}":
            out.append("\\" + ch)
        elif ch == "\n":
            out.append(r"\line ")
        elif 32 <= ord(ch) < 127:
            out.append(ch)
        else:
            out.extend("\\'%02x" % b for b in ch.encode("cp936"))
    return "".join(out)


def footnote(mark, text):
    return (
        r"{\up "
        + mark
        + r"}{\footnote\pard\plain{\up "
        + mark
        + "} "
        + rtf_text(text)
        + "}\n"
    )


def rich(parts):
    if isinstance(parts, str):
        parts = [parts]

    out = []
    for part in parts:
        if isinstance(part, str):
            out.append(rtf_text(part))
            continue

        kind, text, target = part
        if kind == "jump":
            out.append(
                r"\uldb "
                + rtf_text(text)
                + r"\plain\f1\fs18 {\v "
                + target
                + r"}\f1\fs18 "
            )
        elif kind == "popup":
            out.append(
                r"\ul "
                + rtf_text(text)
                + r"\plain\f1\fs18 {\v "
                + target
                + r"}\f1\fs18 "
            )
        else:
            raise ValueError(f"Unknown rich segment: {kind}")
    return "".join(out)


def para(parts, before=75, left=115, first=0):
    fi = f"\\fi{first}" if first else ""
    return (
        f"\\pard\\sb{before}\\sl-235\\li{left}\\ri125{fi} \\f1\\fs18 "
        + rich(parts)
        + "\n\\par\n"
    )


def title_line(text):
    return (
        r"\pard\sb115\sa35\sl-235\li115\ri125 \f1\b\fs18 "
        + rtf_text(text)
        + "\n\\par\n"
    )


def subheading(text, before=115):
    return (
        f"\\pard\\sb{before}\\sa35\\sl-235\\li115\\ri125 \\f1\\b\\fs18 "
        + rtf_text(text)
        + r"\plain\f1\fs18"
        + "\n\\par\n"
    )


def link(text, topic, before=35):
    return (
        f"\\pard\\sb{before}\\sl-235\\li355\\fi-235 \\f1\\fs18\\uldb "
        + rtf_text(text)
        + r"\plain\f1\fs18 {\v "
        + topic
        + "}\n\\par\n"
    )


def numbered(num, parts, before=75):
    return (
        f"\\pard\\sb{before}\\sl-235\\li295\\ri125\\fi-185\\tx295 \\plain\\fs18 "
        + str(num)
        + r"\tab \f1 "
        + rich(parts)
        + "\n\\par\n"
    )


def bullet(parts, before=45, left=295, first=-185):
    return (
        f"\\pard\\sb{before}\\sl-235\\li{left}\\ri125\\fi{first}\\tx{left} "
        + r"\plain\f1\fs18 \{bmc bm0.bmp\}\plain\fs18 \tab \f1 "
        + rich(parts)
        + "\n\\par\n"
    )


def table_row(left, right, before=55):
    return (
        f"\\pard\\sb{before}\\sl-235\\li2155\\fi-2035\\tx2155 \\f1\\fs18 "
        + rtf_text(left)
        + r"\tab "
        + rtf_text(right)
        + "\n\\par\n"
    )


def keyboard_table():
    return (
        r"\pard\sb115\sa35\sl-235\li2135\fi-2015\brdrb\brdrs\tx2635 \f1\b\fs18 "
        + rtf_text("按键")
        + r"\tab "
        + rtf_text("功能")
        + r"\plain\f1\fs18"
        + "\n\\par\n"
        + table_row("方向键", "移动指针。")
        + table_row("Enter 或 空格键", "落子。", before=35)
    )


def related(items):
    body = subheading("相关主题")
    for title, topic_id in items:
        body += link(title, topic_id)
    return body


def topic(topic_id, title, keywords, body):
    return (
        footnote("$", title)
        + footnote("#", topic_id)
        + footnote(">", "proc4")
        + title_line(title)
        + footnote("K", ";".join(keywords))
        + body
        + "\n\\page\n"
    )


def popup_topic(topic_id, body):
    return (
        footnote("#", topic_id)
        + r"\pard\sb45\sl-235\li115\ri125 \f1\fs18 "
        + rtf_text(body)
        + "\n\\par\n\\page\n"
    )


def hidden_contents():
    return (
        footnote("!", "NS(), FD()")
        + footnote("#", "JP.RLA")
        + r"\pard\sb45\sl-235\li295\ri125\fi-185\tx295 \f1\fs18 "
        + rtf_text("单击“帮助主题”返回到主题列表。")
        + "\n\\par\n\\par\n\\page\n"
    )


def build_topics():
    topics = []

    topics.append(
        topic(
            TOPICS["overview"],
            "“翻转棋”概述",
            ["概述", "翻转棋", "规则", "红色棋子", "蓝色棋子"],
            para(
                "翻转棋是一种在棋盘上轮流落子的游戏。您使用红色棋子，计算机使用蓝色棋子。落子时，要夹住对手棋子，并把它们翻转成您的棋子。游戏结束时，棋子较多的一方获胜。"
            )
            + para("了解如何开始新游戏、下棋、进行比赛回合，并查看完整的游戏规则。")
            + related(
                [
                    ("开始新游戏", TOPICS["new_game"]),
                    ("游戏回合", TOPICS["turn"]),
                    ("游戏规则", TOPICS["rules"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["play"],
            "玩翻转棋",
            ["玩法", "选择难度", "新游戏", "游戏回合", "让棋"],
            para("您可以按下面的步骤开始一局游戏。")
            + subheading("玩法")
            + numbered(
                1,
                [("jump", "选择难度", TOPICS["difficulty"]), "：设置计算机的走棋水平。"],
            )
            + numbered(
                2,
                [("jump", "开始新游戏", TOPICS["new_game"]), "：开始一局新的翻转棋。"],
                before=45,
            )
            + numbered(
                3,
                [("jump", "游戏回合", TOPICS["turn"]), "：在合法位置落子，或在不能落子时让棋。"],
                before=45,
            )
            + related(
                [
                    ("游戏规则", TOPICS["rules"]),
                    ("获得提示", TOPICS["hint"]),
                    ("使用键盘下棋", TOPICS["keyboard"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["difficulty"],
            "选择难度",
            ["难度", "初级", "中级", "高级", "大师级", "选项"],
            para("难度决定计算机的走棋水平。级别越高，计算机思考时间通常越长。")
            + subheading("步骤")
            + numbered(1, "打开“选项”菜单。")
            + numbered(2, "选择一个难度级别：初级、中级、高级或大师级。", before=45)
            + numbered(3, "继续当前棋局，或开始新游戏。", before=45)
            + related(
                [
                    ("开始新游戏", TOPICS["new_game"]),
                    ("玩翻转棋", TOPICS["play"]),
                    ("获得提示", TOPICS["hint"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["new_game"],
            "开始新游戏",
            ["新游戏", "开局", "难度", "让棋"],
            para("想重新开始时，可以开始新游戏。新游戏会替换当前棋局。")
            + subheading("步骤")
            + numbered(1, "如果需要，先在“选项”菜单中选择难度。")
            + numbered(2, "打开“游戏”菜单。", before=45)
            + numbered(3, "选择“新游戏”。", before=45)
            + para("新游戏开始后，通常由您先落子。第一步时，您也可以选择“让棋”，让计算机先走。")
            + related(
                [
                    ("选择难度", TOPICS["difficulty"]),
                    ("游戏回合", TOPICS["turn"]),
                    ("游戏规则", TOPICS["rules"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["turn"],
            "游戏回合",
            ["游戏回合", "落子", "让棋", "十字形", "合法走法"],
            para("轮到您时，移动指针查看棋盘上的空格。能落子的空格会让指针变成十字形。如果所有空格都不能落子，游戏会提示您让棋。")
            + para(
                [
                    "若要了解为什么某个位置可以落子，请参阅",
                    ("jump", "合法走法", TOPICS["legal"]),
                    "。",
                ]
            )
            + subheading("可以落子时")
            + numbered(1, "将指针移到一个空格上。")
            + numbered(2, "如果指针变成十字形，选择这个空格，或按 Enter 键。", before=45)
            + numbered(3, "等待计算机走棋。", before=45)
            + subheading("不能落子时")
            + numbered(1, "等待游戏提示您让棋。")
            + numbered(2, "打开“游戏”菜单。", before=45)
            + numbered(3, "选择“让棋”。", before=45)
            + related(
                [
                    ("合法走法", TOPICS["legal"]),
                    ("翻转棋子", TOPICS["flip"]),
                    ("使用键盘下棋", TOPICS["keyboard"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["keyboard"],
            "使用键盘下棋",
            ["键盘", "方向键", "Enter", "空格键", "落子"],
            para("您可以使用键盘移动指针和落子。")
            + keyboard_table()
            + related(
                [
                    ("游戏回合", TOPICS["turn"]),
                    ("合法走法", TOPICS["legal"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["hint"],
            "获得提示",
            ["提示", "练习赛", "建议位置", "难度"],
            para("如果不知道下一步怎么走，或者想看看建议位置，可以使用“提示”。")
            + subheading("注意！")
            + para("如果您想完成当前比赛，请不要使用“提示”。使用“提示”会结束当前游戏，并进入“练习赛”模式。只有重新开局，才能回到正常模式。")
            + subheading("步骤")
            + numbered(1, "打开“游戏”菜单。")
            + numbered(2, "选择“提示”。", before=45)
            + numbered(3, "查看棋盘上的提示位置。", before=45)
            + subheading("说明")
            + para("难度越高，计算机给出提示前可能需要更长时间。")
            + related(
                [
                    ("玩翻转棋", TOPICS["play"]),
                    ("游戏规则", TOPICS["rules"]),
                    ("选择难度", TOPICS["difficulty"]),
                    ("开始新游戏", TOPICS["new_game"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["rules"],
            "游戏规则",
            ["规则", "合法走法", "翻转棋子", "让棋", "胜负"],
            para("翻转棋使用同一套规则判断落子、翻转棋子和胜负。双方都不能继续落子时，游戏结束，棋子较多的一方获胜。")
            + subheading("规则要点")
            + bullet(
                [
                    ("jump", "棋盘和棋子", TOPICS["board"]),
                    "：棋盘提供落子位置，棋子表示双方占据的格子。",
                ],
                before=75,
            )
            + bullet(
                [
                    ("jump", "合法走法", TOPICS["legal"]),
                    "：落子位置必须夹住至少一个对手棋子。",
                ]
            )
            + bullet(
                [
                    ("jump", "翻转棋子", TOPICS["flip"]),
                    "：被夹住的对手棋子会翻转成本方棋子。",
                ]
            )
            + bullet("让棋：没有合法走法时，必须让棋。")
            + bullet(
                [
                    ("jump", "游戏结束和胜负", TOPICS["end"]),
                    "：双方都不能继续落子时，棋子较多的一方获胜。",
                ]
            )
            + related(
                [
                    ("玩翻转棋", TOPICS["play"]),
                    ("游戏回合", TOPICS["turn"]),
                    ("获得提示", TOPICS["hint"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["board"],
            "棋盘和棋子",
            ["棋盘", "棋子", "空格", "红色棋子", "蓝色棋子"],
            para("棋盘提供落子位置。棋子表示双方占据的格子。红色棋子属于您，蓝色棋子属于计算机。")
            + para("（如果您使用黑白监视器，红色棋子显示为白色，蓝色棋子显示为黑色。）")
            + subheading("棋盘")
            + para("棋盘是方格区域。每个格子可以为空，也可以放一枚棋子。")
            + subheading("棋子")
            + para("棋子表示一方占据的格子。")
            + subheading("空格")
            + para("空格是还没有放棋子的格子。只有合法位置可以落子。")
            + related(
                [
                    ("合法走法", TOPICS["legal"]),
                    ("游戏回合", TOPICS["turn"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["legal"],
            "合法走法",
            ["合法走法", "夹住", "空格", "十字形"],
            para("合法走法是可以落子的空格。这个空格必须夹住至少一个对手棋子。")
            + subheading("判断方法")
            + numbered(1, "落子位置必须是空格。")
            + numbered(
                2,
                [
                    "这个位置必须",
                    ("popup", "夹住", TOPICS["clamp"]),
                    "至少一个对手棋子。",
                ],
                before=45,
            )
            + numbered(3, "被夹住的棋子必须在同一条横线、竖线或斜线上。", before=45)
            + numbered(4, "如果指针变成十字形，这个位置可以落子。", before=45)
            + related(
                [
                    ("翻转棋子", TOPICS["flip"]),
                    ("游戏回合", TOPICS["turn"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["flip"],
            "翻转棋子",
            ["翻转棋子", "夹住", "多个方向", "横线", "竖线", "斜线"],
            para(
                [
                    "落子后，被夹住的对手棋子会翻转成本方棋子。一次落子可以同时翻转",
                    ("popup", "多个方向", TOPICS["multi_direction"]),
                    "上的棋子。",
                ]
            )
            + subheading("翻转规则")
            + bullet("棋子只在形成夹住关系的方向上翻转。", before=75)
            + bullet("方向可以是横线、竖线或斜线。")
            + bullet("一次落子可以翻转一个方向，也可以翻转多个方向。")
            + related(
                [
                    ("合法走法", TOPICS["legal"]),
                    ("游戏结束和胜负", TOPICS["end"]),
                ]
            ),
        )
    )

    topics.append(
        topic(
            TOPICS["end"],
            "游戏结束和胜负",
            ["游戏结束", "胜负", "获胜", "合法走法"],
            para("双方都不能继续落子时，游戏结束。棋盘上棋子较多的一方获胜。")
            + subheading("结束条件")
            + para("双方都没有合法走法时，游戏结束。")
            + subheading("胜负判定")
            + para("游戏结束后，比较棋盘上的棋子数量。棋子较多的一方获胜。")
            + subheading("您怎样获胜")
            + para("如果红色棋子多于蓝色棋子，您获胜。")
            + related(
                [
                    ("合法走法", TOPICS["legal"]),
                    ("游戏回合", TOPICS["turn"]),
                ]
            ),
        )
    )

    topics.append(
        popup_topic(
            TOPICS["clamp"],
            "夹住 是指对手棋子位于您新落的棋子和您已有的棋子之间。当形成夹住关系后，落子会将夹住范围内对方所有的棋子翻转成我方的颜色。",
        )
    )
    topics.append(
        popup_topic(
            TOPICS["multi_direction"],
            "多个方向 是指同一次落子同时在横线、竖线或斜线上形成夹住关系。一步棋可能同时改变棋盘上多条线的归属。",
        )
    )

    return topics


def build_rtf():
    return (
        r"{\rtf1\ansi\ansicpg936\deff0"
        + "\n"
        + r"{\fonttbl{\f0\fnil\fcharset134 "
        + rtf_text("宋体")
        + r";}{\f1\fnil\fcharset134 "
        + rtf_text("宋体")
        + r";}}"
        + "\n"
        + r"{\stylesheet{\f0\fs18 \snext0 Normal;}"
        + "\n}"
        + r"\pard\plain\f0\fs18"
        + "\n"
        + "".join(build_topics())
        + hidden_contents()
        + "}"
    )


def build_hpj():
    return """[OPTIONS]
TITLE=“翻转棋”帮助
CONTENTS=JP.RLA
LCID=0x804 0x0 0x0
CNT=REVERSI.cnt
COMPRESS=0

[WINDOWS]
proc4="",(653,102,360,600),20736,(255,255,226),(192,192,192),f3
trouble="",(653,102,360,600),,(255,255,226),(192,192,192),f3
big="",(18,10,600,435),20736,(255,255,226),(192,192,192),f2
moreinfo="新功能",(313,137,210,287),,(255,255,226),(192,192,192),f2
error="",,,(255,255,226),(192,192,192),f2
medium="",(115,18,350,425),20736,(255,255,226),(192,192,192),f2
bigbrows="",(18,10,600,435),21248,(255,255,226),(192,192,192),f2
overview="",(115,18,350,425),20740,(255,255,225),(192,192,192),f2

[CONFIG:0]
CS()

[CONFIG:1]
CS()

[CONFIG:2]
CS()

[CONFIG:4]
CS()

[CONFIG:5]
CS()

[CONFIG:7]
CS()

[FILES]
REVERSI.rtf

[MAP]
TOPIC_KEYBOARD 30
TOPIC_HINT 31
TOPIC_RULES 32
TOPIC_PLAY 33
TOPIC_OVERVIEW 34
TOPIC_DIFFICULTY 35
TOPIC_NEW_GAME 36
TOPIC_TURN 37
TOPIC_BOARD 38
TOPIC_LEGAL 39
TOPIC_FLIP 40
TOPIC_END 41
"""


def build_cnt():
    return """:Base REVERSI.HLP>proc4
:Title “翻转棋”帮助
1 翻转棋
2 “翻转棋”概述=TOPIC_OVERVIEW
2 玩翻转棋
3 “玩翻转棋”概述=TOPIC_PLAY
3 选择难度=TOPIC_DIFFICULTY
3 开始新游戏=TOPIC_NEW_GAME
3 游戏回合=TOPIC_TURN
3 使用键盘下棋=TOPIC_KEYBOARD
2 游戏规则
3 “游戏规则”概述=TOPIC_RULES
3 棋盘和棋子=TOPIC_BOARD
3 合法走法=TOPIC_LEGAL
3 翻转棋子=TOPIC_FLIP
3 游戏结束和胜负=TOPIC_END
2 获得提示=TOPIC_HINT
"""


def main():
    (OUT / "REVERSI.rtf").write_bytes(build_rtf().encode("ascii"))
    (OUT / "REVERSI.hpj").write_bytes(build_hpj().encode("cp936"))
    (OUT / "REVERSI.cnt").write_bytes(build_cnt().encode("cp936"))

    if OFFICIAL_BULLET.exists():
        copyfile(OFFICIAL_BULLET, OUT / "bm0.bmp")


if __name__ == "__main__":
    main()
