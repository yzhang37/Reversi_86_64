#include <emmintrin.h>

#define BOARD_N 8
#define EMPTY 0
#define RED 1
#define BLUE 2

typedef struct Move {
    int row;
    int col;
    int score;
} Move;

static const int k_dirs[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
    {0, 1}, {1, -1}, {1, 0}, {1, 1}
};

static const int k_weights[BOARD_N][BOARD_N] = {
    {120, -25, 20,  8,  8, 20, -25, 120},
    {-25, -45, -5, -5, -5, -5, -45, -25},
    { 20,  -5, 15,  3,  3, 15,  -5,  20},
    {  8,  -5,  3,  3,  3,  3,  -5,   8},
    {  8,  -5,  3,  3,  3,  3,  -5,   8},
    { 20,  -5, 15,  3,  3, 15,  -5,  20},
    {-25, -45, -5, -5, -5, -5, -45, -25},
    {120, -25, 20,  8,  8, 20, -25, 120}
};

static int Opponent(int player)
{
    return player == RED ? BLUE : RED;
}

static int OnBoard(int row, int col)
{
    return row >= 0 && row < BOARD_N && col >= 0 && col < BOARD_N;
}

static int MinInt(int a, int b)
{
    return a < b ? a : b;
}

static int MaxInt(int a, int b)
{
    return a > b ? a : b;
}

static void CopyBoard(int dst[BOARD_N][BOARD_N], const int src[BOARD_N][BOARD_N])
{
    __m128i *d = (__m128i *)dst;
    const __m128i *s = (const __m128i *)src;
    for (int i = 0; i < 16; ++i) {
        _mm_storeu_si128(d + i, _mm_loadu_si128(s + i));
    }
}

static int CountPiecesOnBoard(const int board[BOARD_N][BOARD_N], int player)
{
    __m128i target = _mm_set1_epi32(player);
    __m128i total = _mm_setzero_si128();
    const __m128i *cells = (const __m128i *)board;
    int counts[4];

    for (int i = 0; i < 16; ++i) {
        __m128i equal = _mm_cmpeq_epi32(_mm_loadu_si128(cells + i), target);
        total = _mm_sub_epi32(total, equal);
    }

    _mm_storeu_si128((__m128i *)counts, total);
    return counts[0] + counts[1] + counts[2] + counts[3];
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
    int score = (CountPiecesOnBoard(board, BLUE) - CountPiecesOnBoard(board, RED)) * 4;
    int red_moves = 0;
    int blue_moves = 0;

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            if (board[r][c] == BLUE) {
                score += k_weights[r][c];
            } else if (board[r][c] == RED) {
                score -= k_weights[r][c];
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

int __cdecl ReversiModernFindBestMove(const int *cells, int player, int depth, Move *best)
{
    int board[BOARD_N][BOARD_N];
    best->row = -1;
    best->col = -1;
    best->score = player == BLUE ? -1000000 : 1000000;

    CopyBoard(board, (const int (*)[BOARD_N])cells);

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            int flips = CountFlipsOnBoard(board, r, c, player);
            if (flips > 0) {
                int next[BOARD_N][BOARD_N];
                CopyBoard(next, board);
                ApplyMoveOnBoard(next, r, c, player);
                int value = SearchBoard(next, depth - 1, Opponent(player), -1000000, 1000000);
                value += (player == BLUE ? 1 : -1) * (k_weights[r][c] + flips * 4);

                if ((player == BLUE && value > best->score) ||
                    (player == RED && value < best->score) ||
                    best->row < 0) {
                    best->row = r;
                    best->col = c;
                    best->score = value;
                }
            }
        }
    }

    return best->row >= 0;
}
