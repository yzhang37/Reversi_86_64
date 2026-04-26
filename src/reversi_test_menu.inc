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

static const APP_CHAR kTestMenuPassword[] = APP_TEXT("shift xyzzy");

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

    LoadText(IDS_BACKGROUND_KEY, key, TEST_MENU_LABEL_LIMIT);
    return AppStringEquals(key, kTestMenuPassword);
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
