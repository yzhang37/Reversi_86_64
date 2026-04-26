/* Included by reversi_win32.c. Hidden background context menu for manual state probes. */

static const UINT kTestMenuCommands[] = {
    IDM_TEST_SCRATCH_BOARD,
    IDM_TEST_PLAYER_PASS,
    IDM_TEST_COMPUTER_PASS,
    IDM_TEST_CANNOT_PASS,
    IDM_TEST_MUST_PASS,
    IDM_TEST_COMPUTER_MUST_PASS,
    IDM_TEST_NO_MOVES,
    IDM_TEST_WIN,
    IDM_TEST_LOSE
};

static const APP_CHAR kTestMenuText[] =
    APP_TEXT("Scratch Board\n")
    APP_TEXT("-\n")
    APP_TEXT("Player Pass\n")
    APP_TEXT("Computer Pass\n")
    APP_TEXT("Player Can't Pass\n")
    APP_TEXT("Player Must Pass\n")
    APP_TEXT("Computer Must Pass\n")
    APP_TEXT("-\n")
    APP_TEXT("Draw\n")
    APP_TEXT("Player Won\n")
    APP_TEXT("Player Lose");

static void WipeAppChars(APP_CHAR *text, int cch)
{
    volatile APP_CHAR *p = text;
    while (cch-- > 0) {
        *p++ = 0;
    }
}

static UINT TestMenuFloorSqrt(UINT value)
{
    UINT root = 0;
    while ((root + 1) * (root + 1) <= value) {
        ++root;
    }
    return root;
}

static UINT TestMenuSideRadicand(void)
{
    UINT folded = (0x5f3759dfu >> 20) / 5;
    return folded - 15;
}

static UINT TestMenuTailRadicand(void)
{
    return IDI_REVERSI * (IDI_REVERSI << 2);
}

static UINT TestMenuBackgroundKeyId(void)
{
    UINT side = TestMenuFloorSqrt(TestMenuSideRadicand());
    return side * side * side + TestMenuFloorSqrt(TestMenuTailRadicand());
}

static UINT TestMenuPasswordLength(void)
{
    return TestMenuFloorSqrt(121);
}

static int TestMenuCaesarRounds(int index)
{
    return 2 + ((index + (int)TestMenuFloorSqrt(TestMenuTailRadicand())) % 3);
}

static int TestMenuCaesarShift(int index, int round)
{
    return (int)(
        (TestMenuTailRadicand() +
         (UINT)(index + 1) * (UINT)(round + 3) +
         (UINT)(round * round)) % 26);
}

static APP_CHAR DecodeTestMenuPasswordChar(BYTE encoded, int index)
{
    int ch = encoded;
    int rounds = TestMenuCaesarRounds(index);

    while (rounds-- > 0) {
        ch -= TestMenuCaesarShift(index, rounds);
    }
    return (APP_CHAR)(BYTE)ch;
}

static void BuildTestMenuPassword(APP_CHAR *password, int cch)
{
    static const BYTE encoded[] = {
        0x8f, 0xa3, 0xa1, 0x7d, 0x9f, 0x40, 0x8a, 0x94, 0xb6, 0xa1, 0x9e
    };
    int count = (int)TestMenuPasswordLength();

    if (count > (int)(sizeof(encoded) / sizeof(encoded[0]))) {
        count = (int)(sizeof(encoded) / sizeof(encoded[0]));
    }
    if (cch <= count) {
        if (cch > 0) {
            password[0] = 0;
        }
        return;
    }

    for (int i = 0; i < count; ++i) {
        password[i] = DecodeTestMenuPasswordChar(encoded[i], i);
    }
    password[count] = 0;
}

static int IsTestCommand(UINT cmd)
{
    return cmd >= IDM_TEST_SCRATCH_BOARD && cmd <= IDM_TEST_LOSE;
}

static void DestroyTestContextMenu(void)
{
    if (g_testContextMenu) {
        DestroyMenu(g_testContextMenu);
        g_testContextMenu = NULL;
    }
}

static int IsTestContextMenuUnlocked(void)
{
    APP_CHAR key[TEST_MENU_LABEL_LIMIT];
    APP_CHAR password[TEST_MENU_LABEL_LIMIT];
    int unlocked;

    LoadText(TestMenuBackgroundKeyId(), key, TEST_MENU_LABEL_LIMIT);
    BuildTestMenuPassword(password, TEST_MENU_LABEL_LIMIT);
    unlocked = AppStringEquals(key, password);
    WipeAppChars(password, TEST_MENU_LABEL_LIMIT);
    WipeAppChars(key, TEST_MENU_LABEL_LIMIT);
    return unlocked;
}

static int InitTestContextMenu(void)
{
    const APP_CHAR *p = kTestMenuText;
    UINT command_index = 0;

    DestroyTestContextMenu();
    if (!IsTestContextMenuUnlocked()) {
        return 0;
    }

    g_testContextMenu = CreatePopupMenu();
    if (!g_testContextMenu) {
        return 0;
    }

    while (*p) {
        APP_CHAR label[TEST_MENU_LABEL_LIMIT];
        int len = 0;

        while (*p && *p != '\n' && len < TEST_MENU_LABEL_LIMIT - 1) {
            label[len++] = *p++;
        }
        label[len] = 0;
        while (*p == '\n' || *p == '\r') {
            ++p;
        }

        if (!label[0]) {
            continue;
        }
        if (label[0] == '-' && !label[1]) {
            AppendMenu(g_testContextMenu, MF_SEPARATOR, 0, NULL);
            continue;
        }
        if (command_index >= sizeof(kTestMenuCommands) / sizeof(kTestMenuCommands[0])) {
            DestroyTestContextMenu();
            return 0;
        }
        if (!AppendMenu(g_testContextMenu, MF_STRING, kTestMenuCommands[command_index], label)) {
            DestroyTestContextMenu();
            return 0;
        }
        ++command_index;
    }

    if (command_index != sizeof(kTestMenuCommands) / sizeof(kTestMenuCommands[0])) {
        DestroyTestContextMenu();
        return 0;
    }
    return 1;
}

static void PrepareTestGame(HWND hwnd)
{
    int skill_cmd = g_game.skill_cmd ? g_game.skill_cmd : IDM_INTERMEDIATE;
    int depth = g_game.search_depth ? g_game.search_depth : 2;
    int animation_cmd = g_game.animation_cmd ? g_game.animation_cmd : IDM_ANIM_FAST;

    KillTimer(hwnd, TIMER_FLASH);
    g_inputLock = 0;
    g_hintBlankCursor = 0;
    AppZeroMemory(&g_anim, sizeof(g_anim));
    AppZeroMemory(&g_placeFlash, sizeof(g_placeFlash));

    InitGameState();
    g_game.skill_cmd = skill_cmd;
    g_game.search_depth = depth;
    g_game.animation_cmd = animation_cmd;
    g_game.opening_pass_available = 0;
    g_game.practice_title = 0;
    g_game.thinking = 0;
    g_game.flash_ticks = 0;
    g_game.message[0] = 0;
    SetTitle(hwnd);
}

static void SetLinearBoard(int red_count)
{
    int index = 0;
    for (int row = 0; row < BOARD_N; ++row) {
        for (int col = 0; col < BOARD_N; ++col) {
            g_game.cells[row][col] = index < red_count ? RED : BLUE;
            ++index;
        }
    }
}

static void SetPassProbeBoard(int first, int second)
{
    AppZeroMemory(g_game.cells, sizeof(g_game.cells));
    g_game.cells[0][0] = first;
    g_game.cells[0][1] = second;
    g_game.selected_row = 0;
    g_game.selected_col = 2;
}

static void FinishTestGame(HWND hwnd)
{
    UpdateMenuChecks(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

static void TryPass(HWND hwnd);

static void ApplyTestScenario(HWND hwnd, UINT cmd)
{
    APP_CHAR message[192];

    PrepareTestGame(hwnd);

    switch (cmd) {
    case IDM_TEST_PLAYER_PASS:
        g_game.turn = BLUE;
        LoadText(IDS_PASS_TEXT, message, 64);
        SetMessage(hwnd, message, 1);
        break;
    case IDM_TEST_COMPUTER_PASS:
        g_game.turn = RED;
        LoadText(IDS_COMPUTER_PASS_TEXT, message, 128);
        SetMessage(hwnd, message, 1);
        break;
    case IDM_TEST_CANNOT_PASS:
        g_game.turn = RED;
        FinishTestGame(hwnd);
        TryPass(hwnd);
        return;
    case IDM_TEST_MUST_PASS:
        SetPassProbeBoard(BLUE, RED);
        g_game.turn = RED;
        g_game.must_pass = 1;
        LoadText(IDS_MUST_PASS, message, 128);
        SetMessage(hwnd, message, 1);
        break;
    case IDM_TEST_COMPUTER_MUST_PASS:
        SetPassProbeBoard(RED, BLUE);
        g_game.turn = RED;
        LoadText(IDS_COMPUTER_PASS, message, 128);
        SetMessage(hwnd, message, 1);
        break;
    case IDM_TEST_NO_MOVES:
        SetLinearBoard(32);
        g_game.turn = RED;
        g_game.game_over = 1;
        SetGameOverMessage(hwnd);
        break;
    case IDM_TEST_WIN:
        SetLinearBoard(42);
        g_game.turn = RED;
        g_game.game_over = 1;
        SetGameOverMessage(hwnd);
        break;
    case IDM_TEST_LOSE:
        SetLinearBoard(22);
        g_game.turn = RED;
        g_game.game_over = 1;
        SetGameOverMessage(hwnd);
        break;
    default:
        return;
    }

    FinishTestGame(hwnd);
}
