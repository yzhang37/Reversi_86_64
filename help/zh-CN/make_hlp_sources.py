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

CONTENTS_FALLBACK_TOPIC = "JP.RLA"

ALINKS = {
    topic_id: "a-reversi-" + topic_key.replace("_", "-")
    for topic_key, topic_id in TOPICS.items()
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
        f"\\pard\\sb{before}\\sl-235\\li{left}\\ri125{fi} \\plain\\f1\\fs18 "
        + rich(parts)
        + "\n\\par\n"
    )


def title_line(text):
    return (
        r"\pard\sb115\sa35\sl-235\li115\ri125 \f1\b\fs18 "
        + rtf_text(text)
        + r"\plain\f1\fs18"
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
        f"\\pard\\sb{before}\\sl-235\\li2155\\fi-2035\\tx2155 \\plain\\f1\\fs18 "
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
    keywords = ";".join(ALINKS.get(topic_id, topic_id) for _, topic_id in items)
    macro = f'AL("{keywords}")'
    return (
        subheading("相关主题")
        + r"\pard\sb35\sl-235\li115\ri125\tx295 \f1\fs18 "
        + r"\{button ,"
        + macro
        + r"\} \uldb "
        + rtf_text("相关主题")
        + r"\plain\f1\fs18 {\v %!"
        + macro
        + "}\n\\par\n"
    )


def topic(topic_id, title, keywords, body):
    return (
        footnote("$", title)
        + footnote("#", topic_id)
        + footnote(">", "proc4")
        + footnote("K", ";".join(keywords))
        + footnote("A", ALINKS[topic_id])
        + title_line(title)
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


def contents_fallback_topic():
    return (
        footnote("!", "NS();FD()")
        + footnote("#", CONTENTS_FALLBACK_TOPIC)
        + r"\pard\sb45\sl-235\li295\ri125\fi-185\tx295 \f1\fs18 "
        + rtf_text("单击“帮助主题”返回到主题列表。")
        + "\n\\par\n\\page\n"
    )


def build_topics():
    topics = []

    topics.append(
        topic(
            TOPICS["overview"],
            "“翻转棋”概述",
            [
                "翻转棋",
                "翻转棋, 概述",
                "翻转棋, 玩法",
                "翻转棋, 游戏目标",
                "概述",
                "游戏, 概述",
                "游戏, 翻转棋",
                "游戏目标",
                "目标",
                "棋盘游戏",
                "红色棋子",
                "蓝色棋子",
                "玩家",
                "计算机",
                "对手",
                "胜负",
                "获胜",
                "规则, 概述",
                "帮助, 主题",
            ],
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
            [
                "玩翻转棋",
                "翻转棋, 玩",
                "翻转棋, 步骤",
                "翻转棋, 流程",
                "玩法",
                "游戏, 开始",
                "游戏, 流程",
                "流程",
                "步骤",
                "选择难度",
                "难度, 选择",
                "开始新游戏",
                "新游戏",
                "游戏回合",
                "回合",
                "落子",
                "下棋",
                "让棋",
                "比赛",
                "正常比赛",
                "键盘, 下棋",
                "提示, 获得",
            ],
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
            [
                "难度",
                "难度, 选择",
                "难度, 级别",
                "难度, 初级",
                "难度, 中级",
                "难度, 高级",
                "难度, 大师",
                "选择难度",
                "级别",
                "初级",
                "中级",
                "高级",
                "大师",
                "选项",
                "选项, 难度",
                "选项菜单",
                "计算机, 走棋水平",
                "计算机, 思考时间",
                "走棋水平",
                "思考时间",
                "当前棋局",
                "继续当前棋局",
                "提示, 难度",
            ],
            para("难度决定计算机的走棋水平。级别越高，计算机思考时间通常越长。")
            + subheading("步骤")
            + numbered(1, "打开“选项”菜单。")
            + numbered(2, "选择一个难度级别：初级、中级、高级或大师。", before=45)
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
            [
                "新游戏",
                "开始新游戏",
                "游戏, 新游戏",
                "游戏, 重新开始",
                "游戏菜单",
                "游戏菜单, 新游戏",
                "开局",
                "开始一局",
                "重新开始",
                "重开",
                "重开一局",
                "当前棋局",
                "替换当前棋局",
                "第一步",
                "先走",
                "玩家先走",
                "计算机先走",
                "让棋, 第一步",
                "难度, 新游戏前",
                "选择难度",
            ],
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
            [
                "游戏回合",
                "回合",
                "回合, 可以落子",
                "回合, 不能落子",
                "轮到您",
                "轮到玩家",
                "玩家回合",
                "落子",
                "落子, 鼠标",
                "落子, Enter",
                "下棋",
                "棋盘, 查看空格",
                "空格, 查看",
                "指针",
                "指针, 十字形",
                "十字形",
                "合法位置",
                "合法走法",
                "不能落子",
                "无法落子",
                "没有合法走法",
                "让棋",
                "让棋, 提示",
                "让棋, 没有合法走法",
                "游戏菜单, 让棋",
                "等待计算机",
                "计算机走棋",
            ],
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
            [
                "键盘",
                "键盘, 下棋",
                "键盘, 移动指针",
                "键盘, 落子",
                "按键",
                "快捷键",
                "控制",
                "方向键",
                "方向键, 移动指针",
                "Enter",
                "Enter, 落子",
                "回车键",
                "回车键, 落子",
                "空格键",
                "空格键, 落子",
                "指针, 键盘移动",
                "移动指针",
                "落子, 键盘",
            ],
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
            [
                "提示",
                "获得提示",
                "提示, 获得",
                "提示, 建议位置",
                "提示, 注意",
                "提示, 练习赛",
                "提示, 正常比赛",
                "提示, 正常模式",
                "练习赛",
                "练习赛, 提示",
                "练习赛, 返回正常模式",
                "正常模式",
                "正常模式, 返回",
                "重新开局",
                "新游戏, 返回正常模式",
                "建议位置",
                "建议走位",
                "卡住",
                "不知道怎么走",
                "难度, 提示时间",
                "思考时间, 提示",
            ],
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
            [
                "规则",
                "游戏规则",
                "规则, 概述",
                "规则, 要点",
                "规则, 棋盘和棋子",
                "规则, 合法走法",
                "规则, 翻转棋子",
                "规则, 让棋",
                "规则, 胜负",
                "棋盘和棋子",
                "棋盘",
                "棋子",
                "合法走法",
                "落子",
                "翻转棋子",
                "夹住",
                "让棋",
                "胜负",
                "游戏结束",
                "双方",
                "没有合法走法",
                "获胜",
                "本方",
                "对手",
                "计算机",
                "玩家",
            ],
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
            [
                "棋盘和棋子",
                "棋盘",
                "棋盘, 方格",
                "棋盘, 格子",
                "棋盘, 空格",
                "方格",
                "格子",
                "空格",
                "空格, 落子",
                "空格, 合法位置",
                "棋子",
                "棋子, 红色",
                "棋子, 蓝色",
                "棋子, 数量",
                "红色棋子",
                "蓝色棋子",
                "玩家, 红色棋子",
                "计算机, 蓝色棋子",
                "黑白监视器",
                "监视器, 黑白",
                "白色棋子",
                "黑色棋子",
                "占据格子",
                "格子, 占据",
            ],
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
            [
                "合法走法",
                "合法走法, 判断",
                "合法走法, 空格",
                "合法走法, 夹住",
                "合法走法, 十字形",
                "合法位置",
                "可以落子",
                "可落子位置",
                "落子位置",
                "空格, 合法",
                "夹住",
                "夹住, 至少一个对手棋子",
                "对手棋子",
                "同一条线",
                "横线",
                "竖线",
                "斜线",
                "十字形",
                "指针, 十字形",
                "不能落子",
                "没有合法走法",
                "规则, 合法走法",
                "为什么可以落子",
                "判断方法",
            ],
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
            [
                "翻转棋子",
                "翻转",
                "棋子, 翻转",
                "落子, 翻转",
                "夹住, 翻转",
                "对手棋子, 翻转",
                "本方棋子",
                "方向",
                "方向, 横线",
                "方向, 竖线",
                "方向, 斜线",
                "多个方向",
                "多个方向, 翻转",
                "横线",
                "竖线",
                "斜线",
                "一次落子",
                "一次落子, 多个方向",
                "规则, 翻转棋子",
            ],
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
            [
                "游戏结束",
                "结束",
                "结束条件",
                "胜负",
                "胜负判定",
                "获胜",
                "赢家",
                "输赢",
                "胜利",
                "棋子数量",
                "数量, 棋子",
                "棋盘, 棋子数量",
                "双方不能落子",
                "双方都不能继续落子",
                "没有合法走法",
                "合法走法, 游戏结束",
                "红色棋子, 获胜",
                "蓝色棋子, 计算机获胜",
                "平局",
                "比较棋子数量",
                "规则, 胜负",
            ],
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
    topics.append(contents_fallback_topic())

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
        + "}"
    )


def build_hpj():
    return """[OPTIONS]
TITLE=“翻转棋”帮助
CONTENTS=JP.RLA
CNT=REVERSI.cnt
LCID=0x804 0x0 0x0
DEFFONT=宋体,9,134
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
