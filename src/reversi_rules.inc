/* Included by reversi_win32.c. Board rules, scoring, and move search. */

static int Opponent(int player)
{
    return player == RED ? BLUE : RED;
}

static int OnBoard(int row, int col)
{
    return row >= 0 && row < BOARD_N && col >= 0 && col < BOARD_N;
}

static int CountPieces(int player)
{
    int total = 0;
    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (g_game.cells[r][c] == player) {
                ++total;
            }
        }
    }
    return total;
}

static int CollectFlips(int row, int col, int player, Flip *flips, int max_flips)
{
    if (!OnBoard(row, col) || g_game.cells[row][col] != EMPTY) {
        return 0;
    }

    int total = 0;
    int enemy = Opponent(player);

    for (int i = 0; i < 8; ++i) {
        int dr = k_dirs[i][0];
        int dc = k_dirs[i][1];
        int r = row + dr;
        int c = col + dc;
        int seen = 0;

        while (OnBoard(r, c) && g_game.cells[r][c] == enemy) {
            ++seen;
            r += dr;
            c += dc;
        }

        if (seen > 0 && OnBoard(r, c) && g_game.cells[r][c] == player) {
            int fr = row + dr;
            int fc = col + dc;
            while (fr != r || fc != c) {
                if (flips && total < max_flips) {
                    flips[total].row = fr;
                    flips[total].col = fc;
                }
                ++total;
                fr += dr;
                fc += dc;
            }
        }
    }

    return total;
}

static int HasLegalMove(int player)
{
    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (CollectFlips(r, c, player, NULL, 0) > 0) {
                return 1;
            }
        }
    }
    return 0;
}

static void CopyBoard(int dst[BOARD_N][BOARD_N], int src[BOARD_N][BOARD_N])
{
    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            dst[r][c] = src[r][c];
        }
    }
}

static int CountFlipsOnBoard(int board[BOARD_N][BOARD_N], int row, int col, int player)
{
    if (!OnBoard(row, col) || board[row][col] != EMPTY) {
        return 0;
    }

    int total = 0;
    int enemy = Opponent(player);
    for (int i = 0; i < 8; ++i) {
        int dr = k_dirs[i][0];
        int dc = k_dirs[i][1];
        int r = row + dr;
        int c = col + dc;
        int seen = 0;
        while (OnBoard(r, c) && board[r][c] == enemy) {
            ++seen;
            r += dr;
            c += dc;
        }
        if (seen > 0 && OnBoard(r, c) && board[r][c] == player) {
            total += seen;
        }
    }
    return total;
}

static int ApplyMoveOnBoard(int board[BOARD_N][BOARD_N], int row, int col, int player)
{
    int flips = CountFlipsOnBoard(board, row, col, player);
    if (!flips) {
        return 0;
    }

    int enemy = Opponent(player);
    board[row][col] = player;
    for (int i = 0; i < 8; ++i) {
        int dr = k_dirs[i][0];
        int dc = k_dirs[i][1];
        int r = row + dr;
        int c = col + dc;
        int seen = 0;
        while (OnBoard(r, c) && board[r][c] == enemy) {
            ++seen;
            r += dr;
            c += dc;
        }
        if (seen > 0 && OnBoard(r, c) && board[r][c] == player) {
            int fr = row + dr;
            int fc = col + dc;
            while (fr != r || fc != c) {
                board[fr][fc] = player;
                fr += dr;
                fc += dc;
            }
        }
    }
    return flips;
}

static int HasLegalMoveOnBoard(int board[BOARD_N][BOARD_N], int player)
{
    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (CountFlipsOnBoard(board, r, c, player) > 0) {
                return 1;
            }
        }
    }
    return 0;
}

static int EvaluateBoard(int board[BOARD_N][BOARD_N])
{
    int score = 0;
    int red_moves = 0;
    int blue_moves = 0;

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (board[r][c] == BLUE) {
                score += k_weights[r][c] + 4;
            } else if (board[r][c] == RED) {
                score -= k_weights[r][c] + 4;
            }
            if (CountFlipsOnBoard(board, r, c, BLUE) > 0) {
                ++blue_moves;
            }
            if (CountFlipsOnBoard(board, r, c, RED) > 0) {
                ++red_moves;
            }
        }
    }

    score += (blue_moves - red_moves) * 7;
    return score;
}

static int SearchBoard(int board[BOARD_N][BOARD_N], int depth, int player, int alpha, int beta)
{
    int has_move = HasLegalMoveOnBoard(board, player);
    int enemy = Opponent(player);

    if (depth <= 0 || (!has_move && !HasLegalMoveOnBoard(board, enemy))) {
        return EvaluateBoard(board);
    }

    if (!has_move) {
        return SearchBoard(board, depth - 1, enemy, alpha, beta);
    }

    if (player == BLUE) {
        int best = -1000000;
        for (int r = 0; r < BOARD_N; ++r) {
            for (int c = 0; c < BOARD_N; ++c) {
                if (CountFlipsOnBoard(board, r, c, player) > 0) {
                    int next[BOARD_N][BOARD_N];
                    CopyBoard(next, board);
                    ApplyMoveOnBoard(next, r, c, player);
                    int value = SearchBoard(next, depth - 1, enemy, alpha, beta);
                    best = MaxInt(best, value);
                    alpha = MaxInt(alpha, best);
                    if (beta <= alpha) {
                        return best;
                    }
                }
            }
        }
        return best;
    }

    int best = 1000000;
    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (CountFlipsOnBoard(board, r, c, player) > 0) {
                int next[BOARD_N][BOARD_N];
                CopyBoard(next, board);
                ApplyMoveOnBoard(next, r, c, player);
                int value = SearchBoard(next, depth - 1, enemy, alpha, beta);
                best = MinInt(best, value);
                beta = MinInt(beta, best);
                if (beta <= alpha) {
                    return best;
                }
            }
        }
    }
    return best;
}

static Move FindBestMoveLegacy(int player)
{
    Move best;
    best.row = -1;
    best.col = -1;
    best.score = player == BLUE ? -1000000 : 1000000;

    int board[BOARD_N][BOARD_N];
    CopyBoard(board, g_game.cells);

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            int flips = CountFlipsOnBoard(board, r, c, player);
            if (flips > 0) {
                int next[BOARD_N][BOARD_N];
                CopyBoard(next, board);
                ApplyMoveOnBoard(next, r, c, player);
                int value = SearchBoard(next, g_game.search_depth - 1, Opponent(player), -1000000, 1000000);
                value += (player == BLUE ? 1 : -1) * (k_weights[r][c] + flips * 4);

                if ((player == BLUE && value > best.score) ||
                    (player == RED && value < best.score) ||
                    best.row < 0) {
                    best.row = r;
                    best.col = c;
                    best.score = value;
                }
            }
        }
    }

    return best;
}

static Move FindBestMove(int player)
{
    Move best;
    if (g_modernFindBestMove &&
        g_modernFindBestMove(&g_game.cells[0][0], player, g_game.search_depth, &best)) {
        return best;
    }
    return FindBestMoveLegacy(player);
}
