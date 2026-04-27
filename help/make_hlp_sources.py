from __future__ import annotations

import argparse
from pathlib import Path
from shutil import copyfile


HELP_ROOT = Path(__file__).resolve().parent
ROOT = HELP_ROOT.parent
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
    "outflank": "POP_OUTFLANK",
    "multi_direction": "POP_MULTI_DIRECTION",
}

CONTENTS_FALLBACK_TOPIC = "JP.RLA"
ALINKS = {
    topic_id: "a-reversi-" + topic_key.replace("_", "-")
    for topic_key, topic_id in TOPICS.items()
}

TOPIC_ORDER = [
    "overview",
    "play",
    "difficulty",
    "new_game",
    "turn",
    "keyboard",
    "hint",
    "rules",
    "board",
    "legal",
    "flip",
    "end",
]

KEYWORD_KEYS = {
    "overview": [
        "game", "overview", "goal", "board_game", "red_pieces", "blue_pieces",
        "player", "computer", "opponent", "winner", "rules", "help_topics",
    ],
    "play": [
        "play", "steps", "difficulty", "new_game", "turn", "move", "pass",
        "hint", "keyboard", "practice_game", "normal_game",
    ],
    "difficulty": [
        "difficulty", "skill", "beginner", "novice", "expert", "master",
        "options", "thinking_time", "current_game",
    ],
    "new_game": [
        "new_game", "restart", "game_menu", "first_move", "player_first",
        "computer_first", "pass", "difficulty",
    ],
    "turn": [
        "turn", "player_turn", "move", "mouse", "cursor", "cross_cursor",
        "legal_move", "legal_square", "no_legal_move", "pass", "computer_move",
    ],
    "keyboard": [
        "keyboard", "arrow_keys", "enter_key", "spacebar", "cursor",
        "move", "keyboard_move",
    ],
    "hint": [
        "hint", "suggested_move", "practice_game", "normal_game", "restart",
        "difficulty", "thinking_time",
    ],
    "rules": [
        "rules", "board", "pieces", "legal_move", "outflank", "flip",
        "pass", "game_end", "scoring", "winner", "both_players",
    ],
    "board": [
        "board", "pieces", "square", "empty_square", "red_pieces",
        "blue_pieces", "mono_monitor", "white_pieces", "black_pieces",
    ],
    "legal": [
        "legal_move", "legal_square", "empty_square", "outflank", "opponent",
        "line", "row", "column", "diagonal", "cross_cursor", "no_legal_move",
    ],
    "flip": [
        "flip", "pieces", "move", "outflank", "opponent", "direction",
        "row", "column", "diagonal", "multi_direction",
    ],
    "end": [
        "game_end", "end_condition", "scoring", "winner", "tie_game",
        "piece_count", "both_players", "no_legal_move",
    ],
}

COMPOUNDS = {
    "overview": [("game", "overview"), ("game", "goal"), ("rules", "overview")],
    "play": [("game", "start"), ("game", "turn"), ("hint", "practice_game")],
    "difficulty": [("difficulty", "skill"), ("options", "difficulty")],
    "new_game": [("game_menu", "new_game"), ("pass", "first_move")],
    "turn": [("turn", "legal_move"), ("pass", "no_legal_move")],
    "keyboard": [("keyboard", "cursor"), ("keyboard", "move")],
    "hint": [("hint", "suggested_move"), ("hint", "practice_game")],
    "rules": [("rules", "legal_move"), ("rules", "flip"), ("rules", "scoring")],
    "board": [("board", "square"), ("pieces", "red_pieces"), ("pieces", "blue_pieces")],
    "legal": [("legal_move", "outflank"), ("cursor", "cross_cursor")],
    "flip": [("flip", "multi_direction"), ("move", "flip")],
    "end": [("game_end", "scoring"), ("winner", "piece_count")],
}


def locale(
    encoding,
    lcid,
    font,
    charset,
    terms,
    text,
    rtl=False,
):
    return {
        "encoding": encoding,
        "lcid": lcid,
        "font": font,
        "charset": charset,
        "terms": terms,
        "text": text,
        "rtl": rtl,
    }


LOCALES = {
    "en-US": locale(
        "cp1252",
        "0x409",
        "Arial",
        0,
        {
            "game": "Reversi",
            "help_title": "Reversi Help",
            "overview": "Reversi overview",
            "play": "Playing Reversi",
            "difficulty": "Choosing skill",
            "new_game": "Starting a new game",
            "turn": "Game turns",
            "keyboard": "Using the keyboard",
            "hint": "Getting a hint",
            "rules": "Rules",
            "board": "Board and pieces",
            "legal": "Legal moves",
            "flip": "Flipping pieces",
            "end": "Game end and scoring",
            "steps": "Steps",
            "notes": "Notes",
            "rules_points": "Rules summary",
            "game_menu": "Game menu",
            "options": "Options",
            "help_topics": "help topics",
            "goal": "object of the game",
            "board_game": "board game",
            "red_pieces": "red pieces",
            "blue_pieces": "blue pieces",
            "white_pieces": "white pieces",
            "black_pieces": "black pieces",
            "pieces": "pieces",
            "piece_count": "piece count",
            "player": "player",
            "computer": "computer",
            "opponent": "opponent",
            "both_players": "both players",
            "winner": "winner",
            "tie_game": "tie game",
            "skill": "skill",
            "beginner": "Beginner",
            "novice": "Novice",
            "expert": "Expert",
            "master": "Master",
            "thinking_time": "thinking time",
            "current_game": "current game",
            "restart": "restart",
            "first_move": "first move",
            "player_first": "player moves first",
            "computer_first": "computer moves first",
            "move": "move",
            "start": "start",
            "player_turn": "player turn",
            "computer_move": "computer move",
            "mouse": "mouse",
            "cursor": "cursor",
            "cross_cursor": "cross cursor",
            "legal_move": "legal move",
            "legal_square": "legal square",
            "no_legal_move": "no legal move",
            "pass": "pass",
            "keyboard_move": "keyboard move",
            "arrow_keys": "arrow keys",
            "enter_key": "Enter key",
            "spacebar": "Spacebar",
            "suggested_move": "suggested move",
            "practice_game": "practice game",
            "normal_game": "normal game",
            "game_end": "game end",
            "end_condition": "end condition",
            "scoring": "scoring",
            "square": "square",
            "empty_square": "empty square",
            "mono_monitor": "monochrome monitor",
            "outflank": "outflank",
            "line": "line",
            "row": "row",
            "column": "column",
            "diagonal": "diagonal",
            "direction": "direction",
            "multi_direction": "multiple directions",
        },
        {
            "overview_p1": "Reversi is a board game in which you and the computer take turns placing pieces. You use red pieces; the computer uses blue pieces.",
            "overview_p2": "A move must outflank at least one opposing piece. Outflanked pieces are flipped to your color, and the side with more pieces at the end wins.",
            "play_intro": "Use these steps to start and play a game.",
            "play_step1": "Choose a skill level for the computer.",
            "play_step2": "Start a new game.",
            "play_step3": "Move on legal squares, or pass when no move is available.",
            "difficulty_intro": "Skill controls how carefully the computer searches for a move. Higher levels may take longer.",
            "difficulty_step1": "Open the Options menu.",
            "difficulty_step2": "Choose Beginner, Novice, Expert, or Master.",
            "difficulty_step3": "Continue the current game, or start a new one.",
            "new_intro": "Starting a new game replaces the current game.",
            "new_step1": "If needed, choose a skill level first.",
            "new_step2": "Open the Game menu.",
            "new_step3": "Choose New.",
            "new_note": "You normally move first. On the first turn you may also choose Pass to let the computer move first.",
            "turn_intro": "When it is your turn, move the pointer over empty squares. A square that can be played changes the pointer to a cross.",
            "turn_can1": "Move the pointer to an empty square.",
            "turn_can2": "If the pointer becomes a cross, click the square or press Enter.",
            "turn_can3": "Wait for the computer to move.",
            "turn_cannot1": "If no square can be played, wait for the pass message.",
            "turn_cannot2": "Open the Game menu.",
            "turn_cannot3": "Choose Pass.",
            "keyboard_intro": "You can use the keyboard to move the board cursor and place a piece.",
            "keyboard_arrows": "Move the board cursor.",
            "keyboard_enter": "Place a piece.",
            "hint_intro": "Use Hint if you are not sure where to move.",
            "hint_warning": "Using Hint ends the current normal game and switches to practice mode. Start a new game to return to normal play.",
            "hint_step1": "Open the Game menu.",
            "hint_step2": "Choose Hint.",
            "hint_step3": "Watch the highlighted suggested square.",
            "hint_note": "At higher skill levels, the computer may need more time before showing the hint.",
            "rules_intro": "Reversi uses one set of rules for moving, flipping pieces, passing, and scoring.",
            "rules_b1": "The board contains the squares where pieces can be placed.",
            "rules_b2": "A legal move must outflank at least one opposing piece.",
            "rules_b3": "All outflanked pieces are flipped.",
            "rules_b4": "If a side has no legal move, that side must pass.",
            "rules_b5": "When neither side can move, the game ends and the side with more pieces wins.",
            "board_intro": "The board supplies the playing squares. Each square is empty or contains one piece.",
            "board_mono": "On a monochrome monitor, red pieces are shown as white pieces and blue pieces are shown as black pieces.",
            "board_board": "The board is an eight-by-eight square grid.",
            "board_pieces": "Pieces show which side controls a square.",
            "board_empty": "An empty square can be used only if the move is legal.",
            "legal_intro": "A legal move is an empty square that outflanks at least one opposing piece.",
            "legal_step1": "The square must be empty.",
            "legal_step2": "The new piece must outflank at least one opposing piece.",
            "legal_step3": "The outflanked pieces must be in one straight horizontal, vertical, or diagonal line.",
            "legal_step4": "If the pointer becomes a cross, the square is legal.",
            "flip_intro": "After a legal move, every outflanked opposing piece is flipped to your color.",
            "flip_b1": "Pieces flip only on lines created by the move.",
            "flip_b2": "Lines can be horizontal, vertical, or diagonal.",
            "flip_b3": "One move can flip pieces in more than one direction.",
            "end_intro": "The game ends when neither side can make another legal move.",
            "end_condition": "A game may end before every square is filled.",
            "end_scoring": "When the game ends, the pieces are counted.",
            "end_win": "If your red pieces outnumber the computer's blue pieces, you win.",
            "popup_outflank": "To outflank means to place a piece so that one or more opposing pieces are between the new piece and another piece of your color.",
            "popup_multi": "Multiple directions means that the same move outflanks pieces along more than one horizontal, vertical, or diagonal line.",
            "how_to": "How to play",
            "can_move": "When you can move",
            "cannot_move": "When you cannot move",
            "keyboard_key": "Key",
            "keyboard_action": "Action",
            "related": "Related Topics",
            "contents_hint": "Click Help Topics to return to the topic list.",
        },
    ),
    "zh-CN": locale(
        "cp936",
        "0x804",
        "宋体",
        134,
        {
            "game": "翻转棋",
            "help_title": "“翻转棋”帮助",
            "overview": "“翻转棋”概述",
            "play": "玩翻转棋",
            "difficulty": "选择难度",
            "new_game": "开始新游戏",
            "turn": "游戏回合",
            "keyboard": "使用键盘下棋",
            "hint": "获得提示",
            "rules": "游戏规则",
            "board": "棋盘和棋子",
            "legal": "合法走法",
            "flip": "翻转棋子",
            "end": "游戏结束和胜负",
            "steps": "步骤",
            "notes": "说明",
            "rules_points": "规则要点",
            "game_menu": "游戏菜单",
            "options": "选项",
            "help_topics": "帮助主题",
            "goal": "游戏目标",
            "board_game": "棋盘游戏",
            "red_pieces": "红色棋子",
            "blue_pieces": "蓝色棋子",
            "white_pieces": "白色棋子",
            "black_pieces": "黑色棋子",
            "pieces": "棋子",
            "piece_count": "棋子数量",
            "player": "玩家",
            "computer": "计算机",
            "opponent": "对手",
            "both_players": "双方",
            "winner": "赢家",
            "tie_game": "平局",
            "skill": "走棋水平",
            "beginner": "初级",
            "novice": "中级",
            "expert": "高级",
            "master": "大师",
            "thinking_time": "思考时间",
            "current_game": "当前棋局",
            "restart": "重新开始",
            "first_move": "第一步",
            "player_first": "玩家先走",
            "computer_first": "计算机先走",
            "move": "落子",
            "start": "开始",
            "player_turn": "玩家回合",
            "computer_move": "计算机走棋",
            "mouse": "鼠标",
            "cursor": "指针",
            "cross_cursor": "十字准心",
            "legal_move": "合法走法",
            "legal_square": "合法位置",
            "no_legal_move": "没有合法走法",
            "pass": "让棋",
            "keyboard_move": "键盘移动",
            "arrow_keys": "方向键",
            "enter_key": "Enter 键",
            "spacebar": "空格键",
            "suggested_move": "建议位置",
            "practice_game": "练习赛",
            "normal_game": "正常比赛",
            "game_end": "游戏结束",
            "end_condition": "结束条件",
            "scoring": "胜负判定",
            "square": "格子",
            "empty_square": "空格",
            "mono_monitor": "黑白监视器",
            "outflank": "夹住",
            "line": "同一条线",
            "row": "横线",
            "column": "竖线",
            "diagonal": "斜线",
            "direction": "方向",
            "multi_direction": "多个方向",
        },
        {
            "overview_p1": "翻转棋是一种在棋盘上轮流落子的游戏。您使用红色棋子，计算机使用蓝色棋子。",
            "overview_p2": "落子时，必须夹住至少一个对手棋子。被夹住的棋子会翻转成您的颜色，游戏结束时棋子较多的一方获胜。",
            "play_intro": "您可以按下面的步骤开始一局游戏。",
            "play_step1": "选择计算机的难度。",
            "play_step2": "开始新游戏。",
            "play_step3": "在合法位置落子；如果没有合法走法，则让棋。",
            "difficulty_intro": "难度决定计算机的走棋水平。级别越高，计算机思考时间通常越长。",
            "difficulty_step1": "打开“选项”菜单。",
            "difficulty_step2": "选择初级、中级、高级或大师。",
            "difficulty_step3": "继续当前棋局，或开始新游戏。",
            "new_intro": "开始新游戏会替换当前棋局。",
            "new_step1": "如果需要，先选择难度。",
            "new_step2": "打开“游戏”菜单。",
            "new_step3": "选择“开局”。",
            "new_note": "新游戏通常由您先落子。第一步时，您也可以选择“让棋”，让计算机先走。",
            "turn_intro": "轮到您时，移动指针查看棋盘上的空格。能落子的空格会让指针变成十字准心。",
            "turn_can1": "将指针移到一个空格上。",
            "turn_can2": "如果指针变成十字准心，单击这个空格，或按 Enter 键。",
            "turn_can3": "等待计算机走棋。",
            "turn_cannot1": "如果所有空格都不能落子，请等待让棋提示。",
            "turn_cannot2": "打开“游戏”菜单。",
            "turn_cannot3": "选择“让棋”。",
            "keyboard_intro": "您可以使用键盘移动棋盘指针并落子。",
            "keyboard_arrows": "移动棋盘指针。",
            "keyboard_enter": "落子。",
            "hint_intro": "如果不知道下一步怎么走，可以使用“提示”。",
            "hint_warning": "使用“提示”会结束当前正常比赛，并进入练习赛模式。只有重新开局，才能回到正常比赛。",
            "hint_step1": "打开“游戏”菜单。",
            "hint_step2": "选择“提示”。",
            "hint_step3": "查看棋盘上闪烁的建议位置。",
            "hint_note": "难度越高，计算机给出提示前可能需要更长时间。",
            "rules_intro": "翻转棋使用同一套规则判断落子、翻转棋子、让棋和胜负。",
            "rules_b1": "棋盘提供落子位置。",
            "rules_b2": "合法走法必须夹住至少一个对手棋子。",
            "rules_b3": "所有被夹住的棋子都必须翻转。",
            "rules_b4": "一方没有合法走法时，必须让棋。",
            "rules_b5": "双方都不能继续落子时，游戏结束，棋子较多的一方获胜。",
            "board_intro": "棋盘提供落子用的格子。每个格子可以为空，也可以放一枚棋子。",
            "board_mono": "如果您使用黑白监视器，红色棋子显示为白色，蓝色棋子显示为黑色。",
            "board_board": "棋盘是八乘八的方格区域。",
            "board_pieces": "棋子表示一方占据的格子。",
            "board_empty": "空格只有在形成合法走法时才能落子。",
            "legal_intro": "合法走法是可以落子的空格。这个空格必须夹住至少一个对手棋子。",
            "legal_step1": "落子位置必须是空格。",
            "legal_step2": "新落的棋子必须夹住至少一个对手棋子。",
            "legal_step3": "被夹住的棋子必须在同一条横线、竖线或斜线上。",
            "legal_step4": "如果指针变成十字准心，这个位置可以落子。",
            "flip_intro": "合法落子后，所有被夹住的对手棋子都会翻转成您的颜色。",
            "flip_b1": "棋子只在形成夹住关系的方向上翻转。",
            "flip_b2": "方向可以是横线、竖线或斜线。",
            "flip_b3": "一次落子可以同时翻转多个方向上的棋子。",
            "end_intro": "双方都没有合法走法时，游戏结束。",
            "end_condition": "游戏可能在棋盘未填满前结束。",
            "end_scoring": "游戏结束后，比较棋盘上的棋子数量。",
            "end_win": "如果红色棋子多于蓝色棋子，您获胜。",
            "popup_outflank": "夹住 是指对手棋子位于您新落的棋子和您已有的棋子之间。形成夹住关系后，这些对手棋子会翻转成您的颜色。",
            "popup_multi": "多个方向 是指同一次落子同时在横线、竖线或斜线上形成夹住关系。",
            "how_to": "玩法",
            "can_move": "可以落子时",
            "cannot_move": "不能落子时",
            "keyboard_key": "按键",
            "keyboard_action": "功能",
            "related": "相关主题",
            "contents_hint": "单击“帮助主题”返回到主题列表。",
        },
    ),
    "zh-TW": locale(
        "cp950",
        "0x404",
        "新細明體",
        136,
        {
            "game": "翻轉棋",
            "help_title": "「翻轉棋」說明",
            "overview": "「翻轉棋」概述",
            "play": "玩翻轉棋",
            "difficulty": "選擇難度",
            "new_game": "開始新遊戲",
            "turn": "遊戲回合",
            "keyboard": "使用鍵盤下棋",
            "hint": "取得提示",
            "rules": "遊戲規則",
            "board": "棋盤和棋子",
            "legal": "合法走法",
            "flip": "翻轉棋子",
            "end": "遊戲結束和勝負",
            "steps": "步驟",
            "notes": "說明",
            "rules_points": "規則要點",
            "game_menu": "遊戲功能表",
            "options": "選項",
            "help_topics": "說明主題",
            "goal": "遊戲目標",
            "board_game": "棋盤遊戲",
            "red_pieces": "紅色棋子",
            "blue_pieces": "藍色棋子",
            "white_pieces": "白色棋子",
            "black_pieces": "黑色棋子",
            "pieces": "棋子",
            "piece_count": "棋子數量",
            "player": "玩家",
            "computer": "電腦",
            "opponent": "對手",
            "both_players": "雙方",
            "winner": "贏家",
            "tie_game": "平局",
            "skill": "走棋程度",
            "beginner": "初級",
            "novice": "新手",
            "expert": "高級",
            "master": "大師",
            "thinking_time": "思考時間",
            "current_game": "目前棋局",
            "restart": "重新開始",
            "first_move": "第一步",
            "player_first": "玩家先走",
            "computer_first": "電腦先走",
            "move": "落子",
            "start": "開始",
            "player_turn": "玩家回合",
            "computer_move": "電腦走棋",
            "mouse": "滑鼠",
            "cursor": "游標",
            "cross_cursor": "十字準心",
            "legal_move": "合法走法",
            "legal_square": "合法位置",
            "no_legal_move": "沒有合法走法",
            "pass": "讓棋",
            "keyboard_move": "鍵盤移動",
            "arrow_keys": "方向鍵",
            "enter_key": "Enter 鍵",
            "spacebar": "空白鍵",
            "suggested_move": "建議位置",
            "practice_game": "練習賽",
            "normal_game": "正常比賽",
            "game_end": "遊戲結束",
            "end_condition": "結束條件",
            "scoring": "勝負判定",
            "square": "格子",
            "empty_square": "空格",
            "mono_monitor": "黑白監視器",
            "outflank": "夾住",
            "line": "同一條線",
            "row": "橫線",
            "column": "直線",
            "diagonal": "斜線",
            "direction": "方向",
            "multi_direction": "多個方向",
        },
        {
            "overview_p1": "翻轉棋是一種在棋盤上輪流落子的遊戲。您使用紅色棋子，電腦使用藍色棋子。",
            "overview_p2": "落子時，必須夾住至少一個對手棋子。被夾住的棋子會翻轉成您的顏色，遊戲結束時棋子較多的一方獲勝。",
            "play_intro": "您可以依照下列步驟開始一局遊戲。",
            "play_step1": "選擇電腦的難度。",
            "play_step2": "開始新遊戲。",
            "play_step3": "在合法位置落子；如果沒有合法走法，則讓棋。",
            "difficulty_intro": "難度決定電腦的走棋程度。等級越高，電腦思考時間通常越長。",
            "difficulty_step1": "開啟「選項」功能表。",
            "difficulty_step2": "選擇初級、新手、高級或大師。",
            "difficulty_step3": "繼續目前棋局，或開始新遊戲。",
            "new_intro": "開始新遊戲會取代目前棋局。",
            "new_step1": "如果需要，先選擇難度。",
            "new_step2": "開啟「遊戲」功能表。",
            "new_step3": "選擇「開局」。",
            "new_note": "新遊戲通常由您先落子。第一步時，您也可以選擇「讓棋」，讓電腦先走。",
            "turn_intro": "輪到您時，移動游標查看棋盤上的空格。可以落子的空格會讓游標變成十字準心。",
            "turn_can1": "將游標移到一個空格上。",
            "turn_can2": "如果游標變成十字準心，按一下這個空格，或按 Enter 鍵。",
            "turn_can3": "等待電腦走棋。",
            "turn_cannot1": "如果所有空格都不能落子，請等待讓棋提示。",
            "turn_cannot2": "開啟「遊戲」功能表。",
            "turn_cannot3": "選擇「讓棋」。",
            "keyboard_intro": "您可以使用鍵盤移動棋盤游標並落子。",
            "keyboard_arrows": "移動棋盤游標。",
            "keyboard_enter": "落子。",
            "hint_intro": "如果不知道下一步怎麼走，可以使用「提示」。",
            "hint_warning": "使用「提示」會結束目前正常比賽，並進入練習賽模式。只有重新開局，才能回到正常比賽。",
            "hint_step1": "開啟「遊戲」功能表。",
            "hint_step2": "選擇「提示」。",
            "hint_step3": "查看棋盤上閃爍的建議位置。",
            "hint_note": "難度越高，電腦給出提示前可能需要更長時間。",
            "rules_intro": "翻轉棋使用同一套規則判斷落子、翻轉棋子、讓棋和勝負。",
            "rules_b1": "棋盤提供落子位置。",
            "rules_b2": "合法走法必須夾住至少一個對手棋子。",
            "rules_b3": "所有被夾住的棋子都必須翻轉。",
            "rules_b4": "一方沒有合法走法時，必須讓棋。",
            "rules_b5": "雙方都不能繼續落子時，遊戲結束，棋子較多的一方獲勝。",
            "board_intro": "棋盤提供落子用的格子。每個格子可以為空，也可以放一枚棋子。",
            "board_mono": "如果您使用黑白監視器，紅色棋子顯示為白色，藍色棋子顯示為黑色。",
            "board_board": "棋盤是八乘八的方格區域。",
            "board_pieces": "棋子表示一方佔據的格子。",
            "board_empty": "空格只有在形成合法走法時才能落子。",
            "legal_intro": "合法走法是可以落子的空格。這個空格必須夾住至少一個對手棋子。",
            "legal_step1": "落子位置必須是空格。",
            "legal_step2": "新落的棋子必須夾住至少一個對手棋子。",
            "legal_step3": "被夾住的棋子必須在同一條橫線、直線或斜線上。",
            "legal_step4": "如果游標變成十字準心，這個位置可以落子。",
            "flip_intro": "合法落子後，所有被夾住的對手棋子都會翻轉成您的顏色。",
            "flip_b1": "棋子只在形成夾住關係的方向上翻轉。",
            "flip_b2": "方向可以是橫線、直線或斜線。",
            "flip_b3": "一次落子可以同時翻轉多個方向上的棋子。",
            "end_intro": "雙方都沒有合法走法時，遊戲結束。",
            "end_condition": "遊戲可能在棋盤未填滿前結束。",
            "end_scoring": "遊戲結束後，比較棋盤上的棋子數量。",
            "end_win": "如果紅色棋子多於藍色棋子，您獲勝。",
            "popup_outflank": "夾住 是指對手棋子位於您新落的棋子和您已有的棋子之間。形成夾住關係後，這些對手棋子會翻轉成您的顏色。",
            "popup_multi": "多個方向 是指同一次落子同時在橫線、直線或斜線上形成夾住關係。",
            "how_to": "玩法",
            "can_move": "可以落子時",
            "cannot_move": "不能落子時",
            "keyboard_key": "按鍵",
            "keyboard_action": "功能",
            "related": "相關主題",
            "contents_hint": "按一下「說明主題」返回主題清單。",
        },
    ),
}


def add_locale(name, base):
    LOCALES[name] = locale(*base)


for _name, _base in {
    "ja-JP": (
        "cp932", "0x411", "ＭＳ Ｐゴシック", 128,
        {
            "game": "リバーシ", "help_title": "リバーシ ヘルプ", "overview": "リバーシの概要",
            "play": "リバーシで遊ぶ", "difficulty": "難易度を選ぶ", "new_game": "新しいゲームを始める",
            "turn": "ゲームの手番", "keyboard": "キーボードを使う", "hint": "ヒントを表示する",
            "rules": "ルール", "board": "盤面と石", "legal": "合法手", "flip": "石を返す",
            "end": "ゲームの終了と得点", "steps": "手順", "notes": "注意", "rules_points": "ルールの要点",
            "game_menu": "ゲーム メニュー", "options": "オプション", "help_topics": "ヘルプ トピック",
            "goal": "ゲームの目的", "board_game": "盤上ゲーム", "red_pieces": "赤い石", "blue_pieces": "青い石",
            "white_pieces": "白い石", "black_pieces": "黒い石", "pieces": "石", "piece_count": "石の数",
            "player": "プレイヤー", "computer": "コンピューター", "opponent": "相手", "both_players": "両者",
            "winner": "勝者", "tie_game": "引き分け", "skill": "強さ", "beginner": "初級",
            "novice": "初心者", "expert": "上級", "master": "達人", "thinking_time": "思考時間",
            "current_game": "現在のゲーム", "restart": "やり直し", "first_move": "最初の手",
            "player_first": "プレイヤーが先手", "computer_first": "コンピューターが先手", "move": "着手",
            "start": "開始", "player_turn": "プレイヤーの手番", "computer_move": "コンピューターの手",
            "mouse": "マウス", "cursor": "ポインター", "cross_cursor": "十字カーソル",
            "legal_move": "合法手", "legal_square": "打てるマス", "no_legal_move": "合法手なし",
            "pass": "パス", "keyboard_move": "キーボード操作", "arrow_keys": "方向キー",
            "enter_key": "Enter キー", "spacebar": "スペース キー", "suggested_move": "おすすめの手",
            "practice_game": "練習ゲーム", "normal_game": "通常ゲーム", "game_end": "ゲーム終了",
            "end_condition": "終了条件", "scoring": "得点計算", "square": "マス", "empty_square": "空きマス",
            "mono_monitor": "モノクロ モニター", "outflank": "挟む", "line": "直線", "row": "横方向",
            "column": "縦方向", "diagonal": "斜め方向", "direction": "方向", "multi_direction": "複数方向",
        },
        {
            "overview_p1": "リバーシは、盤面に交互に石を置くゲームです。あなたは赤い石、コンピューターは青い石を使います。",
            "overview_p2": "着手では相手の石を少なくとも 1 個挟む必要があります。挟まれた石はあなたの色に返り、終了時に石の多い側が勝ちます。",
            "play_intro": "次の手順でゲームを始めます。", "play_step1": "コンピューターの強さを選びます。",
            "play_step2": "新しいゲームを始めます。", "play_step3": "打てるマスに着手します。打てる場所がない場合はパスします。",
            "difficulty_intro": "難易度はコンピューターの読みの深さを決めます。高いレベルほど時間がかかることがあります。",
            "difficulty_step1": "オプション メニューを開きます。", "difficulty_step2": "初級、初心者、上級、達人のいずれかを選びます。",
            "difficulty_step3": "現在のゲームを続けるか、新しいゲームを始めます。",
            "new_intro": "新しいゲームを始めると、現在のゲームは置き換えられます。", "new_step1": "必要なら先に難易度を選びます。",
            "new_step2": "ゲーム メニューを開きます。", "new_step3": "新規を選びます。",
            "new_note": "通常はあなたが先に着手します。最初の手でパスを選ぶと、コンピューターを先手にできます。",
            "turn_intro": "あなたの手番では、空きマスの上にポインターを移動します。打てるマスではポインターが十字になります。",
            "turn_can1": "ポインターを空きマスに移動します。", "turn_can2": "ポインターが十字になったら、そのマスをクリックするか Enter キーを押します。",
            "turn_can3": "コンピューターの手を待ちます。", "turn_cannot1": "どこにも打てない場合は、パスのメッセージを待ちます。",
            "turn_cannot2": "ゲーム メニューを開きます。", "turn_cannot3": "パスを選びます。",
            "keyboard_intro": "キーボードで盤面カーソルを移動し、石を置くことができます。", "keyboard_arrows": "盤面カーソルを移動します。",
            "keyboard_enter": "石を置きます。", "hint_intro": "次の手がわからないときはヒントを使います。",
            "hint_warning": "ヒントを使うと現在の通常ゲームは終了し、練習モードに切り替わります。通常ゲームに戻るには新しく始めてください。",
            "hint_step1": "ゲーム メニューを開きます。", "hint_step2": "ヒントを選びます。",
            "hint_step3": "点滅するおすすめのマスを確認します。", "hint_note": "難易度が高いほど、ヒント表示まで時間がかかることがあります。",
            "rules_intro": "リバーシでは、着手、石返し、パス、得点計算に同じルールを使います。",
            "rules_b1": "盤面には石を置くマスがあります。", "rules_b2": "合法手は相手の石を少なくとも 1 個挟む必要があります。",
            "rules_b3": "挟まれた石はすべて返します。", "rules_b4": "合法手がない側はパスしなければなりません。",
            "rules_b5": "両者とも着手できなくなるとゲームは終了し、石の多い側が勝ちます。",
            "board_intro": "盤面には石を置くためのマスがあります。各マスは空きか、1 個の石を持ちます。",
            "board_mono": "モノクロ モニターでは、赤い石は白、青い石は黒で表示されます。",
            "board_board": "盤面は 8 x 8 の正方形の格子です。", "board_pieces": "石はそのマスを支配している側を表します。",
            "board_empty": "空きマスは合法手になる場合だけ使えます。",
            "legal_intro": "合法手とは、少なくとも 1 個の相手の石を挟む空きマスです。",
            "legal_step1": "着手先は空きマスでなければなりません。", "legal_step2": "新しい石は相手の石を少なくとも 1 個挟む必要があります。",
            "legal_step3": "挟まれる石は横、縦、斜めの直線上になければなりません。", "legal_step4": "ポインターが十字になれば、そのマスに打てます。",
            "flip_intro": "合法手の後、挟まれた相手の石はすべてあなたの色に返ります。",
            "flip_b1": "石はその手で作られた線上だけで返ります。", "flip_b2": "線は横、縦、斜めのいずれかです。",
            "flip_b3": "1 手で複数方向の石を返すことができます。",
            "end_intro": "両者とも合法手がなくなるとゲームは終了します。", "end_condition": "すべてのマスが埋まる前に終了することがあります。",
            "end_scoring": "終了後、盤面の石を数えます。", "end_win": "赤い石が青い石より多ければ、あなたの勝ちです。",
            "popup_outflank": "挟むとは、新しく置いた石と自分の石の間に相手の石を置くことです。挟まれた石は自分の色に返ります。",
            "popup_multi": "複数方向とは、同じ手が横、縦、斜めの複数の線で相手の石を挟むことです。",
            "how_to": "遊び方", "can_move": "打てるとき", "cannot_move": "打てないとき",
            "keyboard_key": "キー", "keyboard_action": "動作", "related": "関連トピック",
            "contents_hint": "［ヘルプ トピック］をクリックするとトピック一覧に戻ります。",
        },
    ),
    "ko-KR": (
        "cp949", "0x412", "Gulim", 129,
        {
            "game": "리버시", "help_title": "리버시 도움말", "overview": "리버시 개요",
            "play": "리버시 플레이", "difficulty": "난이도 선택", "new_game": "새 게임 시작",
            "turn": "게임 차례", "keyboard": "키보드 사용", "hint": "힌트 얻기",
            "rules": "규칙", "board": "판과 돌", "legal": "둘 수 있는 수", "flip": "돌 뒤집기",
            "end": "게임 종료와 점수", "steps": "단계", "notes": "참고", "rules_points": "규칙 요약",
            "game_menu": "게임 메뉴", "options": "옵션", "help_topics": "도움말 항목",
            "goal": "게임 목표", "board_game": "보드 게임", "red_pieces": "빨간 돌", "blue_pieces": "파란 돌",
            "white_pieces": "흰 돌", "black_pieces": "검은 돌", "pieces": "돌", "piece_count": "돌 수",
            "player": "플레이어", "computer": "컴퓨터", "opponent": "상대", "both_players": "양쪽",
            "winner": "승자", "tie_game": "무승부", "skill": "실력 수준", "beginner": "초급",
            "novice": "초보", "expert": "전문가", "master": "고수", "thinking_time": "생각 시간",
            "current_game": "현재 게임", "restart": "다시 시작", "first_move": "첫 수",
            "player_first": "플레이어 선공", "computer_first": "컴퓨터 선공", "move": "수",
            "start": "시작", "player_turn": "플레이어 차례", "computer_move": "컴퓨터 수",
            "mouse": "마우스", "cursor": "포인터", "cross_cursor": "십자 포인터",
            "legal_move": "둘 수 있는 수", "legal_square": "둘 수 있는 칸", "no_legal_move": "둘 수 없음",
            "pass": "패스", "keyboard_move": "키보드 이동", "arrow_keys": "방향키",
            "enter_key": "Enter 키", "spacebar": "스페이스바", "suggested_move": "추천 위치",
            "practice_game": "연습 게임", "normal_game": "일반 게임", "game_end": "게임 종료",
            "end_condition": "종료 조건", "scoring": "점수 계산", "square": "칸", "empty_square": "빈 칸",
            "mono_monitor": "흑백 모니터", "outflank": "사이에 두기", "line": "직선", "row": "가로줄",
            "column": "세로줄", "diagonal": "대각선", "direction": "방향", "multi_direction": "여러 방향",
        },
        {
            "overview_p1": "리버시는 판 위에 번갈아 돌을 놓는 게임입니다. 사용자는 빨간 돌을, 컴퓨터는 파란 돌을 사용합니다.",
            "overview_p2": "한 수는 상대 돌을 하나 이상 사이에 두어야 합니다. 사이에 놓인 돌은 내 색으로 뒤집히며, 끝났을 때 돌이 더 많은 쪽이 이깁니다.",
            "play_intro": "다음 단계로 게임을 시작하고 진행합니다.", "play_step1": "컴퓨터의 난이도를 선택합니다.",
            "play_step2": "새 게임을 시작합니다.", "play_step3": "둘 수 있는 칸에 두거나, 둘 수 없으면 패스합니다.",
            "difficulty_intro": "난이도는 컴퓨터가 수를 찾는 깊이를 정합니다. 높은 수준은 시간이 더 걸릴 수 있습니다.",
            "difficulty_step1": "옵션 메뉴를 엽니다.", "difficulty_step2": "초급, 초보, 전문가 또는 고수를 선택합니다.",
            "difficulty_step3": "현재 게임을 계속하거나 새 게임을 시작합니다.",
            "new_intro": "새 게임을 시작하면 현재 게임이 바뀝니다.", "new_step1": "필요하면 먼저 난이도를 선택합니다.",
            "new_step2": "게임 메뉴를 엽니다.", "new_step3": "새로 만들기를 선택합니다.",
            "new_note": "보통 사용자가 먼저 둡니다. 첫 차례에 패스를 선택하면 컴퓨터가 먼저 둘 수 있습니다.",
            "turn_intro": "사용자 차례에는 빈 칸 위로 포인터를 움직입니다. 둘 수 있는 칸에서는 포인터가 십자 모양으로 바뀝니다.",
            "turn_can1": "포인터를 빈 칸으로 이동합니다.", "turn_can2": "포인터가 십자가 되면 그 칸을 클릭하거나 Enter 키를 누릅니다.",
            "turn_can3": "컴퓨터가 둘 때까지 기다립니다.", "turn_cannot1": "둘 수 있는 칸이 없으면 패스 메시지를 기다립니다.",
            "turn_cannot2": "게임 메뉴를 엽니다.", "turn_cannot3": "패스를 선택합니다.",
            "keyboard_intro": "키보드로 판 커서를 이동하고 돌을 놓을 수 있습니다.", "keyboard_arrows": "판 커서를 이동합니다.",
            "keyboard_enter": "돌을 놓습니다.", "hint_intro": "어디에 둘지 모를 때 힌트를 사용합니다.",
            "hint_warning": "힌트를 사용하면 현재 일반 게임이 끝나고 연습 모드로 전환됩니다. 일반 게임으로 돌아가려면 새 게임을 시작하십시오.",
            "hint_step1": "게임 메뉴를 엽니다.", "hint_step2": "힌트를 선택합니다.", "hint_step3": "깜박이는 추천 칸을 봅니다.",
            "hint_note": "난이도가 높을수록 힌트가 표시되기까지 시간이 더 걸릴 수 있습니다.",
            "rules_intro": "리버시는 두기, 뒤집기, 패스, 점수 계산에 같은 규칙을 사용합니다.",
            "rules_b1": "판에는 돌을 놓을 칸이 있습니다.", "rules_b2": "둘 수 있는 수는 상대 돌을 하나 이상 사이에 두어야 합니다.",
            "rules_b3": "사이에 놓인 돌은 모두 뒤집습니다.", "rules_b4": "둘 수 있는 수가 없으면 반드시 패스해야 합니다.",
            "rules_b5": "양쪽 모두 둘 수 없으면 게임이 끝나고 돌이 더 많은 쪽이 이깁니다.",
            "board_intro": "판에는 돌을 놓는 칸이 있습니다. 각 칸은 비어 있거나 돌 하나를 가집니다.",
            "board_mono": "흑백 모니터에서는 빨간 돌이 흰 돌로, 파란 돌이 검은 돌로 표시됩니다.",
            "board_board": "판은 8 x 8 정사각 격자입니다.", "board_pieces": "돌은 어느 쪽이 칸을 차지했는지를 나타냅니다.",
            "board_empty": "빈 칸은 합법적인 수일 때만 사용할 수 있습니다.",
            "legal_intro": "둘 수 있는 수는 상대 돌을 하나 이상 사이에 두는 빈 칸입니다.",
            "legal_step1": "둘 칸은 비어 있어야 합니다.", "legal_step2": "새 돌은 상대 돌을 하나 이상 사이에 두어야 합니다.",
            "legal_step3": "사이에 놓이는 돌은 가로, 세로 또는 대각선의 한 직선에 있어야 합니다.",
            "legal_step4": "포인터가 십자가 되면 그 칸에 둘 수 있습니다.",
            "flip_intro": "합법적인 수를 둔 뒤에는 사이에 놓인 상대 돌이 모두 내 색으로 뒤집힙니다.",
            "flip_b1": "돌은 그 수로 만들어진 줄에서만 뒤집힙니다.", "flip_b2": "줄은 가로, 세로 또는 대각선일 수 있습니다.",
            "flip_b3": "한 수로 여러 방향의 돌을 뒤집을 수 있습니다.",
            "end_intro": "양쪽 모두 둘 수 있는 수가 없으면 게임이 끝납니다.", "end_condition": "모든 칸이 차기 전에 게임이 끝날 수도 있습니다.",
            "end_scoring": "게임이 끝나면 판 위의 돌을 셉니다.", "end_win": "빨간 돌이 파란 돌보다 많으면 사용자가 이깁니다.",
            "popup_outflank": "사이에 두기란 새 돌과 내 돌 사이에 상대 돌이 놓이도록 두는 것입니다. 그 돌들은 내 색으로 뒤집힙니다.",
            "popup_multi": "여러 방향은 한 수가 가로, 세로 또는 대각선 여러 줄에서 상대 돌을 사이에 두는 경우입니다.",
            "how_to": "게임 방법", "can_move": "둘 수 있을 때", "cannot_move": "둘 수 없을 때",
            "keyboard_key": "키", "keyboard_action": "동작", "related": "관련 항목",
            "contents_hint": "항목 목록으로 돌아가려면 도움말 항목을 클릭하십시오.",
        },
    ),
}.items():
    add_locale(_name, _base)


WESTERN_LOCALE_TEXT = {
    "fr-FR": (
        "cp1252", "0x40c", "Arial", 0,
        ("Reversi", "Aide de Reversi", "Présentation de Reversi", "Jouer à Reversi", "Choisir le niveau",
         "Commencer une nouvelle partie", "Tours de jeu", "Utiliser le clavier", "Obtenir un indice",
         "Règles", "Plateau et pions", "Coups légaux", "Retourner les pions", "Fin de partie et score",
         "pions rouges", "pions bleus", "pion", "pions", "case", "case vide", "coup légal",
         "passer", "indice", "encadrer", "retourner", "plusieurs directions", "ordinateur"),
    ),
    "de-DE": (
        "cp1252", "0x407", "Arial", 0,
        ("Reversi", "Reversi-Hilfe", "Reversi-Übersicht", "Reversi spielen", "Spielstärke wählen",
         "Neues Spiel starten", "Spielzüge", "Tastatur verwenden", "Hinweis anzeigen",
         "Regeln", "Brett und Steine", "Gültige Züge", "Steine umdrehen", "Spielende und Wertung",
         "rote Steine", "blaue Steine", "Stein", "Steine", "Feld", "leeres Feld", "gültiger Zug",
         "passen", "Hinweis", "einschließen", "umdrehen", "mehrere Richtungen", "Computer"),
    ),
    "es-ES": (
        "cp1252", "0x40a", "Arial", 0,
        ("Reversi", "Ayuda de Reversi", "Información general de Reversi", "Jugar a Reversi", "Elegir dificultad",
         "Iniciar una partida nueva", "Turnos de juego", "Usar el teclado", "Obtener una pista",
         "Reglas", "Tablero y fichas", "Jugadas válidas", "Voltear fichas", "Final y puntuación",
         "fichas rojas", "fichas azules", "ficha", "fichas", "casilla", "casilla vacía", "jugada válida",
         "pasar", "pista", "flanquear", "voltear", "varias direcciones", "equipo"),
    ),
    "sv-SE": (
        "cp1252", "0x41d", "Arial", 0,
        ("Reversi", "Hjälp för Reversi", "Översikt över Reversi", "Spela Reversi", "Välja nivå",
         "Starta ett nytt spel", "Spelturer", "Använda tangentbordet", "Få ett tips",
         "Regler", "Bräde och brickor", "Tillåtna drag", "Vända brickor", "Spelslut och poäng",
         "röda brickor", "blå brickor", "bricka", "brickor", "ruta", "tom ruta", "tillåtet drag",
         "passa", "tips", "omringa", "vända", "flera riktningar", "datorn"),
    ),
    "fi-FI": (
        "cp1252", "0x40b", "Arial", 0,
        ("Reversi", "Reversin ohje", "Reversin yleiskatsaus", "Reversin pelaaminen", "Vaikeustason valitseminen",
         "Uuden pelin aloittaminen", "Pelivuorot", "Näppäimistön käyttäminen", "Vihjeen saaminen",
         "Säännöt", "Lauta ja nappulat", "Sallitut siirrot", "Nappuloiden kääntäminen", "Pelin loppu ja pisteet",
         "punaiset nappulat", "siniset nappulat", "nappula", "nappulat", "ruutu", "tyhjä ruutu", "sallittu siirto",
         "passata", "vihje", "saartaa", "kääntää", "useita suuntia", "tietokone"),
    ),
    "pt-PT": (
        "cp1252", "0x816", "Arial", 0,
        ("Reversi", "Ajuda do Reversi", "Descrição geral do Reversi", "Jogar Reversi", "Escolher a dificuldade",
         "Iniciar um novo jogo", "Turnos do jogo", "Utilizar o teclado", "Obter uma sugestão",
         "Regras", "Tabuleiro e peças", "Jogadas válidas", "Virar peças", "Fim do jogo e pontuação",
         "peças vermelhas", "peças azuis", "peça", "peças", "casa", "casa vazia", "jogada válida",
         "passar", "sugestão", "flanquear", "virar", "várias direções", "computador"),
    ),
    "it-IT": (
        "cp1252", "0x410", "Arial", 0,
        ("Reversi", "Guida di Reversi", "Panoramica di Reversi", "Giocare a Reversi", "Scegliere il livello",
         "Iniziare una nuova partita", "Turni di gioco", "Usare la tastiera", "Ottenere un suggerimento",
         "Regole", "Scacchiera e pedine", "Mosse valide", "Girare le pedine", "Fine partita e punteggio",
         "pedine rosse", "pedine blu", "pedina", "pedine", "casella", "casella vuota", "mossa valida",
         "passare", "suggerimento", "circondare", "girare", "più direzioni", "computer"),
    ),
}


def add_western_locale(name, payload):
    encoding, lcid, font, charset, values = payload
    (
        game, help_title, overview, play, difficulty, new_game, turn, keyboard, hint,
        rules, board, legal, flip, end, red_pieces, blue_pieces, piece, pieces,
        square, empty_square, legal_move, pass_word, hint_word, outflank, flip_word,
        multi_direction, computer,
    ) = values

    terms = {
        "game": game, "help_title": help_title, "overview": overview, "play": play,
        "difficulty": difficulty, "new_game": new_game, "turn": turn, "keyboard": keyboard,
        "hint": hint, "rules": rules, "board": board, "legal": legal, "flip": flip,
        "end": end, "steps": "Steps", "notes": "Notes", "rules_points": rules,
        "game_menu": "Game", "options": "Options", "help_topics": "Help Topics",
        "goal": "object", "board_game": "board game", "red_pieces": red_pieces,
        "blue_pieces": blue_pieces, "white_pieces": "white pieces", "black_pieces": "black pieces",
        "pieces": pieces, "piece_count": f"{pieces} count", "player": "player",
        "computer": computer, "opponent": "opponent", "both_players": "both players",
        "winner": "winner", "tie_game": "tie game", "skill": difficulty,
        "beginner": "Beginner", "novice": "Novice", "expert": "Expert", "master": "Master",
        "thinking_time": "thinking time", "current_game": "current game", "restart": "restart",
        "first_move": "first move", "player_first": "player moves first",
        "computer_first": f"{computer} moves first", "move": legal_move, "start": "start",
        "player_turn": "player turn", "computer_move": f"{computer} move", "mouse": "mouse",
        "cursor": "cursor", "cross_cursor": "cross cursor", "legal_move": legal_move,
        "legal_square": square, "no_legal_move": f"no {legal_move}", "pass": pass_word,
        "keyboard_move": "keyboard move", "arrow_keys": "arrow keys", "enter_key": "Enter",
        "spacebar": "Spacebar", "suggested_move": f"{hint_word} move", "practice_game": "practice game",
        "normal_game": "normal game", "game_end": "game end", "end_condition": "end condition",
        "scoring": "scoring", "square": square, "empty_square": empty_square,
        "mono_monitor": "monochrome monitor", "outflank": outflank, "line": "line",
        "row": "row", "column": "column", "diagonal": "diagonal", "direction": "direction",
        "multi_direction": multi_direction,
    }
    text = {
        "overview_p1": f"{game} is a board game where you and the {computer} take turns placing {pieces}. You use {red_pieces}; the {computer} uses {blue_pieces}.",
        "overview_p2": f"A move must {outflank} at least one opposing {piece}. {pieces.capitalize()} that are {outflank}ed are {flip_word}ed to your color, and the side with more {pieces} at the end wins.",
        "play_intro": "Use these steps to start and play a game.",
        "play_step1": f"Choose the {computer}'s difficulty.",
        "play_step2": new_game + ".",
        "play_step3": f"Play on a {legal_move}, or {pass_word} when no move is available.",
        "difficulty_intro": f"{difficulty} controls how carefully the {computer} searches for a move.",
        "difficulty_step1": "Open the Options menu.", "difficulty_step2": "Choose one of the four levels.",
        "difficulty_step3": "Continue the current game, or start a new one.",
        "new_intro": f"{new_game} replaces the current game.", "new_step1": "Choose a difficulty first, if needed.",
        "new_step2": "Open the Game menu.", "new_step3": "Choose New.",
        "new_note": f"You normally move first. On the first turn, you may also choose {pass_word} to let the {computer} move first.",
        "turn_intro": f"When it is your turn, move the pointer over {empty_square}s. A playable {square} changes the pointer to a cross.",
        "turn_can1": f"Move the pointer to an {empty_square}.", "turn_can2": "If the pointer becomes a cross, click the square or press Enter.",
        "turn_can3": f"Wait for the {computer} to move.", "turn_cannot1": f"If no {square} can be played, wait for the {pass_word} message.",
        "turn_cannot2": "Open the Game menu.", "turn_cannot3": f"Choose {pass_word}.",
        "keyboard_intro": "You can use the keyboard to move the board cursor and place a piece.",
        "keyboard_arrows": "Move the board cursor.", "keyboard_enter": "Place a piece.",
        "hint_intro": f"Use {hint_word} if you are not sure where to move.",
        "hint_warning": f"Using {hint_word} ends the current normal game and switches to practice mode. Start a new game to return to normal play.",
        "hint_step1": "Open the Game menu.", "hint_step2": f"Choose {hint_word}.", "hint_step3": "Watch the highlighted suggested square.",
        "hint_note": "At higher skill levels, the computer may need more time before showing the hint.",
        "rules_intro": f"{game} uses one set of rules for moving, flipping pieces, passing, and scoring.",
        "rules_b1": f"The board contains the {square}s where pieces can be placed.",
        "rules_b2": f"A {legal_move} must {outflank} at least one opposing {piece}.",
        "rules_b3": f"All outflanked {pieces} are {flip_word}ed.",
        "rules_b4": f"If a side has no {legal_move}, that side must {pass_word}.",
        "rules_b5": f"When neither side can move, the game ends and the side with more {pieces} wins.",
        "board_intro": f"The board supplies the playing {square}s. Each {square} is empty or contains one {piece}.",
        "board_mono": f"On a monochrome monitor, {red_pieces} are shown as white pieces and {blue_pieces} are shown as black pieces.",
        "board_board": "The board is an eight-by-eight square grid.", "board_pieces": f"{pieces.capitalize()} show which side controls a {square}.",
        "board_empty": f"An {empty_square} can be used only if the move is legal.",
        "legal_intro": f"A {legal_move} is an {empty_square} that {outflank}s at least one opposing {piece}.",
        "legal_step1": f"The {square} must be empty.", "legal_step2": f"The new {piece} must {outflank} at least one opposing {piece}.",
        "legal_step3": "The outflanked pieces must be in one straight horizontal, vertical, or diagonal line.",
        "legal_step4": f"If the pointer becomes a cross, the {square} is legal.",
        "flip_intro": f"After a legal move, every outflanked opposing {piece} is {flip_word}ed to your color.",
        "flip_b1": "Pieces flip only on lines created by the move.", "flip_b2": "Lines can be horizontal, vertical, or diagonal.",
        "flip_b3": f"One move can flip pieces in {multi_direction}.",
        "end_intro": "The game ends when neither side can make another legal move.",
        "end_condition": "A game may end before every square is filled.", "end_scoring": "When the game ends, the pieces are counted.",
        "end_win": f"If your {red_pieces} outnumber the {computer}'s {blue_pieces}, you win.",
        "popup_outflank": f"To {outflank} means to place a {piece} so that one or more opposing {pieces} are between the new {piece} and another {piece} of your color.",
        "popup_multi": f"{multi_direction.capitalize()} means that the same move outflanks pieces along more than one horizontal, vertical, or diagonal line.",
        "how_to": "How to play", "can_move": "When you can move", "cannot_move": "When you cannot move",
        "keyboard_key": "Key", "keyboard_action": "Action", "related": "Related Topics",
        "contents_hint": "Click Help Topics to return to the topic list.",
    }
    add_locale(name, (encoding, lcid, font, charset, terms, text))


for _name, _payload in WESTERN_LOCALE_TEXT.items():
    add_western_locale(_name, _payload)

WESTERN_OVERRIDES = {
    "fr-FR": {
        "terms": {
            "steps": "Étapes", "notes": "Remarques", "rules_points": "Résumé des règles",
            "game_menu": "menu Jeu", "help_topics": "rubriques d'aide", "goal": "but du jeu",
            "board_game": "jeu de plateau", "white_pieces": "pions blancs", "black_pieces": "pions noirs",
            "player": "joueur", "opponent": "adversaire", "both_players": "les deux joueurs",
            "winner": "vainqueur", "tie_game": "partie nulle", "skill": "niveau",
            "beginner": "Débutant", "novice": "Novice", "expert": "Expert", "master": "Maître",
            "thinking_time": "temps de réflexion", "current_game": "partie en cours",
            "restart": "recommencer", "first_move": "premier coup", "player_first": "le joueur commence",
            "computer_first": "l'ordinateur commence", "move": "coup", "start": "commencer",
            "player_turn": "tour du joueur", "computer_move": "coup de l'ordinateur", "mouse": "souris",
            "cursor": "pointeur", "cross_cursor": "pointeur en croix", "no_legal_move": "aucun coup légal",
            "keyboard_move": "coup au clavier", "arrow_keys": "touches de direction",
            "enter_key": "touche Entrée", "spacebar": "barre d'espace", "practice_game": "partie d'entraînement",
            "normal_game": "partie normale", "game_end": "fin de partie", "end_condition": "condition de fin",
            "scoring": "score", "mono_monitor": "écran monochrome", "line": "ligne",
            "row": "ligne horizontale", "column": "colonne", "diagonal": "diagonale", "direction": "direction",
        },
        "text": {
            "overview_p1": "Reversi est un jeu de plateau dans lequel vous et l'ordinateur placez des pions à tour de rôle. Vous utilisez les pions rouges ; l'ordinateur utilise les pions bleus.",
            "overview_p2": "Un coup doit encadrer au moins un pion adverse. Les pions encadrés sont retournés à votre couleur ; à la fin, le joueur qui possède le plus de pions gagne.",
            "play_intro": "Suivez ces étapes pour commencer et jouer une partie.",
            "play_step1": "Choisissez le niveau de l'ordinateur.", "play_step2": "Commencez une nouvelle partie.",
            "play_step3": "Jouez sur une case légale ou passez lorsqu'aucun coup n'est possible.",
            "difficulty_intro": "Le niveau détermine la profondeur de recherche de l'ordinateur. Les niveaux élevés peuvent demander plus de temps.",
            "difficulty_step1": "Ouvrez le menu Options.", "difficulty_step2": "Choisissez Débutant, Novice, Expert ou Maître.",
            "difficulty_step3": "Continuez la partie en cours ou commencez-en une nouvelle.",
            "new_intro": "Une nouvelle partie remplace la partie en cours.", "new_step1": "Choisissez d'abord le niveau si nécessaire.",
            "new_step2": "Ouvrez le menu Jeu.", "new_step3": "Choisissez Nouvelle.",
            "new_note": "Vous jouez normalement le premier coup. Au premier tour, vous pouvez aussi choisir Passer pour laisser l'ordinateur commencer.",
            "turn_intro": "Lorsque c'est votre tour, déplacez le pointeur sur les cases vides. Une case jouable transforme le pointeur en croix.",
            "turn_can1": "Déplacez le pointeur sur une case vide.", "turn_can2": "Si le pointeur devient une croix, cliquez sur la case ou appuyez sur Entrée.",
            "turn_can3": "Attendez le coup de l'ordinateur.", "turn_cannot1": "Si aucune case n'est jouable, attendez le message de passe.",
            "turn_cannot2": "Ouvrez le menu Jeu.", "turn_cannot3": "Choisissez Passer.",
            "keyboard_intro": "Vous pouvez utiliser le clavier pour déplacer le curseur du plateau et placer un pion.",
            "keyboard_arrows": "Déplacent le curseur du plateau.", "keyboard_enter": "Place un pion.",
            "hint_intro": "Utilisez Indice si vous ne savez pas où jouer.",
            "hint_warning": "Utiliser Indice termine la partie normale en cours et passe en mode entraînement. Commencez une nouvelle partie pour revenir au mode normal.",
            "hint_step1": "Ouvrez le menu Jeu.", "hint_step2": "Choisissez Indice.", "hint_step3": "Regardez la case suggérée qui clignote.",
            "hint_note": "Aux niveaux élevés, l'ordinateur peut avoir besoin de plus de temps avant d'afficher l'indice.",
            "rules_intro": "Reversi utilise les mêmes règles pour jouer, retourner les pions, passer et compter le score.",
            "rules_b1": "Le plateau contient les cases où placer les pions.", "rules_b2": "Un coup légal doit encadrer au moins un pion adverse.",
            "rules_b3": "Tous les pions encadrés sont retournés.", "rules_b4": "Si un joueur n'a aucun coup légal, il doit passer.",
            "rules_b5": "Lorsque les deux joueurs ne peuvent plus jouer, la partie se termine et le joueur qui possède le plus de pions gagne.",
            "board_intro": "Le plateau fournit les cases de jeu. Chaque case est vide ou contient un pion.",
            "board_mono": "Sur un écran monochrome, les pions rouges sont blancs et les pions bleus sont noirs.",
            "board_board": "Le plateau est une grille carrée de huit cases sur huit.", "board_pieces": "Les pions indiquent quel joueur occupe une case.",
            "board_empty": "Une case vide ne peut être utilisée que si le coup est légal.",
            "legal_intro": "Un coup légal est une case vide qui encadre au moins un pion adverse.",
            "legal_step1": "La case doit être vide.", "legal_step2": "Le nouveau pion doit encadrer au moins un pion adverse.",
            "legal_step3": "Les pions encadrés doivent se trouver sur une même ligne horizontale, verticale ou diagonale.",
            "legal_step4": "Si le pointeur devient une croix, la case est légale.",
            "flip_intro": "Après un coup légal, tous les pions adverses encadrés sont retournés à votre couleur.",
            "flip_b1": "Les pions ne se retournent que sur les lignes créées par le coup.", "flip_b2": "Les lignes peuvent être horizontales, verticales ou diagonales.",
            "flip_b3": "Un seul coup peut retourner des pions dans plusieurs directions.",
            "end_intro": "La partie se termine lorsqu'aucun joueur ne peut jouer de coup légal.",
            "end_condition": "Une partie peut se terminer avant que toutes les cases soient remplies.", "end_scoring": "À la fin, les pions sont comptés.",
            "end_win": "Si vos pions rouges sont plus nombreux que les pions bleus de l'ordinateur, vous gagnez.",
            "popup_outflank": "Encadrer signifie placer un pion de façon qu'un ou plusieurs pions adverses soient entre ce pion et un autre pion de votre couleur.",
            "popup_multi": "Plusieurs directions signifie qu'un même coup encadre des pions sur plusieurs lignes horizontales, verticales ou diagonales.",
            "how_to": "Comment jouer", "can_move": "Lorsque vous pouvez jouer", "cannot_move": "Lorsque vous ne pouvez pas jouer",
            "keyboard_key": "Touche", "keyboard_action": "Action", "related": "Rubriques connexes",
            "contents_hint": "Cliquez sur Rubriques d'aide pour revenir à la liste des rubriques.",
        },
    },
    "de-DE": {
        "terms": {
            "steps": "Schritte", "notes": "Hinweise", "rules_points": "Regelübersicht",
            "game_menu": "Menü Spiel", "help_topics": "Hilfethemen", "goal": "Ziel des Spiels",
            "board_game": "Brettspiel", "white_pieces": "weiße Steine", "black_pieces": "schwarze Steine",
            "player": "Spieler", "opponent": "Gegner", "both_players": "beide Spieler",
            "winner": "Gewinner", "tie_game": "Unentschieden", "skill": "Spielstärke",
            "beginner": "Anfänger", "novice": "Neuling", "expert": "Experte", "master": "Meister",
            "thinking_time": "Bedenkzeit", "current_game": "aktuelles Spiel", "restart": "neu starten",
            "first_move": "erster Zug", "player_first": "Spieler beginnt", "computer_first": "Computer beginnt",
            "move": "Zug", "start": "starten", "player_turn": "Zug des Spielers", "computer_move": "Zug des Computers",
            "mouse": "Maus", "cursor": "Zeiger", "cross_cursor": "Kreuzzeiger",
            "no_legal_move": "kein gültiger Zug", "keyboard_move": "Tastaturzug",
            "arrow_keys": "Pfeiltasten", "enter_key": "Eingabetaste", "spacebar": "Leertaste",
            "practice_game": "Übungsspiel", "normal_game": "normales Spiel", "game_end": "Spielende",
            "end_condition": "Endbedingung", "scoring": "Wertung", "mono_monitor": "Schwarzweißmonitor",
            "line": "Linie", "row": "waagerechte Linie", "column": "senkrechte Linie", "diagonal": "Diagonale",
        },
        "text": {
            "overview_p1": "Reversi ist ein Brettspiel, in dem Sie und der Computer abwechselnd Steine setzen. Sie spielen mit roten Steinen, der Computer mit blauen.",
            "overview_p2": "Ein Zug muss mindestens einen gegnerischen Stein einschließen. Eingeschlossene Steine werden in Ihre Farbe umgedreht; am Ende gewinnt die Seite mit den meisten Steinen.",
            "play_intro": "Mit diesen Schritten starten und spielen Sie eine Partie.", "play_step1": "Wählen Sie die Spielstärke des Computers.",
            "play_step2": "Starten Sie ein neues Spiel.", "play_step3": "Ziehen Sie auf ein gültiges Feld oder passen Sie, wenn kein Zug möglich ist.",
            "difficulty_intro": "Die Spielstärke bestimmt, wie gründlich der Computer einen Zug sucht. Höhere Stufen können länger dauern.",
            "difficulty_step1": "Öffnen Sie das Menü Optionen.", "difficulty_step2": "Wählen Sie Anfänger, Neuling, Experte oder Meister.",
            "difficulty_step3": "Setzen Sie das aktuelle Spiel fort oder starten Sie ein neues.",
            "new_intro": "Ein neues Spiel ersetzt das aktuelle Spiel.", "new_step1": "Wählen Sie bei Bedarf zuerst die Spielstärke.",
            "new_step2": "Öffnen Sie das Menü Spiel.", "new_step3": "Wählen Sie Neu.",
            "new_note": "Normalerweise ziehen Sie zuerst. Im ersten Zug können Sie auch Passen wählen, damit der Computer beginnt.",
            "turn_intro": "Wenn Sie am Zug sind, bewegen Sie den Zeiger über leere Felder. Auf einem spielbaren Feld wird der Zeiger zum Kreuz.",
            "turn_can1": "Bewegen Sie den Zeiger auf ein leeres Feld.", "turn_can2": "Wenn der Zeiger ein Kreuz wird, klicken Sie auf das Feld oder drücken Sie die Eingabetaste.",
            "turn_can3": "Warten Sie auf den Zug des Computers.", "turn_cannot1": "Wenn kein Feld spielbar ist, warten Sie auf die Passen-Meldung.",
            "turn_cannot2": "Öffnen Sie das Menü Spiel.", "turn_cannot3": "Wählen Sie Passen.",
            "keyboard_intro": "Sie können den Brettcursor mit der Tastatur bewegen und einen Stein setzen.", "keyboard_arrows": "Bewegen den Brettcursor.",
            "keyboard_enter": "Setzt einen Stein.", "hint_intro": "Verwenden Sie Hinweis, wenn Sie nicht wissen, wohin Sie ziehen sollen.",
            "hint_warning": "Ein Hinweis beendet das aktuelle normale Spiel und wechselt in den Übungsmodus. Starten Sie ein neues Spiel, um zum normalen Spiel zurückzukehren.",
            "hint_step1": "Öffnen Sie das Menü Spiel.", "hint_step2": "Wählen Sie Hinweis.", "hint_step3": "Beachten Sie das blinkende vorgeschlagene Feld.",
            "hint_note": "Auf höheren Stufen benötigt der Computer vor dem Hinweis möglicherweise mehr Zeit.",
            "rules_intro": "Reversi verwendet dieselben Regeln für Züge, Umdrehen, Passen und Wertung.",
            "rules_b1": "Das Brett enthält die Felder, auf die Steine gesetzt werden.", "rules_b2": "Ein gültiger Zug muss mindestens einen gegnerischen Stein einschließen.",
            "rules_b3": "Alle eingeschlossenen Steine werden umgedreht.", "rules_b4": "Wenn eine Seite keinen gültigen Zug hat, muss sie passen.",
            "rules_b5": "Wenn keine Seite mehr ziehen kann, endet das Spiel; die Seite mit den meisten Steinen gewinnt.",
            "board_intro": "Das Brett stellt die Spielfelder bereit. Jedes Feld ist leer oder enthält einen Stein.",
            "board_mono": "Auf einem Schwarzweißmonitor werden rote Steine weiß und blaue Steine schwarz angezeigt.",
            "board_board": "Das Brett ist ein quadratisches Raster mit acht mal acht Feldern.", "board_pieces": "Steine zeigen, welche Seite ein Feld besitzt.",
            "board_empty": "Ein leeres Feld kann nur verwendet werden, wenn der Zug gültig ist.",
            "legal_intro": "Ein gültiger Zug ist ein leeres Feld, das mindestens einen gegnerischen Stein einschließt.",
            "legal_step1": "Das Feld muss leer sein.", "legal_step2": "Der neue Stein muss mindestens einen gegnerischen Stein einschließen.",
            "legal_step3": "Die eingeschlossenen Steine müssen auf einer geraden waagerechten, senkrechten oder diagonalen Linie liegen.",
            "legal_step4": "Wenn der Zeiger ein Kreuz wird, ist das Feld gültig.",
            "flip_intro": "Nach einem gültigen Zug werden alle eingeschlossenen gegnerischen Steine in Ihre Farbe umgedreht.",
            "flip_b1": "Steine werden nur auf Linien umgedreht, die durch den Zug entstehen.", "flip_b2": "Linien können waagerecht, senkrecht oder diagonal sein.",
            "flip_b3": "Ein Zug kann Steine in mehreren Richtungen umdrehen.",
            "end_intro": "Das Spiel endet, wenn keine Seite noch einen gültigen Zug machen kann.",
            "end_condition": "Eine Partie kann enden, bevor alle Felder gefüllt sind.", "end_scoring": "Am Spielende werden die Steine gezählt.",
            "end_win": "Wenn Ihre roten Steine zahlreicher sind als die blauen Steine des Computers, gewinnen Sie.",
            "popup_outflank": "Einschließen bedeutet, einen Stein so zu setzen, dass ein oder mehrere gegnerische Steine zwischen dem neuen Stein und einem Stein Ihrer Farbe liegen.",
            "popup_multi": "Mehrere Richtungen bedeutet, dass derselbe Zug Steine auf mehr als einer waagerechten, senkrechten oder diagonalen Linie einschließt.",
            "how_to": "Spielanleitung", "can_move": "Wenn Sie ziehen können", "cannot_move": "Wenn Sie nicht ziehen können",
            "keyboard_key": "Taste", "keyboard_action": "Aktion", "related": "Verwandte Themen",
            "contents_hint": "Klicken Sie auf Hilfethemen, um zur Themenliste zurückzukehren.",
        },
    },
    "es-ES": {
        "terms": {
            "steps": "Pasos", "notes": "Notas", "rules_points": "Resumen de reglas",
            "game_menu": "menú Juego", "help_topics": "temas de ayuda", "goal": "objetivo del juego",
            "board_game": "juego de tablero", "white_pieces": "fichas blancas", "black_pieces": "fichas negras",
            "player": "jugador", "opponent": "oponente", "both_players": "ambos jugadores",
            "winner": "ganador", "tie_game": "empate", "skill": "nivel",
            "beginner": "Principiante", "novice": "Novato", "expert": "Experto", "master": "Maestro",
            "thinking_time": "tiempo de cálculo", "current_game": "partida actual", "restart": "reiniciar",
            "first_move": "primer movimiento", "player_first": "el jugador empieza", "computer_first": "el equipo empieza",
            "move": "jugada", "start": "iniciar", "player_turn": "turno del jugador", "computer_move": "jugada del equipo",
            "mouse": "mouse", "cursor": "puntero", "cross_cursor": "puntero en cruz",
            "no_legal_move": "sin jugada válida", "keyboard_move": "jugada con teclado",
            "arrow_keys": "teclas de dirección", "enter_key": "tecla Entrar", "spacebar": "barra espaciadora",
            "practice_game": "partida de práctica", "normal_game": "partida normal", "game_end": "fin de partida",
            "end_condition": "condición de final", "scoring": "puntuación", "mono_monitor": "monitor monocromo",
            "line": "línea", "row": "fila", "column": "columna", "diagonal": "diagonal",
        },
        "text": {
            "overview_p1": "Reversi es un juego de tablero en el que usted y el equipo colocan fichas por turnos. Usted usa fichas rojas; el equipo usa fichas azules.",
            "overview_p2": "Una jugada debe flanquear al menos una ficha del oponente. Las fichas flanqueadas se vuelven de su color; al final gana quien tenga más fichas.",
            "play_intro": "Use estos pasos para iniciar y jugar una partida.",
            "play_step1": "Elija la dificultad del equipo.", "play_step2": "Inicie una partida nueva.",
            "play_step3": "Juegue en una casilla válida o pase si no hay jugada disponible.",
            "difficulty_intro": "La dificultad controla cuánto busca el equipo una jugada. Los niveles altos pueden tardar más.",
            "difficulty_step1": "Abra el menú Opciones.", "difficulty_step2": "Elija Principiante, Novato, Experto o Maestro.",
            "difficulty_step3": "Continúe la partida actual o inicie una nueva.",
            "new_intro": "Una partida nueva reemplaza la partida actual.", "new_step1": "Si es necesario, elija primero la dificultad.",
            "new_step2": "Abra el menú Juego.", "new_step3": "Elija Nuevo.",
            "new_note": "Normalmente usted mueve primero. En el primer turno también puede elegir Pasar para que empiece el equipo.",
            "turn_intro": "Cuando sea su turno, mueva el puntero sobre las casillas vacías. Una casilla jugable cambia el puntero a una cruz.",
            "turn_can1": "Mueva el puntero a una casilla vacía.", "turn_can2": "Si el puntero se convierte en una cruz, haga clic en la casilla o presione Entrar.",
            "turn_can3": "Espere la jugada del equipo.", "turn_cannot1": "Si no se puede jugar en ninguna casilla, espere el mensaje de pasar.",
            "turn_cannot2": "Abra el menú Juego.", "turn_cannot3": "Elija Pasar.",
            "keyboard_intro": "Puede usar el teclado para mover el cursor del tablero y colocar una ficha.",
            "keyboard_arrows": "Mueven el cursor del tablero.", "keyboard_enter": "Coloca una ficha.",
            "hint_intro": "Use Pista si no sabe dónde jugar.",
            "hint_warning": "Usar Pista finaliza la partida normal actual y cambia al modo de práctica. Inicie una partida nueva para volver al juego normal.",
            "hint_step1": "Abra el menú Juego.", "hint_step2": "Elija Pista.", "hint_step3": "Observe la casilla sugerida que parpadea.",
            "hint_note": "En niveles altos, el equipo puede tardar más en mostrar la pista.",
            "rules_intro": "Reversi usa las mismas reglas para mover, voltear fichas, pasar y puntuar.",
            "rules_b1": "El tablero contiene las casillas donde se colocan las fichas.", "rules_b2": "Una jugada válida debe flanquear al menos una ficha del oponente.",
            "rules_b3": "Todas las fichas flanqueadas se voltean.", "rules_b4": "Si un lado no tiene jugada válida, debe pasar.",
            "rules_b5": "Cuando ningún lado puede mover, termina la partida y gana quien tenga más fichas.",
            "board_intro": "El tablero proporciona las casillas de juego. Cada casilla está vacía o contiene una ficha.",
            "board_mono": "En un monitor monocromo, las fichas rojas se muestran blancas y las fichas azules se muestran negras.",
            "board_board": "El tablero es una cuadrícula de ocho por ocho.", "board_pieces": "Las fichas indican qué lado controla una casilla.",
            "board_empty": "Una casilla vacía solo puede usarse si la jugada es válida.",
            "legal_intro": "Una jugada válida es una casilla vacía que flanquea al menos una ficha del oponente.",
            "legal_step1": "La casilla debe estar vacía.", "legal_step2": "La ficha nueva debe flanquear al menos una ficha del oponente.",
            "legal_step3": "Las fichas flanqueadas deben estar en una línea horizontal, vertical o diagonal recta.",
            "legal_step4": "Si el puntero se convierte en una cruz, la casilla es válida.",
            "flip_intro": "Después de una jugada válida, todas las fichas del oponente flanqueadas se vuelven de su color.",
            "flip_b1": "Las fichas solo se voltean en líneas creadas por la jugada.", "flip_b2": "Las líneas pueden ser horizontales, verticales o diagonales.",
            "flip_b3": "Una jugada puede voltear fichas en varias direcciones.",
            "end_intro": "La partida termina cuando ningún lado puede hacer otra jugada válida.",
            "end_condition": "Una partida puede terminar antes de que se llenen todas las casillas.", "end_scoring": "Al final se cuentan las fichas.",
            "end_win": "Si sus fichas rojas superan a las fichas azules del equipo, usted gana.",
            "popup_outflank": "Flanquear significa colocar una ficha de modo que una o más fichas del oponente queden entre la ficha nueva y otra ficha de su color.",
            "popup_multi": "Varias direcciones significa que la misma jugada flanquea fichas en más de una línea horizontal, vertical o diagonal.",
            "how_to": "Cómo jugar", "can_move": "Cuando puede mover", "cannot_move": "Cuando no puede mover",
            "keyboard_key": "Tecla", "keyboard_action": "Acción", "related": "Temas relacionados",
            "contents_hint": "Haga clic en Temas de ayuda para volver a la lista de temas.",
        },
    },
    "sv-SE": {
        "terms": {
            "steps": "Steg", "notes": "Obs!", "rules_points": "Regelöversikt",
            "game_menu": "Spel-menyn", "help_topics": "hjälpavsnitt", "goal": "spelets mål",
            "board_game": "brädspel", "white_pieces": "vita brickor", "black_pieces": "svarta brickor",
            "player": "spelare", "opponent": "motståndare", "both_players": "båda spelarna",
            "winner": "vinnare", "tie_game": "oavgjort", "skill": "nivå",
            "beginner": "Nybörjare", "novice": "Novis", "expert": "Expert", "master": "Mästare",
            "thinking_time": "betänketid", "current_game": "aktuellt spel", "restart": "börja om",
            "first_move": "första draget", "player_first": "spelaren börjar", "computer_first": "datorn börjar",
            "move": "drag", "start": "start", "player_turn": "spelarens tur", "computer_move": "datorns drag",
            "mouse": "mus", "cursor": "pekare", "cross_cursor": "korspekare", "no_legal_move": "inget tillåtet drag",
            "keyboard_move": "tangentbordsdrag", "arrow_keys": "piltangenter", "enter_key": "Retur",
            "spacebar": "mellanslag", "practice_game": "övningsspel", "normal_game": "normalt spel",
            "game_end": "spelslut", "end_condition": "slutvillkor", "scoring": "poängräkning",
            "mono_monitor": "monokrom bildskärm", "line": "linje", "row": "rad", "column": "kolumn",
            "diagonal": "diagonal",
        },
        "text": {
            "overview_p1": "Reversi är ett brädspel där du och datorn turas om att lägga brickor. Du använder röda brickor och datorn blå brickor.",
            "overview_p2": "Ett drag måste omringa minst en motståndarbricka. Omringade brickor vänds till din färg, och den sida som har flest brickor när spelet slutar vinner.",
            "play_intro": "Följ de här stegen för att starta och spela.", "play_step1": "Välj datorns nivå.",
            "play_step2": "Starta ett nytt spel.", "play_step3": "Lägg på en tillåten ruta, eller passa när inget drag finns.",
            "difficulty_intro": "Nivån avgör hur noggrant datorn söker efter ett drag. Högre nivåer kan ta längre tid.",
            "difficulty_step1": "Öppna menyn Alternativ.", "difficulty_step2": "Välj Nybörjare, Novis, Expert eller Mästare.",
            "difficulty_step3": "Fortsätt det aktuella spelet eller starta ett nytt.",
            "new_intro": "Ett nytt spel ersätter det aktuella spelet.", "new_step1": "Välj först nivå om det behövs.",
            "new_step2": "Öppna Spel-menyn.", "new_step3": "Välj Nytt.",
            "new_note": "Normalt börjar du. På första draget kan du också välja Passa för att låta datorn börja.",
            "turn_intro": "När det är din tur flyttar du pekaren över tomma rutor. En spelbar ruta gör pekaren till ett kors.",
            "turn_can1": "Flytta pekaren till en tom ruta.", "turn_can2": "Om pekaren blir ett kors klickar du på rutan eller trycker på Retur.",
            "turn_can3": "Vänta på datorns drag.", "turn_cannot1": "Om ingen ruta kan spelas väntar du på passningsmeddelandet.",
            "turn_cannot2": "Öppna Spel-menyn.", "turn_cannot3": "Välj Passa.",
            "keyboard_intro": "Du kan använda tangentbordet för att flytta brädmarkören och lägga en bricka.",
            "keyboard_arrows": "Flyttar brädmarkören.", "keyboard_enter": "Lägger en bricka.",
            "hint_intro": "Använd Tips om du inte vet var du ska spela.",
            "hint_warning": "Tips avslutar det aktuella normala spelet och byter till övningsläge. Starta ett nytt spel för att återgå till normalt spel.",
            "hint_step1": "Öppna Spel-menyn.", "hint_step2": "Välj Tips.", "hint_step3": "Titta på den blinkande föreslagna rutan.",
            "hint_note": "På högre nivåer kan datorn behöva mer tid innan tipset visas.",
            "rules_intro": "Reversi använder samma regler för drag, vändning av brickor, passning och poängräkning.",
            "rules_b1": "Brädet innehåller rutorna där brickor läggs.", "rules_b2": "Ett tillåtet drag måste omringa minst en motståndarbricka.",
            "rules_b3": "Alla omringade brickor vänds.", "rules_b4": "Om en sida saknar tillåtet drag måste den passa.",
            "rules_b5": "När ingen sida kan spela slutar spelet och sidan med flest brickor vinner.",
            "board_intro": "Brädet ger spelrutorna. Varje ruta är tom eller innehåller en bricka.",
            "board_mono": "På en monokrom bildskärm visas röda brickor som vita och blå brickor som svarta.",
            "board_board": "Brädet är ett rutnät med åtta gånger åtta rutor.", "board_pieces": "Brickorna visar vilken sida som kontrollerar en ruta.",
            "board_empty": "En tom ruta kan bara användas om draget är tillåtet.",
            "legal_intro": "Ett tillåtet drag är en tom ruta som omringar minst en motståndarbricka.",
            "legal_step1": "Rutan måste vara tom.", "legal_step2": "Den nya brickan måste omringa minst en motståndarbricka.",
            "legal_step3": "De omringade brickorna måste ligga på samma vågräta, lodräta eller diagonala linje.",
            "legal_step4": "Om pekaren blir ett kors är rutan tillåten.",
            "flip_intro": "Efter ett tillåtet drag vänds alla omringade motståndarbrickor till din färg.",
            "flip_b1": "Brickor vänds bara på linjer som skapas av draget.", "flip_b2": "Linjer kan vara vågräta, lodräta eller diagonala.",
            "flip_b3": "Ett drag kan vända brickor i flera riktningar.",
            "end_intro": "Spelet slutar när ingen sida kan göra ett tillåtet drag.", "end_condition": "Spelet kan sluta innan alla rutor är fyllda.",
            "end_scoring": "När spelet slutar räknas brickorna.", "end_win": "Om dina röda brickor är fler än datorns blå brickor vinner du.",
            "popup_outflank": "Att omringa betyder att lägga en bricka så att en eller flera motståndarbrickor ligger mellan den nya brickan och en annan bricka i din färg.",
            "popup_multi": "Flera riktningar betyder att samma drag omringar brickor på mer än en vågrät, lodrät eller diagonal linje.",
            "how_to": "Så spelar du", "can_move": "När du kan spela", "cannot_move": "När du inte kan spela",
            "keyboard_key": "Tangent", "keyboard_action": "Åtgärd", "related": "Närliggande avsnitt",
            "contents_hint": "Klicka på Hjälpavsnitt för att återgå till listan med avsnitt.",
        },
    },
    "fi-FI": {
        "terms": {
            "steps": "Vaiheet", "notes": "Huomautuksia", "rules_points": "Sääntöjen yhteenveto",
            "game_menu": "Peli-valikko", "help_topics": "ohjeaiheet", "goal": "pelin tavoite",
            "board_game": "lautapeli", "white_pieces": "valkoiset nappulat", "black_pieces": "mustat nappulat",
            "player": "pelaaja", "opponent": "vastustaja", "both_players": "molemmat pelaajat",
            "winner": "voittaja", "tie_game": "tasapeli", "skill": "taso",
            "beginner": "Aloittelija", "novice": "Noviisi", "expert": "Asiantuntija", "master": "Mestari",
            "thinking_time": "miettimisaika", "current_game": "nykyinen peli", "restart": "aloita uudelleen",
            "first_move": "ensimmäinen siirto", "player_first": "pelaaja aloittaa", "computer_first": "tietokone aloittaa",
            "move": "siirto", "start": "aloitus", "player_turn": "pelaajan vuoro", "computer_move": "tietokoneen siirto",
            "mouse": "hiiri", "cursor": "osoitin", "cross_cursor": "ristiosoitin",
            "no_legal_move": "ei sallittua siirtoa", "keyboard_move": "näppäimistösiirto",
            "arrow_keys": "nuolinäppäimet", "enter_key": "Enter-näppäin", "spacebar": "välilyönti",
            "practice_game": "harjoituspeli", "normal_game": "tavallinen peli", "game_end": "pelin loppu",
            "end_condition": "päättymisehto", "scoring": "pisteytys", "mono_monitor": "mustavalkoinen näyttö",
            "line": "linja", "row": "vaakarivi", "column": "pystyrivi", "diagonal": "vinorivi",
        },
        "text": {
            "overview_p1": "Reversi on lautapeli, jossa sinä ja tietokone asetatte nappuloita vuorotellen. Sinulla on punaiset nappulat ja tietokoneella siniset nappulat.",
            "overview_p2": "Siirron on saarrettava vähintään yksi vastustajan nappula. Saarretut nappulat käännetään sinun väriksesi, ja lopussa se puoli voittaa, jolla on enemmän nappuloita.",
            "play_intro": "Aloita ja pelaa peli näiden vaiheiden avulla.", "play_step1": "Valitse tietokoneen taso.",
            "play_step2": "Aloita uusi peli.", "play_step3": "Siirrä sallittuun ruutuun tai passaa, jos siirtoa ei ole.",
            "difficulty_intro": "Taso määrittää, kuinka tarkasti tietokone etsii siirtoa. Korkeammat tasot voivat kestää kauemmin.",
            "difficulty_step1": "Avaa Asetukset-valikko.", "difficulty_step2": "Valitse Aloittelija, Noviisi, Asiantuntija tai Mestari.",
            "difficulty_step3": "Jatka nykyistä peliä tai aloita uusi.",
            "new_intro": "Uusi peli korvaa nykyisen pelin.", "new_step1": "Valitse taso ensin, jos se on tarpeen.",
            "new_step2": "Avaa Peli-valikko.", "new_step3": "Valitse Uusi.",
            "new_note": "Yleensä sinä aloitat. Ensimmäisellä vuorolla voit myös valita Passaa ja antaa tietokoneen aloittaa.",
            "turn_intro": "Kun on sinun vuorosi, siirrä osoitinta tyhjien ruutujen päällä. Pelattava ruutu muuttaa osoittimen ristiksi.",
            "turn_can1": "Siirrä osoitin tyhjään ruutuun.", "turn_can2": "Jos osoitin muuttuu ristiksi, napsauta ruutua tai paina Enter.",
            "turn_can3": "Odota tietokoneen siirtoa.", "turn_cannot1": "Jos mikään ruutu ei ole pelattavissa, odota passausviestiä.",
            "turn_cannot2": "Avaa Peli-valikko.", "turn_cannot3": "Valitse Passaa.",
            "keyboard_intro": "Voit siirtää lautakohdistinta ja asettaa nappulan näppäimistöllä.",
            "keyboard_arrows": "Siirtävät lautakohdistinta.", "keyboard_enter": "Asettaa nappulan.",
            "hint_intro": "Käytä vihjettä, jos et tiedä mihin siirtää.",
            "hint_warning": "Vihje lopettaa nykyisen tavallisen pelin ja vaihtaa harjoitustilaan. Palaa tavalliseen peliin aloittamalla uusi peli.",
            "hint_step1": "Avaa Peli-valikko.", "hint_step2": "Valitse Vihje.", "hint_step3": "Katso vilkkuvaa ehdotettua ruutua.",
            "hint_note": "Korkeammilla tasoilla tietokone voi tarvita enemmän aikaa ennen vihjeen näyttämistä.",
            "rules_intro": "Reversi käyttää samoja sääntöjä siirtoihin, nappuloiden kääntämiseen, passaamiseen ja pisteytykseen.",
            "rules_b1": "Laudalla on ruudut, joihin nappuloita asetetaan.", "rules_b2": "Sallitun siirron on saarrettava vähintään yksi vastustajan nappula.",
            "rules_b3": "Kaikki saarretut nappulat käännetään.", "rules_b4": "Jos puolella ei ole sallittua siirtoa, sen on passattava.",
            "rules_b5": "Kun kumpikaan puoli ei voi siirtää, peli päättyy ja enemmän nappuloita omistava puoli voittaa.",
            "board_intro": "Laudalla on peliruudut. Jokainen ruutu on tyhjä tai siinä on yksi nappula.",
            "board_mono": "Mustavalkoisella näytöllä punaiset nappulat näkyvät valkoisina ja siniset mustina.",
            "board_board": "Lauta on kahdeksan kertaa kahdeksan ruudun neliöruudukko.", "board_pieces": "Nappulat näyttävät, kumpi puoli hallitsee ruutua.",
            "board_empty": "Tyhjää ruutua voi käyttää vain, jos siirto on sallittu.",
            "legal_intro": "Sallittu siirto on tyhjä ruutu, joka saartaa vähintään yhden vastustajan nappulan.",
            "legal_step1": "Ruudun on oltava tyhjä.", "legal_step2": "Uuden nappulan on saarrettava vähintään yksi vastustajan nappula.",
            "legal_step3": "Saarrettujen nappuloiden on oltava samalla suoralla vaaka-, pysty- tai vinorivillä.",
            "legal_step4": "Jos osoitin muuttuu ristiksi, ruutu on sallittu.",
            "flip_intro": "Sallitun siirron jälkeen kaikki saarretut vastustajan nappulat käännetään sinun väriksesi.",
            "flip_b1": "Nappulat kääntyvät vain siirron luomilla linjoilla.", "flip_b2": "Linjat voivat olla vaaka-, pysty- tai vinorivejä.",
            "flip_b3": "Yksi siirto voi kääntää nappuloita useissa suunnissa.",
            "end_intro": "Peli päättyy, kun kumpikaan puoli ei voi tehdä sallittua siirtoa.", "end_condition": "Peli voi päättyä ennen kuin kaikki ruudut ovat täynnä.",
            "end_scoring": "Pelin päättyessä nappulat lasketaan.", "end_win": "Jos punaisia nappuloitasi on enemmän kuin tietokoneen sinisiä nappuloita, voitat.",
            "popup_outflank": "Saartaminen tarkoittaa nappulan asettamista niin, että yksi tai useampi vastustajan nappula jää uuden nappulan ja toisen oman nappulasi väliin.",
            "popup_multi": "Useita suuntia tarkoittaa, että sama siirto saartaa nappuloita useammalla vaaka-, pysty- tai vinolinjalla.",
            "how_to": "Pelaaminen", "can_move": "Kun voit siirtää", "cannot_move": "Kun et voi siirtää",
            "keyboard_key": "Näppäin", "keyboard_action": "Toiminto", "related": "Aiheeseen liittyvät ohjeet",
            "contents_hint": "Palaa aiheluetteloon napsauttamalla Ohjeaiheet.",
        },
    },
    "pt-PT": {
        "terms": {
            "steps": "Passos", "notes": "Notas", "rules_points": "Resumo das regras",
            "game_menu": "menu Jogo", "help_topics": "tópicos de Ajuda", "goal": "objetivo do jogo",
            "board_game": "jogo de tabuleiro", "white_pieces": "peças brancas", "black_pieces": "peças pretas",
            "player": "jogador", "opponent": "adversário", "both_players": "ambos os jogadores",
            "winner": "vencedor", "tie_game": "empate", "skill": "nível",
            "beginner": "Principiante", "novice": "Novato", "expert": "Perito", "master": "Mestre",
            "thinking_time": "tempo de reflexão", "current_game": "jogo atual", "restart": "reiniciar",
            "first_move": "primeira jogada", "player_first": "o jogador começa", "computer_first": "o computador começa",
            "move": "jogada", "start": "início", "player_turn": "turno do jogador", "computer_move": "jogada do computador",
            "mouse": "rato", "cursor": "ponteiro", "cross_cursor": "ponteiro em cruz",
            "no_legal_move": "sem jogada válida", "keyboard_move": "jogada pelo teclado",
            "arrow_keys": "teclas de seta", "enter_key": "tecla Enter", "spacebar": "barra de espaços",
            "practice_game": "jogo de treino", "normal_game": "jogo normal", "game_end": "fim do jogo",
            "end_condition": "condição final", "scoring": "pontuação", "mono_monitor": "monitor monocromático",
            "line": "linha", "row": "linha horizontal", "column": "coluna", "diagonal": "diagonal",
        },
        "text": {
            "overview_p1": "Reversi é um jogo de tabuleiro em que o utilizador e o computador colocam peças à vez. O utilizador usa peças vermelhas e o computador usa peças azuis.",
            "overview_p2": "Uma jogada deve flanquear pelo menos uma peça adversária. As peças flanqueadas viram para a sua cor; no fim, vence o lado com mais peças.",
            "play_intro": "Utilize estes passos para iniciar e jogar.", "play_step1": "Escolha o nível do computador.",
            "play_step2": "Inicie um novo jogo.", "play_step3": "Jogue numa casa válida ou passe quando não existir jogada.",
            "difficulty_intro": "O nível controla a profundidade com que o computador procura uma jogada. Os níveis mais altos podem demorar mais.",
            "difficulty_step1": "Abra o menu Opções.", "difficulty_step2": "Escolha Principiante, Novato, Perito ou Mestre.",
            "difficulty_step3": "Continue o jogo atual ou inicie um novo.",
            "new_intro": "Um novo jogo substitui o jogo atual.", "new_step1": "Se necessário, escolha primeiro o nível.",
            "new_step2": "Abra o menu Jogo.", "new_step3": "Escolha Novo.",
            "new_note": "Normalmente o utilizador joga primeiro. Na primeira jogada, também pode escolher Passar para deixar o computador começar.",
            "turn_intro": "Quando for a sua vez, mova o ponteiro sobre as casas vazias. Uma casa jogável muda o ponteiro para uma cruz.",
            "turn_can1": "Mova o ponteiro para uma casa vazia.", "turn_can2": "Se o ponteiro se tornar uma cruz, clique na casa ou prima Enter.",
            "turn_can3": "Aguarde a jogada do computador.", "turn_cannot1": "Se nenhuma casa puder ser jogada, aguarde a mensagem de passar.",
            "turn_cannot2": "Abra o menu Jogo.", "turn_cannot3": "Escolha Passar.",
            "keyboard_intro": "Pode usar o teclado para mover o cursor do tabuleiro e colocar uma peça.",
            "keyboard_arrows": "Movem o cursor do tabuleiro.", "keyboard_enter": "Coloca uma peça.",
            "hint_intro": "Use Sugestão se não souber onde jogar.",
            "hint_warning": "Usar Sugestão termina o jogo normal atual e muda para o modo de treino. Inicie um novo jogo para voltar ao jogo normal.",
            "hint_step1": "Abra o menu Jogo.", "hint_step2": "Escolha Sugestão.", "hint_step3": "Veja a casa sugerida intermitente.",
            "hint_note": "Em níveis mais altos, o computador pode precisar de mais tempo antes de mostrar a sugestão.",
            "rules_intro": "Reversi usa o mesmo conjunto de regras para jogar, virar peças, passar e contar a pontuação.",
            "rules_b1": "O tabuleiro contém as casas onde se colocam as peças.", "rules_b2": "Uma jogada válida deve flanquear pelo menos uma peça adversária.",
            "rules_b3": "Todas as peças flanqueadas são viradas.", "rules_b4": "Se um lado não tiver jogada válida, deve passar.",
            "rules_b5": "Quando nenhum lado puder jogar, o jogo termina e vence o lado com mais peças.",
            "board_intro": "O tabuleiro fornece as casas de jogo. Cada casa está vazia ou contém uma peça.",
            "board_mono": "Num monitor monocromático, as peças vermelhas aparecem brancas e as peças azuis aparecem pretas.",
            "board_board": "O tabuleiro é uma grelha quadrada de oito por oito.", "board_pieces": "As peças mostram que lado controla uma casa.",
            "board_empty": "Uma casa vazia só pode ser usada se a jogada for válida.",
            "legal_intro": "Uma jogada válida é uma casa vazia que flanqueia pelo menos uma peça adversária.",
            "legal_step1": "A casa deve estar vazia.", "legal_step2": "A nova peça deve flanquear pelo menos uma peça adversária.",
            "legal_step3": "As peças flanqueadas devem estar numa linha horizontal, vertical ou diagonal reta.",
            "legal_step4": "Se o ponteiro se tornar uma cruz, a casa é válida.",
            "flip_intro": "Depois de uma jogada válida, todas as peças adversárias flanqueadas viram para a sua cor.",
            "flip_b1": "As peças só viram nas linhas criadas pela jogada.", "flip_b2": "As linhas podem ser horizontais, verticais ou diagonais.",
            "flip_b3": "Uma jogada pode virar peças em várias direções.",
            "end_intro": "O jogo termina quando nenhum lado pode fazer outra jogada válida.",
            "end_condition": "Um jogo pode terminar antes de todas as casas estarem preenchidas.", "end_scoring": "Quando o jogo termina, contam-se as peças.",
            "end_win": "Se as suas peças vermelhas forem mais do que as peças azuis do computador, vence.",
            "popup_outflank": "Flanquear significa colocar uma peça de modo que uma ou mais peças adversárias fiquem entre a nova peça e outra peça da sua cor.",
            "popup_multi": "Várias direções significa que a mesma jogada flanqueia peças em mais de uma linha horizontal, vertical ou diagonal.",
            "how_to": "Como jogar", "can_move": "Quando pode jogar", "cannot_move": "Quando não pode jogar",
            "keyboard_key": "Tecla", "keyboard_action": "Ação", "related": "Tópicos relacionados",
            "contents_hint": "Clique em Tópicos de Ajuda para regressar à lista de tópicos.",
        },
    },
    "it-IT": {
        "terms": {
            "steps": "Passaggi", "notes": "Note", "rules_points": "Riepilogo delle regole",
            "game_menu": "menu Gioco", "help_topics": "argomenti della Guida", "goal": "scopo del gioco",
            "board_game": "gioco da tavolo", "white_pieces": "pedine bianche", "black_pieces": "pedine nere",
            "player": "giocatore", "opponent": "avversario", "both_players": "entrambi i giocatori",
            "winner": "vincitore", "tie_game": "pareggio", "skill": "livello",
            "beginner": "Principiante", "novice": "Novizio", "expert": "Esperto", "master": "Maestro",
            "thinking_time": "tempo di riflessione", "current_game": "partita corrente", "restart": "ricominciare",
            "first_move": "prima mossa", "player_first": "il giocatore inizia", "computer_first": "il computer inizia",
            "move": "mossa", "start": "inizio", "player_turn": "turno del giocatore", "computer_move": "mossa del computer",
            "mouse": "mouse", "cursor": "puntatore", "cross_cursor": "puntatore a croce",
            "no_legal_move": "nessuna mossa valida", "keyboard_move": "mossa da tastiera",
            "arrow_keys": "tasti di direzione", "enter_key": "tasto Invio", "spacebar": "barra spaziatrice",
            "practice_game": "partita di pratica", "normal_game": "partita normale", "game_end": "fine partita",
            "end_condition": "condizione di fine", "scoring": "punteggio", "mono_monitor": "monitor monocromatico",
            "line": "linea", "row": "riga", "column": "colonna", "diagonal": "diagonale",
        },
        "text": {
            "overview_p1": "Reversi è un gioco da tavolo in cui tu e il computer collocate pedine a turno. Tu usi le pedine rosse; il computer usa le pedine blu.",
            "overview_p2": "Una mossa deve circondare almeno una pedina avversaria. Le pedine circondate vengono girate nel tuo colore; alla fine vince chi ha più pedine.",
            "play_intro": "Usa questi passaggi per iniziare e giocare.", "play_step1": "Scegli il livello del computer.",
            "play_step2": "Inizia una nuova partita.", "play_step3": "Gioca in una casella valida o passa quando non sono disponibili mosse.",
            "difficulty_intro": "Il livello controlla quanto a fondo il computer cerca una mossa. I livelli più alti possono richiedere più tempo.",
            "difficulty_step1": "Apri il menu Opzioni.", "difficulty_step2": "Scegli Principiante, Novizio, Esperto o Maestro.",
            "difficulty_step3": "Continua la partita corrente o iniziane una nuova.",
            "new_intro": "Una nuova partita sostituisce quella corrente.", "new_step1": "Se necessario, scegli prima il livello.",
            "new_step2": "Apri il menu Gioco.", "new_step3": "Scegli Nuova.",
            "new_note": "Di norma muovi per primo. Alla prima mossa puoi anche scegliere Passare per lasciare iniziare il computer.",
            "turn_intro": "Quando è il tuo turno, sposta il puntatore sulle caselle vuote. Una casella giocabile cambia il puntatore in una croce.",
            "turn_can1": "Sposta il puntatore su una casella vuota.", "turn_can2": "Se il puntatore diventa una croce, fai clic sulla casella o premi Invio.",
            "turn_can3": "Attendi la mossa del computer.", "turn_cannot1": "Se non è giocabile alcuna casella, attendi il messaggio di passaggio.",
            "turn_cannot2": "Apri il menu Gioco.", "turn_cannot3": "Scegli Passare.",
            "keyboard_intro": "Puoi usare la tastiera per spostare il cursore della scacchiera e collocare una pedina.",
            "keyboard_arrows": "Spostano il cursore della scacchiera.", "keyboard_enter": "Colloca una pedina.",
            "hint_intro": "Usa Suggerimento se non sai dove muovere.",
            "hint_warning": "Usare Suggerimento termina la partita normale corrente e passa alla modalità pratica. Inizia una nuova partita per tornare al gioco normale.",
            "hint_step1": "Apri il menu Gioco.", "hint_step2": "Scegli Suggerimento.", "hint_step3": "Osserva la casella suggerita lampeggiante.",
            "hint_note": "Ai livelli più alti, il computer può richiedere più tempo prima di mostrare il suggerimento.",
            "rules_intro": "Reversi usa le stesse regole per muovere, girare le pedine, passare e contare il punteggio.",
            "rules_b1": "La scacchiera contiene le caselle in cui collocare le pedine.", "rules_b2": "Una mossa valida deve circondare almeno una pedina avversaria.",
            "rules_b3": "Tutte le pedine circondate vengono girate.", "rules_b4": "Se una parte non ha mosse valide, deve passare.",
            "rules_b5": "Quando nessuna parte può muovere, la partita termina e vince chi ha più pedine.",
            "board_intro": "La scacchiera fornisce le caselle di gioco. Ogni casella è vuota o contiene una pedina.",
            "board_mono": "Su un monitor monocromatico, le pedine rosse appaiono bianche e le pedine blu appaiono nere.",
            "board_board": "La scacchiera è una griglia quadrata di otto per otto.", "board_pieces": "Le pedine indicano quale parte controlla una casella.",
            "board_empty": "Una casella vuota può essere usata solo se la mossa è valida.",
            "legal_intro": "Una mossa valida è una casella vuota che circonda almeno una pedina avversaria.",
            "legal_step1": "La casella deve essere vuota.", "legal_step2": "La nuova pedina deve circondare almeno una pedina avversaria.",
            "legal_step3": "Le pedine circondate devono trovarsi su una stessa linea orizzontale, verticale o diagonale.",
            "legal_step4": "Se il puntatore diventa una croce, la casella è valida.",
            "flip_intro": "Dopo una mossa valida, tutte le pedine avversarie circondate vengono girate nel tuo colore.",
            "flip_b1": "Le pedine si girano solo sulle linee create dalla mossa.", "flip_b2": "Le linee possono essere orizzontali, verticali o diagonali.",
            "flip_b3": "Una mossa può girare pedine in più direzioni.",
            "end_intro": "La partita termina quando nessuna parte può effettuare un'altra mossa valida.",
            "end_condition": "Una partita può terminare prima che tutte le caselle siano piene.", "end_scoring": "Quando la partita termina, le pedine vengono contate.",
            "end_win": "Se le tue pedine rosse superano le pedine blu del computer, vinci.",
            "popup_outflank": "Circondare significa collocare una pedina in modo che una o più pedine avversarie si trovino tra la nuova pedina e un'altra pedina del tuo colore.",
            "popup_multi": "Più direzioni significa che la stessa mossa circonda pedine su più di una linea orizzontale, verticale o diagonale.",
            "how_to": "Come giocare", "can_move": "Quando puoi muovere", "cannot_move": "Quando non puoi muovere",
            "keyboard_key": "Tasto", "keyboard_action": "Azione", "related": "Argomenti correlati",
            "contents_hint": "Fai clic su Argomenti della Guida per tornare all'elenco degli argomenti.",
        },
    },
}

for _locale_name, _override in WESTERN_OVERRIDES.items():
    LOCALES[_locale_name]["terms"].update(_override["terms"])
    LOCALES[_locale_name]["text"].update(_override["text"])


for _name, _base in {
    "ru-RU": (
        "cp1251", "0x419", "Arial", 204,
        {
            "game": "Реверси", "help_title": "Справка Реверси", "overview": "Обзор Реверси",
            "play": "Игра в Реверси", "difficulty": "Выбор уровня", "new_game": "Начало новой игры",
            "turn": "Ходы игры", "keyboard": "Использование клавиатуры", "hint": "Получение подсказки",
            "rules": "Правила", "board": "Доска и фишки", "legal": "Допустимые ходы", "flip": "Переворот фишек",
            "end": "Конец игры и счет", "steps": "Действия", "notes": "Примечания", "rules_points": "Кратко о правилах",
            "game_menu": "Меню Игра", "options": "Параметры", "help_topics": "разделы справки",
            "goal": "цель игры", "board_game": "настольная игра", "red_pieces": "красные фишки",
            "blue_pieces": "синие фишки", "white_pieces": "белые фишки", "black_pieces": "черные фишки",
            "pieces": "фишки", "piece_count": "число фишек", "player": "игрок", "computer": "компьютер",
            "opponent": "соперник", "both_players": "оба игрока", "winner": "победитель", "tie_game": "ничья",
            "skill": "уровень", "beginner": "Начинающий", "novice": "Новичок", "expert": "Эксперт",
            "master": "Мастер", "thinking_time": "время обдумывания", "current_game": "текущая игра",
            "restart": "перезапуск", "first_move": "первый ход", "player_first": "игрок ходит первым",
            "computer_first": "компьютер ходит первым", "move": "ход", "start": "начало",
            "player_turn": "ход игрока", "computer_move": "ход компьютера", "mouse": "мышь", "cursor": "указатель",
            "cross_cursor": "указатель-крест", "legal_move": "допустимый ход", "legal_square": "допустимая клетка",
            "no_legal_move": "нет допустимого хода", "pass": "пас", "keyboard_move": "ход с клавиатуры",
            "arrow_keys": "клавиши со стрелками", "enter_key": "клавиша Enter", "spacebar": "Пробел",
            "suggested_move": "предлагаемый ход", "practice_game": "тренировочная игра", "normal_game": "обычная игра",
            "game_end": "конец игры", "end_condition": "условие окончания", "scoring": "подсчет очков",
            "square": "клетка", "empty_square": "пустая клетка", "mono_monitor": "монохромный монитор",
            "outflank": "зажать", "line": "линия", "row": "горизонталь", "column": "вертикаль",
            "diagonal": "диагональ", "direction": "направление", "multi_direction": "несколько направлений",
        },
        {
            "overview_p1": "Реверси - игра на доске, где вы и компьютер по очереди ставите фишки. Вы играете красными фишками, компьютер - синими.",
            "overview_p2": "Ход должен зажать хотя бы одну фишку соперника. Зажатые фишки переворачиваются в ваш цвет; в конце выигрывает сторона с большим числом фишек.",
            "play_intro": "Чтобы начать и играть партию, выполните эти действия.", "play_step1": "Выберите уровень компьютера.",
            "play_step2": "Начните новую игру.", "play_step3": "Ходите на допустимые клетки или пасуйте, если хода нет.",
            "difficulty_intro": "Уровень определяет, насколько тщательно компьютер ищет ход. Более высокий уровень может требовать больше времени.",
            "difficulty_step1": "Откройте меню Параметры.", "difficulty_step2": "Выберите Начинающий, Новичок, Эксперт или Мастер.",
            "difficulty_step3": "Продолжайте текущую игру или начните новую.",
            "new_intro": "Новая игра заменяет текущую.", "new_step1": "При необходимости сначала выберите уровень.",
            "new_step2": "Откройте меню Игра.", "new_step3": "Выберите Новая.",
            "new_note": "Обычно вы ходите первым. На первом ходу можно выбрать Пас, чтобы первым ходил компьютер.",
            "turn_intro": "Когда ваш ход, перемещайте указатель по пустым клеткам. На допустимой клетке указатель становится крестом.",
            "turn_can1": "Переместите указатель на пустую клетку.", "turn_can2": "Если указатель стал крестом, щелкните клетку или нажмите Enter.",
            "turn_can3": "Подождите, пока компьютер сделает ход.", "turn_cannot1": "Если ходов нет, дождитесь сообщения о пасе.",
            "turn_cannot2": "Откройте меню Игра.", "turn_cannot3": "Выберите Пас.",
            "keyboard_intro": "Клавиатурой можно перемещать курсор доски и ставить фишку.", "keyboard_arrows": "Перемещают курсор доски.",
            "keyboard_enter": "Ставит фишку.", "hint_intro": "Используйте подсказку, если не знаете, куда ходить.",
            "hint_warning": "Подсказка завершает текущую обычную игру и включает тренировочный режим. Чтобы вернуться к обычной игре, начните новую.",
            "hint_step1": "Откройте меню Игра.", "hint_step2": "Выберите Подсказка.", "hint_step3": "Посмотрите на мигающую предлагаемую клетку.",
            "hint_note": "На высоких уровнях компьютеру может понадобиться больше времени перед показом подсказки.",
            "rules_intro": "В Реверси одни правила определяют ход, переворот фишек, пас и счет.",
            "rules_b1": "На доске находятся клетки для фишек.", "rules_b2": "Допустимый ход должен зажать хотя бы одну фишку соперника.",
            "rules_b3": "Все зажатые фишки переворачиваются.", "rules_b4": "Если допустимого хода нет, сторона должна пасовать.",
            "rules_b5": "Когда ходить не может ни одна сторона, игра заканчивается и выигрывает сторона с большим числом фишек.",
            "board_intro": "Доска содержит клетки для ходов. Каждая клетка пуста или содержит одну фишку.",
            "board_mono": "На монохромном мониторе красные фишки отображаются белыми, а синие - черными.",
            "board_board": "Доска представляет собой сетку восемь на восемь.", "board_pieces": "Фишки показывают, какая сторона занимает клетку.",
            "board_empty": "Пустую клетку можно использовать только для допустимого хода.",
            "legal_intro": "Допустимый ход - это пустая клетка, которая зажимает хотя бы одну фишку соперника.",
            "legal_step1": "Клетка должна быть пустой.", "legal_step2": "Новая фишка должна зажать хотя бы одну фишку соперника.",
            "legal_step3": "Зажатые фишки должны лежать на одной горизонтали, вертикали или диагонали.",
            "legal_step4": "Если указатель становится крестом, ход допустим.",
            "flip_intro": "После допустимого хода все зажатые фишки соперника переворачиваются в ваш цвет.",
            "flip_b1": "Фишки переворачиваются только на линиях, созданных ходом.", "flip_b2": "Линии могут быть горизонтальными, вертикальными или диагональными.",
            "flip_b3": "Один ход может перевернуть фишки в нескольких направлениях.",
            "end_intro": "Игра заканчивается, когда ни одна сторона не может сделать допустимый ход.", "end_condition": "Игра может закончиться до заполнения всех клеток.",
            "end_scoring": "После окончания подсчитываются фишки на доске.", "end_win": "Если красных фишек больше, чем синих, вы выигрываете.",
            "popup_outflank": "Зажать означает поставить фишку так, чтобы одна или несколько фишек соперника оказались между новой фишкой и другой вашей фишкой.",
            "popup_multi": "Несколько направлений означает, что один ход зажимает фишки на нескольких горизонтальных, вертикальных или диагональных линиях.",
            "how_to": "Как играть", "can_move": "Когда ход возможен", "cannot_move": "Когда хода нет",
            "keyboard_key": "Клавиша", "keyboard_action": "Действие", "related": "См. также",
            "contents_hint": "Щелкните разделы справки, чтобы вернуться к списку тем.",
        },
    ),
    "uk-UA": (
        "cp1251", "0x422", "Arial", 204,
        {
            "game": "Реверсі", "help_title": "Довідка Реверсі", "overview": "Огляд Реверсі",
            "play": "Гра в Реверсі", "difficulty": "Вибір рівня", "new_game": "Початок нової гри",
            "turn": "Ходи гри", "keyboard": "Використання клавіатури", "hint": "Отримання підказки",
            "rules": "Правила", "board": "Дошка і фішки", "legal": "Припустимі ходи", "flip": "Перевертання фішок",
            "end": "Кінець гри й рахунок", "steps": "Дії", "notes": "Примітки", "rules_points": "Коротко про правила",
            "game_menu": "Меню Гра", "options": "Параметри", "help_topics": "розділи довідки",
            "goal": "мета гри", "board_game": "настільна гра", "red_pieces": "червоні фішки",
            "blue_pieces": "сині фішки", "white_pieces": "білі фішки", "black_pieces": "чорні фішки",
            "pieces": "фішки", "piece_count": "кількість фішок", "player": "гравець", "computer": "комп'ютер",
            "opponent": "суперник", "both_players": "обидві сторони", "winner": "переможець", "tie_game": "нічия",
            "skill": "рівень", "beginner": "Початківець", "novice": "Новак", "expert": "Експерт",
            "master": "Майстер", "thinking_time": "час обдумування", "current_game": "поточна гра",
            "restart": "перезапуск", "first_move": "перший хід", "player_first": "гравець ходить першим",
            "computer_first": "комп'ютер ходить першим", "move": "хід", "start": "початок",
            "player_turn": "хід гравця", "computer_move": "хід комп'ютера", "mouse": "миша", "cursor": "вказівник",
            "cross_cursor": "вказівник-хрестик", "legal_move": "припустимий хід", "legal_square": "припустима клітинка",
            "no_legal_move": "немає припустимого ходу", "pass": "пас", "keyboard_move": "хід із клавіатури",
            "arrow_keys": "клавіші зі стрілками", "enter_key": "клавіша Enter", "spacebar": "Пробіл",
            "suggested_move": "запропонований хід", "practice_game": "тренувальна гра", "normal_game": "звичайна гра",
            "game_end": "кінець гри", "end_condition": "умова завершення", "scoring": "підрахунок очок",
            "square": "клітинка", "empty_square": "порожня клітинка", "mono_monitor": "монохромний монітор",
            "outflank": "затиснути", "line": "лінія", "row": "горизонталь", "column": "вертикаль",
            "diagonal": "діагональ", "direction": "напрямок", "multi_direction": "кілька напрямків",
        },
        {
            "overview_p1": "Реверсі - гра на дошці, де ви й комп'ютер по черзі ставите фішки. Ви граєте червоними фішками, комп'ютер - синіми.",
            "overview_p2": "Хід має затиснути хоча б одну фішку суперника. Затиснуті фішки перевертаються у ваш колір; наприкінці перемагає сторона з більшою кількістю фішок.",
            "play_intro": "Щоб почати й грати партію, виконайте ці дії.", "play_step1": "Виберіть рівень комп'ютера.",
            "play_step2": "Почніть нову гру.", "play_step3": "Ходіть у припустимі клітинки або пасуйте, якщо ходу немає.",
            "difficulty_intro": "Рівень визначає, наскільки ретельно комп'ютер шукає хід. Вищий рівень може потребувати більше часу.",
            "difficulty_step1": "Відкрийте меню Параметри.", "difficulty_step2": "Виберіть Початківець, Новак, Експерт або Майстер.",
            "difficulty_step3": "Продовжте поточну гру або почніть нову.",
            "new_intro": "Нова гра замінює поточну.", "new_step1": "За потреби спочатку виберіть рівень.",
            "new_step2": "Відкрийте меню Гра.", "new_step3": "Виберіть Нова.",
            "new_note": "Зазвичай ви ходите першим. На першому ході можна вибрати Пас, щоб першим ходив комп'ютер.",
            "turn_intro": "Коли ваш хід, переміщуйте вказівник порожніми клітинками. На припустимій клітинці вказівник стає хрестиком.",
            "turn_can1": "Перемістіть вказівник на порожню клітинку.", "turn_can2": "Якщо вказівник став хрестиком, клацніть клітинку або натисніть Enter.",
            "turn_can3": "Зачекайте, доки комп'ютер зробить хід.", "turn_cannot1": "Якщо ходів немає, дочекайтеся повідомлення про пас.",
            "turn_cannot2": "Відкрийте меню Гра.", "turn_cannot3": "Виберіть Пас.",
            "keyboard_intro": "Клавіатурою можна переміщувати курсор дошки та ставити фішку.", "keyboard_arrows": "Переміщують курсор дошки.",
            "keyboard_enter": "Ставить фішку.", "hint_intro": "Скористайтеся підказкою, якщо не знаєте, куди ходити.",
            "hint_warning": "Підказка завершує поточну звичайну гру й перемикає в тренувальний режим. Щоб повернутися до звичайної гри, почніть нову.",
            "hint_step1": "Відкрийте меню Гра.", "hint_step2": "Виберіть Підказка.", "hint_step3": "Подивіться на блимаючу запропоновану клітинку.",
            "hint_note": "На вищих рівнях комп'ютеру може знадобитися більше часу перед показом підказки.",
            "rules_intro": "У Реверсі одні правила визначають хід, перевертання фішок, пас і рахунок.",
            "rules_b1": "На дошці є клітинки для фішок.", "rules_b2": "Припустимий хід має затиснути хоча б одну фішку суперника.",
            "rules_b3": "Усі затиснуті фішки перевертаються.", "rules_b4": "Якщо припустимого ходу немає, сторона повинна пасувати.",
            "rules_b5": "Коли жодна сторона не може ходити, гра закінчується, і перемагає сторона з більшою кількістю фішок.",
            "board_intro": "Дошка містить клітинки для ходів. Кожна клітинка порожня або містить одну фішку.",
            "board_mono": "На монохромному моніторі червоні фішки показуються білими, а сині - чорними.",
            "board_board": "Дошка є сіткою вісім на вісім.", "board_pieces": "Фішки показують, яка сторона займає клітинку.",
            "board_empty": "Порожню клітинку можна використати лише для припустимого ходу.",
            "legal_intro": "Припустимий хід - це порожня клітинка, що затискає хоча б одну фішку суперника.",
            "legal_step1": "Клітинка має бути порожньою.", "legal_step2": "Нова фішка має затиснути хоча б одну фішку суперника.",
            "legal_step3": "Затиснуті фішки мають лежати на одній горизонталі, вертикалі або діагоналі.",
            "legal_step4": "Якщо вказівник стає хрестиком, хід припустимий.",
            "flip_intro": "Після припустимого ходу всі затиснуті фішки суперника перевертаються у ваш колір.",
            "flip_b1": "Фішки перевертаються лише на лініях, створених ходом.", "flip_b2": "Лінії можуть бути горизонтальними, вертикальними або діагональними.",
            "flip_b3": "Один хід може перевернути фішки в кількох напрямках.",
            "end_intro": "Гра закінчується, коли жодна сторона не може зробити припустимий хід.", "end_condition": "Гра може завершитися до заповнення всіх клітинок.",
            "end_scoring": "Після завершення підраховуються фішки на дошці.", "end_win": "Якщо червоних фішок більше, ніж синіх, ви виграєте.",
            "popup_outflank": "Затиснути означає поставити фішку так, щоб одна або кілька фішок суперника опинилися між новою фішкою та іншою вашою фішкою.",
            "popup_multi": "Кілька напрямків означає, що один хід затискає фішки на кількох горизонтальних, вертикальних або діагональних лініях.",
            "how_to": "Як грати", "can_move": "Коли хід можливий", "cannot_move": "Коли ходу немає",
            "keyboard_key": "Клавіша", "keyboard_action": "Дія", "related": "Див. також",
            "contents_hint": "Клацніть розділи довідки, щоб повернутися до списку тем.",
        },
    ),
}.items():
    add_locale(_name, _base)


for _name, _base in {
    "ar-SA": (
        "cp1256", "0x401", "Arial", 178, True,
        {
            "game": "ريفرسي", "help_title": "تعليمات ريفيرسي", "overview": "نظرة عامة على ريفيرسي",
            "play": "لعب ريفيرسي", "difficulty": "اختيار مستوى الصعوبة", "new_game": "بدء لعبة جديدة",
            "turn": "أدوار اللعبة", "keyboard": "استخدام لوحة المفاتيح", "hint": "الحصول على تلميح",
            "rules": "القواعد", "board": "اللوحة والأقراص", "legal": "النقلات القانونية", "flip": "قلب الأقراص",
            "end": "نهاية اللعبة والنتيجة", "steps": "الخطوات", "notes": "ملاحظات", "rules_points": "ملخص القواعد",
            "game_menu": "قائمة اللعبة", "options": "خيارات", "help_topics": "مواضيع التعليمات",
            "goal": "هدف اللعبة", "board_game": "لعبة لوحة", "red_pieces": "الأقراص الحمراء",
            "blue_pieces": "الأقراص الزرقاء", "white_pieces": "الأقراص البيضاء", "black_pieces": "الأقراص السوداء",
            "pieces": "الأقراص", "piece_count": "عدد الأقراص", "player": "اللاعب", "computer": "الكمبيوتر",
            "opponent": "الخصم", "both_players": "الطرفان", "winner": "الفائز", "tie_game": "تعادل",
            "skill": "المستوى", "beginner": "مبتدئ", "novice": "مستجد", "expert": "خبير", "master": "محترف",
            "thinking_time": "وقت التفكير", "current_game": "اللعبة الحالية", "restart": "إعادة البدء",
            "first_move": "النقلة الأولى", "player_first": "اللاعب يبدأ", "computer_first": "الكمبيوتر يبدأ",
            "move": "نقلة", "start": "بدء", "player_turn": "دور اللاعب", "computer_move": "نقلة الكمبيوتر",
            "mouse": "الماوس", "cursor": "المؤشر", "cross_cursor": "مؤشر صليب",
            "legal_move": "نقلة قانونية", "legal_square": "مربع قانوني", "no_legal_move": "لا توجد نقلة قانونية",
            "pass": "تمرير", "keyboard_move": "نقلة بلوحة المفاتيح", "arrow_keys": "مفاتيح الأسهم",
            "enter_key": "مفتاح Enter", "spacebar": "مفتاح المسافة", "suggested_move": "نقلة مقترحة",
            "practice_game": "لعبة تدريب", "normal_game": "لعبة عادية", "game_end": "نهاية اللعبة",
            "end_condition": "شرط النهاية", "scoring": "احتساب النتيجة", "square": "مربع",
            "empty_square": "مربع فارغ", "mono_monitor": "شاشة أحادية اللون", "outflank": "محاصرة",
            "line": "خط", "row": "صف", "column": "عمود", "diagonal": "قطر", "direction": "اتجاه",
            "multi_direction": "اتجاهات متعددة",
        },
        {
            "overview_p1": "ريفرسي لعبة لوحة يتناوب فيها اللاعب والكمبيوتر على وضع الأقراص. أنت تستخدم الأقراص الحمراء، والكمبيوتر يستخدم الأقراص الزرقاء.",
            "overview_p2": "يجب أن تحاصر النقلة قرصا واحدا للخصم على الأقل. تنقلب الأقراص المحاصرة إلى لونك، ويفوز الطرف الذي لديه أقراص أكثر في النهاية.",
            "play_intro": "استخدم هذه الخطوات لبدء اللعبة ولعبها.", "play_step1": "اختر مستوى الكمبيوتر.",
            "play_step2": "ابدأ لعبة جديدة.", "play_step3": "العب في مربع قانوني، أو مرر إذا لم توجد نقلة.",
            "difficulty_intro": "يحدد المستوى مقدار بحث الكمبيوتر عن النقلة. قد تحتاج المستويات الأعلى إلى وقت أطول.",
            "difficulty_step1": "افتح قائمة خيارات.", "difficulty_step2": "اختر مبتدئ أو مستجد أو خبير أو محترف.",
            "difficulty_step3": "تابع اللعبة الحالية أو ابدأ لعبة جديدة.",
            "new_intro": "بدء لعبة جديدة يستبدل اللعبة الحالية.", "new_step1": "اختر المستوى أولا إذا لزم الأمر.",
            "new_step2": "افتح قائمة اللعبة.", "new_step3": "اختر جديد.",
            "new_note": "عادة تبدأ أنت. في الدور الأول يمكنك أيضا اختيار تمرير ليبدأ الكمبيوتر.",
            "turn_intro": "عندما يحين دورك، حرك المؤشر فوق المربعات الفارغة. المربع القابل للعب يغير المؤشر إلى صليب.",
            "turn_can1": "حرك المؤشر إلى مربع فارغ.", "turn_can2": "إذا أصبح المؤشر صليبا، انقر المربع أو اضغط Enter.",
            "turn_can3": "انتظر نقلة الكمبيوتر.", "turn_cannot1": "إذا لم يكن هناك مربع قابل للعب، فانتظر رسالة التمرير.",
            "turn_cannot2": "افتح قائمة اللعبة.", "turn_cannot3": "اختر تمرير.",
            "keyboard_intro": "يمكنك استخدام لوحة المفاتيح لتحريك مؤشر اللوحة ووضع قرص.", "keyboard_arrows": "تحرك مؤشر اللوحة.",
            "keyboard_enter": "تضع قرصا.", "hint_intro": "استخدم تلميحا إذا لم تكن متأكدا أين تلعب.",
            "hint_warning": "استخدام التلميح ينهي اللعبة العادية الحالية وينتقل إلى وضع التدريب. ابدأ لعبة جديدة للعودة إلى اللعب العادي.",
            "hint_step1": "افتح قائمة اللعبة.", "hint_step2": "اختر تلميح.", "hint_step3": "راقب المربع المقترح الوامض.",
            "hint_note": "في المستويات الأعلى قد يحتاج الكمبيوتر إلى وقت أطول قبل عرض التلميح.",
            "rules_intro": "تستخدم ريفيرسي مجموعة قواعد واحدة للنقلات وقلب الأقراص والتمرير واحتساب النتيجة.",
            "rules_b1": "تحتوي اللوحة على المربعات التي توضع فيها الأقراص.", "rules_b2": "يجب أن تحاصر النقلة القانونية قرصا واحدا للخصم على الأقل.",
            "rules_b3": "يجب قلب كل الأقراص المحاصرة.", "rules_b4": "إذا لم يكن لدى طرف نقلة قانونية، فيجب عليه التمرير.",
            "rules_b5": "عندما لا يستطيع أي طرف اللعب، تنتهي اللعبة ويفوز الطرف ذو الأقراص الأكثر.",
            "board_intro": "توفر اللوحة مربعات اللعب. كل مربع إما فارغ أو يحتوي على قرص واحد.",
            "board_mono": "على شاشة أحادية اللون تظهر الأقراص الحمراء بيضاء، وتظهر الأقراص الزرقاء سوداء.",
            "board_board": "اللوحة شبكة مربعة من ثمانية في ثمانية.", "board_pieces": "تبين الأقراص أي طرف يسيطر على المربع.",
            "board_empty": "لا يستخدم المربع الفارغ إلا إذا كانت النقلة قانونية.",
            "legal_intro": "النقلة القانونية هي مربع فارغ يحاصر قرصا واحدا للخصم على الأقل.",
            "legal_step1": "يجب أن يكون المربع فارغا.", "legal_step2": "يجب أن يحاصر القرص الجديد قرصا واحدا للخصم على الأقل.",
            "legal_step3": "يجب أن تكون الأقراص المحاصرة في خط أفقي أو عمودي أو قطري مستقيم.",
            "legal_step4": "إذا أصبح المؤشر صليبا، فهذا المربع قانوني.",
            "flip_intro": "بعد النقلة القانونية، تنقلب كل أقراص الخصم المحاصرة إلى لونك.",
            "flip_b1": "تنقلب الأقراص فقط على الخطوط التي صنعتها النقلة.", "flip_b2": "يمكن أن تكون الخطوط أفقية أو عمودية أو قطرية.",
            "flip_b3": "يمكن لنقلة واحدة أن تقلب أقراصا في اتجاهات متعددة.",
            "end_intro": "تنتهي اللعبة عندما لا يستطيع أي طرف إجراء نقلة قانونية أخرى.", "end_condition": "قد تنتهي اللعبة قبل امتلاء كل المربعات.",
            "end_scoring": "عند انتهاء اللعبة، يتم عد الأقراص.", "end_win": "إذا زادت أقراصك الحمراء على أقراص الكمبيوتر الزرقاء، فستفوز.",
            "popup_outflank": "المحاصرة تعني وضع قرص بحيث تقع أقراص الخصم بين القرص الجديد وقرص آخر من لونك.",
            "popup_multi": "الاتجاهات المتعددة تعني أن النقلة نفسها تحاصر أقراصا على أكثر من خط أفقي أو عمودي أو قطري.",
            "how_to": "طريقة اللعب", "can_move": "عندما يمكنك اللعب", "cannot_move": "عندما لا يمكنك اللعب",
            "keyboard_key": "المفتاح", "keyboard_action": "الإجراء", "related": "مواضيع ذات صلة",
            "contents_hint": "انقر مواضيع التعليمات للعودة إلى قائمة المواضيع.",
        },
    ),
    "he-IL": (
        "cp1255", "0x40d", "Arial", 177, True,
        {
            "game": "רברסי", "help_title": "עזרה של רברסי", "overview": "מבט כללי על רברסי",
            "play": "משחק ברברסי", "difficulty": "בחירת רמה", "new_game": "התחלת משחק חדש",
            "turn": "תורות משחק", "keyboard": "שימוש במקלדת", "hint": "קבלת רמז",
            "rules": "כללים", "board": "לוח וכלים", "legal": "מהלכים חוקיים", "flip": "הפיכת כלים",
            "end": "סיום המשחק וניקוד", "steps": "שלבים", "notes": "הערות", "rules_points": "סיכום הכללים",
            "game_menu": "תפריט משחק", "options": "אפשרויות", "help_topics": "נושאי עזרה",
            "goal": "מטרת המשחק", "board_game": "משחק לוח", "red_pieces": "כלים אדומים",
            "blue_pieces": "כלים כחולים", "white_pieces": "כלים לבנים", "black_pieces": "כלים שחורים",
            "pieces": "כלים", "piece_count": "מספר כלים", "player": "שחקן", "computer": "מחשב",
            "opponent": "יריב", "both_players": "שני הצדדים", "winner": "מנצח", "tie_game": "תיקו",
            "skill": "רמה", "beginner": "מתחיל", "novice": "טירון", "expert": "מומחה", "master": "אמן",
            "thinking_time": "זמן חשיבה", "current_game": "המשחק הנוכחי", "restart": "הפעלה מחדש",
            "first_move": "מהלך ראשון", "player_first": "השחקן מתחיל", "computer_first": "המחשב מתחיל",
            "move": "מהלך", "start": "התחלה", "player_turn": "תור השחקן", "computer_move": "מהלך המחשב",
            "mouse": "עכבר", "cursor": "מצביע", "cross_cursor": "מצביע צלב",
            "legal_move": "מהלך חוקי", "legal_square": "משבצת חוקית", "no_legal_move": "אין מהלך חוקי",
            "pass": "דלג", "keyboard_move": "מהלך במקלדת", "arrow_keys": "מקשי חצים",
            "enter_key": "מקש Enter", "spacebar": "מקש רווח", "suggested_move": "מהלך מוצע",
            "practice_game": "משחק אימון", "normal_game": "משחק רגיל", "game_end": "סיום משחק",
            "end_condition": "תנאי סיום", "scoring": "ניקוד", "square": "משבצת", "empty_square": "משבצת ריקה",
            "mono_monitor": "צג מונוכרום", "outflank": "ללכוד", "line": "קו", "row": "שורה",
            "column": "עמודה", "diagonal": "אלכסון", "direction": "כיוון", "multi_direction": "כמה כיוונים",
        },
        {
            "overview_p1": "רברסי הוא משחק לוח שבו אתה והמחשב מניחים כלים לפי תור. אתה משתמש בכלים אדומים, והמחשב משתמש בכלים כחולים.",
            "overview_p2": "מהלך חייב ללכוד לפחות כלי אחד של היריב. הכלים הלכודים נהפכים לצבע שלך, ובסיום מנצח הצד עם מספר הכלים הגדול יותר.",
            "play_intro": "השתמש בשלבים אלה כדי להתחיל ולשחק.", "play_step1": "בחר את רמת המחשב.",
            "play_step2": "התחל משחק חדש.", "play_step3": "שחק במשבצת חוקית, או דלג כאשר אין מהלך.",
            "difficulty_intro": "הרמה קובעת עד כמה המחשב מחפש מהלך בזהירות. רמות גבוהות עשויות לקחת זמן רב יותר.",
            "difficulty_step1": "פתח את תפריט אפשרויות.", "difficulty_step2": "בחר מתחיל, טירון, מומחה או אמן.",
            "difficulty_step3": "המשך את המשחק הנוכחי או התחל משחק חדש.",
            "new_intro": "משחק חדש מחליף את המשחק הנוכחי.", "new_step1": "אם צריך, בחר קודם רמה.",
            "new_step2": "פתח את תפריט משחק.", "new_step3": "בחר חדש.",
            "new_note": "בדרך כלל אתה מתחיל. במהלך הראשון אפשר לבחור דלג כדי לתת למחשב להתחיל.",
            "turn_intro": "כאשר תורך, הזז את המצביע מעל משבצות ריקות. משבצת שניתן לשחק בה משנה את המצביע לצלב.",
            "turn_can1": "הזז את המצביע למשבצת ריקה.", "turn_can2": "אם המצביע הופך לצלב, לחץ על המשבצת או הקש Enter.",
            "turn_can3": "המתן למהלך של המחשב.", "turn_cannot1": "אם אין משבצת שניתן לשחק בה, המתן להודעת דילוג.",
            "turn_cannot2": "פתח את תפריט משחק.", "turn_cannot3": "בחר דלג.",
            "keyboard_intro": "אפשר להשתמש במקלדת כדי להזיז את סמן הלוח ולהניח כלי.", "keyboard_arrows": "מזיזים את סמן הלוח.",
            "keyboard_enter": "מניח כלי.", "hint_intro": "השתמש ברמז אם אינך בטוח היכן לשחק.",
            "hint_warning": "שימוש ברמז מסיים את המשחק הרגיל הנוכחי ומעביר למצב אימון. התחל משחק חדש כדי לחזור למשחק רגיל.",
            "hint_step1": "פתח את תפריט משחק.", "hint_step2": "בחר רמז.", "hint_step3": "צפה במשבצת המוצעת המהבהבת.",
            "hint_note": "ברמות גבוהות יותר ייתכן שהמחשב יזדקק לזמן נוסף לפני הצגת הרמז.",
            "rules_intro": "רברסי משתמש באותם כללים למהלכים, להפיכת כלים, לדילוג ולניקוד.",
            "rules_b1": "הלוח מכיל את המשבצות שבהן מניחים כלים.", "rules_b2": "מהלך חוקי חייב ללכוד לפחות כלי אחד של היריב.",
            "rules_b3": "כל הכלים הלכודים נהפכים.", "rules_b4": "אם לצד אין מהלך חוקי, עליו לדלג.",
            "rules_b5": "כאשר אף צד אינו יכול לשחק, המשחק מסתיים והצד עם יותר כלים מנצח.",
            "board_intro": "הלוח מספק את משבצות המשחק. כל משבצת ריקה או מכילה כלי אחד.",
            "board_mono": "בצג מונוכרום כלים אדומים מוצגים כלבנים, וכלים כחולים מוצגים כשחורים.",
            "board_board": "הלוח הוא רשת מרובעת של שמונה על שמונה.", "board_pieces": "כלים מציינים איזה צד שולט במשבצת.",
            "board_empty": "אפשר להשתמש במשבצת ריקה רק אם המהלך חוקי.",
            "legal_intro": "מהלך חוקי הוא משבצת ריקה שלוכדת לפחות כלי אחד של היריב.",
            "legal_step1": "המשבצת חייבת להיות ריקה.", "legal_step2": "הכלי החדש חייב ללכוד לפחות כלי אחד של היריב.",
            "legal_step3": "הכלים הלכודים חייבים להיות בקו אופקי, אנכי או אלכסוני ישר.",
            "legal_step4": "אם המצביע הופך לצלב, המשבצת חוקית.",
            "flip_intro": "אחרי מהלך חוקי, כל כלי היריב הלכודים נהפכים לצבע שלך.",
            "flip_b1": "כלים נהפכים רק בקווים שנוצרו על ידי המהלך.", "flip_b2": "הקווים יכולים להיות אופקיים, אנכיים או אלכסוניים.",
            "flip_b3": "מהלך אחד יכול להפוך כלים בכמה כיוונים.",
            "end_intro": "המשחק מסתיים כאשר אף צד אינו יכול לבצע עוד מהלך חוקי.", "end_condition": "המשחק עשוי להסתיים לפני שכל המשבצות מלאות.",
            "end_scoring": "בסיום המשחק סופרים את הכלים.", "end_win": "אם הכלים האדומים שלך רבים מהכלים הכחולים של המחשב, ניצחת.",
            "popup_outflank": "ללכוד פירושו להניח כלי כך שכלי יריב אחד או יותר יהיו בין הכלי החדש לבין כלי אחר בצבע שלך.",
            "popup_multi": "כמה כיוונים פירושו שאותו מהלך לוכד כלים ביותר מקו אופקי, אנכי או אלכסוני אחד.",
            "how_to": "כיצד לשחק", "can_move": "כאשר אפשר לשחק", "cannot_move": "כאשר אי אפשר לשחק",
            "keyboard_key": "מקש", "keyboard_action": "פעולה", "related": "נושאים קשורים",
            "contents_hint": "לחץ על נושאי עזרה כדי לחזור לרשימת הנושאים.",
        },
    ),
}.items():
    encoding, lcid, font, charset, rtl, terms, text = _base
    LOCALES[_name] = locale(encoding, lcid, font, charset, terms, text, rtl=rtl)


def rtf_text(text, encoding):
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
            out.extend("\\'%02x" % b for b in ch.encode(encoding))
    return "".join(out)


def footnote(mark, text, enc):
    return (
        r"{\up " + mark + r"}{\footnote\pard\plain{\up " + mark + "} " +
        rtf_text(text, enc) + "}\n"
    )


def para_prefix(data, before=75, left=115, first=0):
    align = r"\qr\rtlpar" if data["rtl"] else ""
    fi = f"\\fi{first}" if first else ""
    return f"\\pard{align}\\sb{before}\\sl-235\\li{left}\\ri125{fi} "


def rich(parts, data):
    enc = data["encoding"]
    if isinstance(parts, str):
        parts = [parts]
    out = []
    for part in parts:
        if isinstance(part, str):
            out.append(rtf_text(part, enc))
            continue
        kind, text, target = part
        if kind == "jump":
            out.append(r"\uldb " + rtf_text(text, enc) + r"\plain\f1\fs18 {\v " + target + r"}\f1\fs18 ")
        elif kind == "popup":
            out.append(r"\ul " + rtf_text(text, enc) + r"\plain\f1\fs18 {\v " + target + r"}\f1\fs18 ")
        else:
            raise ValueError(f"Unknown rich segment: {kind}")
    return "".join(out)


def para(parts, data, before=75, left=115, first=0):
    return para_prefix(data, before, left, first) + r"\plain\f1\fs18 " + rich(parts, data) + "\n\\par\n"


def title_line(text, data):
    enc = data["encoding"]
    align = r"\qr\rtlpar" if data["rtl"] else ""
    return (
        rf"\pard{align}\sb115\sa35\sl-235\li115\ri125 \f1\b\fs18 "
        + rtf_text(text, enc)
        + r"\plain\f1\fs18"
        + "\n\\par\n"
    )


def subheading(text, data, before=115):
    enc = data["encoding"]
    align = r"\qr\rtlpar" if data["rtl"] else ""
    return (
        rf"\pard{align}\sb{before}\sa35\sl-235\li115\ri125 \f1\b\fs18 "
        + rtf_text(text, enc)
        + r"\plain\f1\fs18"
        + "\n\\par\n"
    )


def numbered(num, parts, data, before=75):
    return (
        para_prefix(data, before, 295, -185)
        + r"\tx295 \plain\fs18 " + str(num) + r"\tab \f1 "
        + rich(parts, data)
        + "\n\\par\n"
    )


def bullet(parts, data, before=45, left=295, first=-185):
    return (
        para_prefix(data, before, left, first)
        + rf"\tx{left} \plain\f1\fs18 " + r"\{bmc bm0.bmp\}\plain\fs18 \tab \f1 "
        + rich(parts, data)
        + "\n\\par\n"
    )


def table_row(left, right, data, before=55):
    return (
        para_prefix(data, before, 2155, -2035)
        + r"\tx2155 \plain\f1\fs18 "
        + rtf_text(left, data["encoding"])
        + r"\tab "
        + rtf_text(right, data["encoding"])
        + "\n\\par\n"
    )


def keyboard_table(data):
    t = data["text"]
    return (
        para_prefix(data, 115, 2135, -2015)
        + r"\brdrb\brdrs\tx2635 \f1\b\fs18 "
        + rtf_text(t["keyboard_key"], data["encoding"])
        + r"\tab "
        + rtf_text(t["keyboard_action"], data["encoding"])
        + r"\plain\f1\fs18"
        + "\n\\par\n"
        + table_row(data["terms"]["arrow_keys"], t["keyboard_arrows"], data)
        + table_row(data["terms"]["enter_key"] + " / " + data["terms"]["spacebar"], t["keyboard_enter"], data, before=35)
    )


def related(topic_keys, data):
    keywords = ";".join(ALINKS[TOPICS[key]] for key in topic_keys)
    macro = f'AL("{keywords}")'
    label = data["text"]["related"]
    return (
        subheading(label, data)
        + para_prefix(data, 35, 115)
        + r"\tx295 \f1\fs18 "
        + r"\{button ," + macro + r"\} \uldb "
        + rtf_text(label, data["encoding"])
        + r"\plain\f1\fs18 {\v %!" + macro + "}\n\\par\n"
    )


def keywords_for(topic_key, data):
    terms = data["terms"]
    keys = KEYWORD_KEYS[topic_key]
    words = []
    for key in keys:
        value = terms[key]
        if value not in words:
            words.append(value)
    for left, right in COMPOUNDS.get(topic_key, []):
        value = f"{terms[left]}, {terms[right]}"
        if value not in words:
            words.append(value)
    return words


def topic(topic_key, body, data):
    topic_id = TOPICS[topic_key]
    title = data["terms"][topic_key]
    enc = data["encoding"]
    return (
        footnote("$", title, enc)
        + footnote("#", topic_id, enc)
        + footnote(">", "proc4", enc)
        + footnote("K", ";".join(keywords_for(topic_key, data)), enc)
        + footnote("A", ALINKS[topic_id], enc)
        + title_line(title, data)
        + body
        + "\n\\page\n"
    )


def popup_topic(topic_key, text, data):
    return (
        footnote("#", TOPICS[topic_key], data["encoding"])
        + para(text, data, before=45)
        + "\\page\n"
    )


def contents_fallback_topic(data):
    return (
        footnote("!", "NS();FD()", data["encoding"])
        + footnote("#", CONTENTS_FALLBACK_TOPIC, data["encoding"])
        + para(data["text"]["contents_hint"], data, before=45, left=295, first=-185)
        + "\\page\n"
    )


def build_topics(data):
    t = data["text"]
    terms = data["terms"]
    topics = []

    topics.append(topic("overview", para(t["overview_p1"], data) + para(t["overview_p2"], data) +
                        related(["new_game", "turn", "rules"], data), data))

    topics.append(topic("play", para(t["play_intro"], data) + subheading(t["how_to"], data) +
                        numbered(1, [("jump", terms["difficulty"], TOPICS["difficulty"]), ": ", t["play_step1"]], data) +
                        numbered(2, [("jump", terms["new_game"], TOPICS["new_game"]), ": ", t["play_step2"]], data, before=45) +
                        numbered(3, [("jump", terms["turn"], TOPICS["turn"]), ": ", t["play_step3"]], data, before=45) +
                        related(["rules", "hint", "keyboard"], data), data))

    topics.append(topic("difficulty", para(t["difficulty_intro"], data) + subheading(terms["steps"], data) +
                        numbered(1, t["difficulty_step1"], data) + numbered(2, t["difficulty_step2"], data, before=45) +
                        numbered(3, t["difficulty_step3"], data, before=45) +
                        related(["new_game", "play", "hint"], data), data))

    topics.append(topic("new_game", para(t["new_intro"], data) + subheading(terms["steps"], data) +
                        numbered(1, t["new_step1"], data) + numbered(2, t["new_step2"], data, before=45) +
                        numbered(3, t["new_step3"], data, before=45) + para(t["new_note"], data) +
                        related(["difficulty", "turn", "rules"], data), data))

    topics.append(topic("turn", para(t["turn_intro"], data) +
                        subheading(t["can_move"], data) + numbered(1, t["turn_can1"], data) +
                        numbered(2, t["turn_can2"], data, before=45) + numbered(3, t["turn_can3"], data, before=45) +
                        subheading(t["cannot_move"], data) + numbered(1, t["turn_cannot1"], data) +
                        numbered(2, t["turn_cannot2"], data, before=45) + numbered(3, t["turn_cannot3"], data, before=45) +
                        related(["legal", "flip", "keyboard"], data), data))

    topics.append(topic("keyboard", para(t["keyboard_intro"], data) + keyboard_table(data) +
                        related(["turn", "legal"], data), data))

    topics.append(topic("hint", para(t["hint_intro"], data) + subheading(terms["notes"], data) +
                        para(t["hint_warning"], data, before=35) + subheading(terms["steps"], data) +
                        numbered(1, t["hint_step1"], data) + numbered(2, t["hint_step2"], data, before=45) +
                        numbered(3, t["hint_step3"], data, before=45) + subheading(terms["notes"], data) +
                        para(t["hint_note"], data, before=35) +
                        related(["play", "rules", "difficulty", "new_game"], data), data))

    topics.append(topic("rules", para(t["rules_intro"], data) + subheading(terms["rules_points"], data) +
                        bullet([("jump", terms["board"], TOPICS["board"]), ": ", t["rules_b1"]], data, before=75) +
                        bullet([("jump", terms["legal"], TOPICS["legal"]), ": ", t["rules_b2"]], data) +
                        bullet([("jump", terms["flip"], TOPICS["flip"]), ": ", t["rules_b3"]], data) +
                        bullet(t["rules_b4"], data) + bullet([("jump", terms["end"], TOPICS["end"]), ": ", t["rules_b5"]], data) +
                        related(["play", "turn", "hint"], data), data))

    topics.append(topic("board", para(t["board_intro"], data) + para(t["board_mono"], data) +
                        subheading(terms["board"], data) + para(t["board_board"], data, before=35) +
                        subheading(terms["pieces"], data) + para(t["board_pieces"], data, before=35) +
                        subheading(terms["empty_square"], data) + para(t["board_empty"], data, before=35) +
                        related(["legal", "turn"], data), data))

    topics.append(topic("legal", para(t["legal_intro"], data) + subheading(terms["steps"], data) +
                        numbered(1, t["legal_step1"], data) +
                        numbered(2, [t["legal_step2"], " (", ("popup", terms["outflank"], TOPICS["outflank"]), ")"], data, before=45) +
                        numbered(3, t["legal_step3"], data, before=45) +
                        numbered(4, t["legal_step4"], data, before=45) +
                        related(["flip", "turn"], data), data))

    topics.append(topic("flip", para([t["flip_intro"], " (", ("popup", terms["multi_direction"], TOPICS["multi_direction"]), ")"], data) +
                        subheading(terms["rules_points"], data) +
                        bullet(t["flip_b1"], data, before=75) + bullet(t["flip_b2"], data) + bullet(t["flip_b3"], data) +
                        related(["legal", "end"], data), data))

    topics.append(topic("end", para(t["end_intro"], data) + subheading(terms["end_condition"], data) +
                        para(t["end_condition"], data, before=35) + subheading(terms["scoring"], data) +
                        para(t["end_scoring"], data, before=35) + subheading(terms["winner"], data) +
                        para(t["end_win"], data, before=35) +
                        related(["legal", "turn"], data), data))

    topics.append(popup_topic("outflank", t["popup_outflank"], data))
    topics.append(popup_topic("multi_direction", t["popup_multi"], data))
    topics.append(contents_fallback_topic(data))
    return topics


def build_rtf(data):
    enc = data["encoding"]
    font = data["font"]
    charset = data["charset"]
    return (
        r"{\rtf1\ansi\ansicpg" + enc.replace("cp", "") + r"\deff0" + "\n"
        + r"{\fonttbl{\f0\fnil\fcharset" + str(charset) + " " + rtf_text(font, enc)
        + r";}{\f1\fnil\fcharset" + str(charset) + " " + rtf_text(font, enc) + r";}}" + "\n"
        + r"{\stylesheet{\f0\fs18 \snext0 Normal;}" + "\n}" + r"\pard\plain\f0\fs18" + "\n"
        + "".join(build_topics(data))
        + "}"
    )


def build_hpj(data):
    enc = data["encoding"]
    font = data["font"]
    charset = data["charset"]
    title = data["terms"]["help_title"]
    return f"""[OPTIONS]
TITLE={title}
CONTENTS={CONTENTS_FALLBACK_TOPIC}
CNT=REVERSI.cnt
LCID={data["lcid"]} 0x0 0x0
DEFFONT={font},9,{charset}
COMPRESS=0

[WINDOWS]
proc4="",(653,102,360,600),20736,(255,255,226),(192,192,192),f3
trouble="",(653,102,360,600),,(255,255,226),(192,192,192),f3
big="",(18,10,600,435),20736,(255,255,226),(192,192,192),f2
moreinfo="",(313,137,210,287),,(255,255,226),(192,192,192),f2
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
""".encode(enc)


def build_cnt(data):
    terms = data["terms"]
    lines = [
        ":Base REVERSI.HLP>proc4",
        f":Title {terms['help_title']}",
        f"1 {terms['game']}",
        f"2 {terms['overview']}={TOPICS['overview']}",
        f"2 {terms['play']}",
        f"3 {terms['play']}={TOPICS['play']}",
        f"3 {terms['difficulty']}={TOPICS['difficulty']}",
        f"3 {terms['new_game']}={TOPICS['new_game']}",
        f"3 {terms['turn']}={TOPICS['turn']}",
        f"3 {terms['keyboard']}={TOPICS['keyboard']}",
        f"2 {terms['rules']}",
        f"3 {terms['rules']}={TOPICS['rules']}",
        f"3 {terms['board']}={TOPICS['board']}",
        f"3 {terms['legal']}={TOPICS['legal']}",
        f"3 {terms['flip']}={TOPICS['flip']}",
        f"3 {terms['end']}={TOPICS['end']}",
        f"2 {terms['hint']}={TOPICS['hint']}",
        "",
    ]
    return "\n".join(lines).encode(data["encoding"])


def generate_locale(locale_name):
    data = LOCALES[locale_name]
    out = HELP_ROOT / locale_name
    out.mkdir(parents=True, exist_ok=True)
    (out / "REVERSI.rtf").write_bytes(build_rtf(data).encode("ascii"))
    (out / "REVERSI.hpj").write_bytes(build_hpj(data))
    (out / "REVERSI.cnt").write_bytes(build_cnt(data))
    if OFFICIAL_BULLET.exists():
        copyfile(OFFICIAL_BULLET, out / "bm0.bmp")


def write_terms_report(path):
    lines = [
        "# WinHelp Terms And Index Candidates",
        "",
        "Generated from `help/make_hlp_sources.py`. These terms are the localized",
        "topic names, gameplay terms, and K-footnote index candidates used by the",
        "WinHelp source generator.",
        "",
        "Reference policy:",
        "",
        "- UI terms should stay aligned with `src/lang/*.rcinc` and `docs/TERMINOLOGY.md`.",
        "- Windows UI/help wording should follow Microsoft Language Resources and Microsoft Terminology where available.",
        "- Gameplay terms should follow the World Othello Federation rules vocabulary: board, square, disc/piece, move, outflank, flip, pass, and game end.",
        "- WinHelp remains ANSI. Each locale uses its own legacy Windows code page and font/charset pair.",
        "",
    ]
    for locale_name in sorted(LOCALES):
        data = LOCALES[locale_name]
        lines += [
            f"## {locale_name}",
            "",
            f"- Encoding: `{data['encoding']}`",
            f"- LCID: `{data['lcid']}`",
            f"- Font/charset: `{data['font']}`, `{data['charset']}`",
            "",
            "| Topic | Index terms |",
            "| --- | --- |",
        ]
        for topic_key in TOPIC_ORDER:
            lines.append(
                f"| {data['terms'][topic_key]} | " +
                "; ".join(keywords_for(topic_key, data)).replace("|", "\\|") +
                " |"
            )
        lines.append("")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines), encoding="utf-8")


def selected_locales(values):
    if not values:
        return ["zh-CN"]
    result = []
    for value in values:
        for part in value.split(","):
            name = part.strip()
            if not name:
                continue
            if name.lower() == "all":
                return list(LOCALES)
            if name not in LOCALES:
                known = ", ".join(sorted(LOCALES))
                raise SystemExit(f"Unknown locale '{name}'. Known locales: {known}")
            result.append(name)
    return result


def main(argv=None):
    parser = argparse.ArgumentParser(description="Generate localized Reversi WinHelp sources.")
    parser.add_argument("-Locale", "--locale", action="append", help="Locale name, comma list, or all.")
    parser.add_argument("--terms-report", help="Write a Markdown report of WinHelp index terms.")
    args = parser.parse_args(argv)

    for locale_name in selected_locales(args.locale):
        generate_locale(locale_name)
    if args.terms_report:
        write_terms_report(Path(args.terms_report))


if __name__ == "__main__":
    main()
