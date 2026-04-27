/* Included by reversi_win32.c. Board painting, piece cache, and visual animation frames. */

static void FillClassicFace(HDC hdc, const RECT *rc, COLORREF color, int cell_face)
{
    HBRUSH face;
    int delete_face = 0;

    if (g_monoDisplay) {
        face = (HBRUSH)GetStockObject(cell_face ? LTGRAY_BRUSH : GRAY_BRUSH);
    } else {
        face = CreateSolidBrush(color);
        delete_face = face != NULL;
    }

    if (!face) {
        face = (HBRUSH)GetStockObject(WHITE_BRUSH);
    }

    FillRect(hdc, rc, face);
    if (delete_face) {
        DeleteObject(face);
    }
}

static void Draw3DCell(HDC hdc, RECT rc)
{
    FillClassicFace(hdc, &rc, g_cellGray, 1);

    HPEN gray = CreatePen(PS_SOLID, 1, g_darkGray);
    HPEN white = CreatePen(PS_SOLID, 1, g_edgeLightColor);
    HPEN black = CreatePen(PS_SOLID, 1, g_edgeDarkColor);
    HGDIOBJ old_pen = SelectObject(hdc, gray);

    if (MirrorBoardView()) {
        MoveToEx(hdc, rc.right - 2, rc.bottom - 2, NULL);
        LineTo(hdc, rc.left + 2, rc.bottom - 2);
        LineTo(hdc, rc.left + 2, rc.top + 1);

        SelectObject(hdc, white);
        LineTo(hdc, rc.right - 1, rc.top + 1);
        LineTo(hdc, rc.right - 1, rc.bottom);

        SelectObject(hdc, black);
        MoveToEx(hdc, rc.right - 2, rc.bottom - 1, NULL);
        LineTo(hdc, rc.left + 1, rc.bottom - 1);
        LineTo(hdc, rc.left + 1, rc.top);
    } else {
        MoveToEx(hdc, rc.left + 2, rc.bottom - 2, NULL);
        LineTo(hdc, rc.right - 2, rc.bottom - 2);
        LineTo(hdc, rc.right - 2, rc.top + 1);

        SelectObject(hdc, white);
        LineTo(hdc, rc.left + 1, rc.top + 1);
        LineTo(hdc, rc.left + 1, rc.bottom);

        SelectObject(hdc, black);
        MoveToEx(hdc, rc.left + 2, rc.bottom - 1, NULL);
        LineTo(hdc, rc.right - 1, rc.bottom - 1);
        LineTo(hdc, rc.right - 1, rc.top);
    }

    SelectObject(hdc, old_pen);
    DeleteObject(gray);
    DeleteObject(white);
    DeleteObject(black);
}

static void DestroyCellCache(void)
{
    for (int i = 0; i < CELL_CACHE_BITMAPS; ++i) {
        if (g_cellCache.bitmap[i]) {
            DeleteObject(g_cellCache.bitmap[i]);
            g_cellCache.bitmap[i] = NULL;
        }
    }
    g_cellCache.valid = 0;
}

static int CellCacheIndex(int piece, int shrink_level)
{
    if (piece == EMPTY) {
        return 0;
    }
    if (shrink_level < 0) {
        shrink_level = 0;
    } else if (shrink_level >= CELL_CACHE_SHRINKS) {
        shrink_level = CELL_CACHE_SHRINKS - 1;
    }
    return (piece == RED ? 1 : 1 + CELL_CACHE_SHRINKS) + shrink_level;
}

static int GdiPlusDrawEllipse(GpGraphics *graphics, const RECT *rc, COLORREF fill, COLORREF edge)
{
    GpBrush *brush = NULL;
    GpPen *pen = NULL;
    int width = rc->right - rc->left;
    int height = rc->bottom - rc->top;
    int ok = 0;

    if (width <= 0 || height <= 0) {
        return 0;
    }

    if (g_gdiPlus.create_solid_fill(GdiPlusColor(fill), &brush) != 0 || !brush) {
        return 0;
    }
    if (g_gdiPlus.create_pen(GdiPlusColor(edge), 1.0f, 2, &pen) != 0 || !pen) {
        g_gdiPlus.delete_brush(brush);
        return 0;
    }

    if (g_gdiPlus.fill_ellipse(graphics, brush, rc->left, rc->top, width, height) == 0 &&
        g_gdiPlus.draw_ellipse(graphics, pen, rc->left, rc->top, width, height) == 0) {
        ok = 1;
    }

    g_gdiPlus.delete_pen(pen);
    g_gdiPlus.delete_brush(brush);
    return ok;
}

static int IsGdiPlusPieceRenderingAvailable(void)
{
    return g_gdiPlusReady && !g_monoDisplay;
}

static int ShouldDrawPiecesWithGdiPlus(void)
{
    return IsGdiPlusPieceRenderingAvailable() && !g_forceGdiPieces;
}

static int ShouldBuildCellCacheWithGdiPlus(void)
{
    return IsGdiPlusPieceRenderingAvailable() && !g_inSizeMove;
}

static int DrawPieceGdiPlus(HDC hdc, const RECT *white_rc, const RECT *gray_rc,
    const RECT *color_rc, COLORREF main_color)
{
    GpGraphics *graphics = NULL;

    if (!ShouldDrawPiecesWithGdiPlus()) {
        return 0;
    }

    if (g_gdiPlus.create_from_hdc(hdc, &graphics) != 0 || !graphics) {
        return 0;
    }

    g_gdiPlus.set_smoothing_mode(graphics, 4);
    g_gdiPlus.set_pixel_offset_mode(graphics, 4);

    if (!GdiPlusDrawEllipse(graphics, white_rc, kClassicLight, kClassicLight) ||
        !GdiPlusDrawEllipse(graphics, gray_rc, g_darkGray, g_darkGray) ||
        !GdiPlusDrawEllipse(graphics, color_rc, main_color, g_edgeDarkColor)) {
        g_gdiPlus.delete_graphics(graphics);
        return 0;
    }

    g_gdiPlus.delete_graphics(graphics);
    return 1;
}

static void DrawPieceRaw(HDC hdc, RECT cell, int piece, int shrink_level)
{
    int shrink_x;

    if (piece == EMPTY) {
        return;
    }

    if (shrink_level < 0) {
        shrink_level = 0;
    } else if (shrink_level >= CELL_CACHE_SHRINKS) {
        shrink_level = CELL_CACHE_SHRINKS - 1;
    }
    shrink_x = shrink_level * (cell.right - cell.left) / 8;

    COLORREF main_color = piece == RED ? g_redColor : g_blueColor;
    RECT white_rc = cell;
    RECT gray_rc = cell;
    RECT color_rc = cell;
    if (MirrorBoardView()) {
        white_rc.left += 6;
        white_rc.top += 3;
        white_rc.right -= 3;
        white_rc.bottom -= 6;
        gray_rc.left += 3;
        gray_rc.top += 6;
        gray_rc.right -= 6;
        gray_rc.bottom -= 3;
        color_rc.left += 5;
        color_rc.top += 4;
        color_rc.right -= 4;
        color_rc.bottom -= 5;
    } else {
        white_rc.left += 3;
        white_rc.top += 3;
        white_rc.right -= 6;
        white_rc.bottom -= 6;
        gray_rc.left += 6;
        gray_rc.top += 6;
        gray_rc.right -= 3;
        gray_rc.bottom -= 3;
        color_rc.left += 4;
        color_rc.top += 4;
        color_rc.right -= 5;
        color_rc.bottom -= 5;
    }
    InflateRect(&white_rc, -shrink_x, 0);
    InflateRect(&gray_rc, -shrink_x, 0);
    InflateRect(&color_rc, -shrink_x, 0);

    if (DrawPieceGdiPlus(hdc, &white_rc, &gray_rc, &color_rc, main_color)) {
        return;
    }

    HBRUSH white_brush = CreateSolidBrush(kClassicLight);
    HBRUSH gray_brush = CreateSolidBrush(g_darkGray);
    HBRUSH main_brush = CreateSolidBrush(main_color);
    HPEN white_pen = CreatePen(PS_SOLID, 1, kClassicLight);
    HPEN gray_pen = CreatePen(PS_SOLID, 1, g_darkGray);
    HPEN black_pen = CreatePen(PS_SOLID, 1, g_edgeDarkColor);
    HGDIOBJ old_brush = SelectObject(hdc, white_brush);
    HGDIOBJ old_pen = SelectObject(hdc, white_pen);

    Ellipse(hdc, white_rc.left, white_rc.top, white_rc.right, white_rc.bottom);
    SelectObject(hdc, gray_pen);
    SelectObject(hdc, gray_brush);
    Ellipse(hdc, gray_rc.left, gray_rc.top, gray_rc.right, gray_rc.bottom);
    SelectObject(hdc, black_pen);
    SelectObject(hdc, main_brush);
    Ellipse(hdc, color_rc.left, color_rc.top, color_rc.right, color_rc.bottom);

    SelectObject(hdc, old_brush);
    SelectObject(hdc, old_pen);
    DeleteObject(white_brush);
    DeleteObject(gray_brush);
    DeleteObject(main_brush);
    DeleteObject(white_pen);
    DeleteObject(gray_pen);
    DeleteObject(black_pen);
}

static void ResolvePieceFrame(int piece, const AnimCell *anim, int *draw_piece, int *shrink_level)
{
    *draw_piece = piece;
    *shrink_level = 0;

    if (anim && anim->active) {
        if (anim->step <= 2) {
            *draw_piece = anim->from;
            *shrink_level = anim->step + 1;
        } else {
            *draw_piece = anim->to;
            *shrink_level = 6 - anim->step;
        }
    }
}

static void DrawPiece(HDC hdc, RECT cell, int piece, const AnimCell *anim)
{
    int draw_piece;
    int shrink_level;

    ResolvePieceFrame(piece, anim, &draw_piece, &shrink_level);
    DrawPieceRaw(hdc, cell, draw_piece, shrink_level);
}

static void DrawHintPiece(HDC hdc, RECT rc)
{
    DrawPieceRaw(hdc, rc, RED, 0);
}

static int IsCellCacheCurrent(int cell_w, int cell_h)
{
    if (!g_cellCache.valid ||
        g_cellCache.cell_w != cell_w ||
        g_cellCache.cell_h != cell_h ||
        g_cellCache.mono_display != g_monoDisplay ||
        g_cellCache.rtl_layout != MirrorBoardView() ||
        g_cellCache.window_gray != g_windowGray ||
        g_cellCache.cell_gray != g_cellGray ||
        g_cellCache.dark_gray != g_darkGray ||
        g_cellCache.red_color != g_redColor ||
        g_cellCache.blue_color != g_blueColor ||
        g_cellCache.edge_light_color != g_edgeLightColor ||
        g_cellCache.edge_dark_color != g_edgeDarkColor) {
        return 0;
    }

    if (g_inSizeMove) {
        return 1;
    }

    return g_cellCache.gdiplus_ready == ShouldBuildCellCacheWithGdiPlus();
}

static int RenderCellCacheBitmap(HDC hdc, int index, int piece, int shrink_level)
{
    HDC cell_dc;
    HBITMAP bitmap;
    HGDIOBJ old_bitmap;
    RECT rc;

    cell_dc = CreateCompatibleDC(hdc);
    if (!cell_dc) {
        return 0;
    }

    bitmap = CreateCompatibleBitmap(hdc, g_cellCache.cell_w, g_cellCache.cell_h);
    if (!bitmap) {
        DeleteDC(cell_dc);
        return 0;
    }

    old_bitmap = SelectObject(cell_dc, bitmap);
    rc.left = 0;
    rc.top = 0;
    rc.right = g_cellCache.cell_w;
    rc.bottom = g_cellCache.cell_h;
    Draw3DCell(cell_dc, rc);
    DrawPieceRaw(cell_dc, rc, piece, shrink_level);
    if (old_bitmap) {
        SelectObject(cell_dc, old_bitmap);
    }
    DeleteDC(cell_dc);

    g_cellCache.bitmap[index] = bitmap;
    return 1;
}

static int EnsureCellCache(HDC hdc, int cell_w, int cell_h)
{
    int old_force_gdi;
    int use_gdiplus;

    if (cell_w <= 0 || cell_h <= 0) {
        return 0;
    }
    if (IsCellCacheCurrent(cell_w, cell_h)) {
        return 1;
    }

    use_gdiplus = ShouldBuildCellCacheWithGdiPlus();
    if (g_inSizeMove && !use_gdiplus && IsGdiPlusPieceRenderingAvailable()) {
        g_builtGdiCacheDuringSizeMove = 1;
    }
    old_force_gdi = g_forceGdiPieces;
    g_forceGdiPieces = !use_gdiplus;

    DestroyCellCache();
    g_cellCache.cell_w = cell_w;
    g_cellCache.cell_h = cell_h;
    g_cellCache.mono_display = g_monoDisplay;
    g_cellCache.gdiplus_ready = use_gdiplus;
    g_cellCache.rtl_layout = MirrorBoardView();
    g_cellCache.window_gray = g_windowGray;
    g_cellCache.cell_gray = g_cellGray;
    g_cellCache.dark_gray = g_darkGray;
    g_cellCache.red_color = g_redColor;
    g_cellCache.blue_color = g_blueColor;
    g_cellCache.edge_light_color = g_edgeLightColor;
    g_cellCache.edge_dark_color = g_edgeDarkColor;

    if (!RenderCellCacheBitmap(hdc, CellCacheIndex(EMPTY, 0), EMPTY, 0)) {
        g_forceGdiPieces = old_force_gdi;
        DestroyCellCache();
        return 0;
    }
    for (int shrink = 0; shrink < CELL_CACHE_SHRINKS; ++shrink) {
        if (!RenderCellCacheBitmap(hdc, CellCacheIndex(RED, shrink), RED, shrink) ||
            !RenderCellCacheBitmap(hdc, CellCacheIndex(BLUE, shrink), BLUE, shrink)) {
            g_forceGdiPieces = old_force_gdi;
            DestroyCellCache();
            return 0;
        }
    }

    g_forceGdiPieces = old_force_gdi;
    g_cellCache.valid = 1;
    return 1;
}

static int DrawCachedCell(HDC hdc, HDC cell_dc, RECT cell, int piece, const AnimCell *anim)
{
    int draw_piece;
    int shrink_level;
    int index;
    HGDIOBJ old_bitmap;

    if (!cell_dc || !g_cellCache.valid) {
        return 0;
    }

    ResolvePieceFrame(piece, anim, &draw_piece, &shrink_level);
    index = CellCacheIndex(draw_piece, shrink_level);
    if (!g_cellCache.bitmap[index]) {
        return 0;
    }

    old_bitmap = SelectObject(cell_dc, g_cellCache.bitmap[index]);
    if (!old_bitmap) {
        return 0;
    }
    BitBlt(hdc, cell.left, cell.top, cell.right - cell.left, cell.bottom - cell.top, cell_dc, 0, 0, SRCCOPY);
    SelectObject(cell_dc, old_bitmap);
    return 1;
}

static void DrawMessageLine(HWND hwnd, HDC hdc, const Layout *layout)
{
    RECT text_rc;
    HBRUSH face;
    int len;
    HFONT font;
    HGDIOBJ old_font;

    if (layout->message.bottom <= layout->message.top ||
        layout->message.right <= layout->message.left) {
        (void)hwnd;
        return;
    }

    face = CreateSolidBrush(g_messageBackColor);
    FillRect(hdc, &layout->message, face);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_messageTextColor);

    font = MessageFont(hwnd);
    old_font = font ? SelectObject(hdc, font) : NULL;
    len = APP_LSTRLEN(g_game.message);
    text_rc = layout->message;
    APP_DRAW_TEXT(
        hdc,
        g_game.message,
        len,
        &text_rc,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    if (old_font) {
        SelectObject(hdc, old_font);
    }

    if (g_game.flash_ticks > 0 && (g_game.flash_ticks & 1)) {
        InvertRect(hdc, &layout->message);
    }
    DeleteObject(face);
    (void)hwnd;
}

static void DrawGame(HWND hwnd, HDC hdc)
{
    Layout layout;
    CalculateLayout(hwnd, &layout);

    RECT client;
    GetClientRect(hwnd, &client);
    FillClassicFace(hdc, &client, g_windowGray, 0);
    SetBkMode(hdc, TRANSPARENT);

    int depth_x = layout.depth_x;
    int depth_y = layout.depth_y;
    int cache_ready = EnsureCellCache(hdc, layout.cell_w, layout.cell_h);
    HDC cell_dc = cache_ready ? CreateCompatibleDC(hdc) : NULL;

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            RECT cell = CellRect(&layout, r, c);

            AnimCell local_anim;
            AnimCell *anim = NULL;
            int piece = g_game.cells[r][c];
            if (g_anim.active && g_anim.row == r && g_anim.col == c) {
                local_anim = g_anim;
                anim = &local_anim;
            }
            if (g_placeFlash.active && g_placeFlash.row == r && g_placeFlash.col == c) {
                piece = g_placeFlash.visible ? g_placeFlash.piece : EMPTY;
                anim = NULL;
            }

            if (!DrawCachedCell(hdc, cell_dc, cell, piece, anim)) {
                Draw3DCell(hdc, cell);
                DrawPiece(hdc, cell, piece, anim);
            }
        }
    }

    if (cell_dc) {
        DeleteDC(cell_dc);
    }

    RECT bottom_shadow;
    RECT side_shadow;
    if (MirrorBoardView()) {
        bottom_shadow.left = layout.board.left - depth_x * 2;
        bottom_shadow.top = layout.board.bottom;
        bottom_shadow.right = layout.board.right - depth_x * 2;
        bottom_shadow.bottom = layout.board.bottom + depth_y * 2;
        side_shadow.left = layout.board.left - depth_x * 2;
        side_shadow.top = layout.board.top + depth_y * 2;
        side_shadow.right = layout.board.left;
        side_shadow.bottom = layout.board.bottom + depth_y * 2;
    } else {
        bottom_shadow.left = layout.board.left + depth_x * 2;
        bottom_shadow.top = layout.board.bottom;
        bottom_shadow.right = layout.board.right + depth_x * 2;
        bottom_shadow.bottom = layout.board.bottom + depth_y * 2;
        side_shadow.left = layout.board.right;
        side_shadow.top = layout.board.top + depth_y * 2;
        side_shadow.right = layout.board.right + depth_x * 2;
        side_shadow.bottom = layout.board.bottom + depth_y * 2;
    }
    HBRUSH black = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hdc, &bottom_shadow, black);
    FillRect(hdc, &side_shadow, black);

    HBRUSH slope = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    for (int i = 1; i < depth_x; ++i) {
        RECT side_strip;
        if (MirrorBoardView()) {
            side_strip.left = layout.board.left - i - 1;
            side_strip.top = layout.board.top + i / g_deviceAspect;
            side_strip.right = layout.board.left - i;
            side_strip.bottom = layout.board.bottom + i / g_deviceAspect;
        } else {
            side_strip.left = layout.board.right + i;
            side_strip.top = layout.board.top + i / g_deviceAspect;
            side_strip.right = layout.board.right + i + 1;
            side_strip.bottom = layout.board.bottom + i / g_deviceAspect;
        }
        FillRect(hdc, &side_strip, slope);
    }
    for (int i = 1; i < depth_y; ++i) {
        RECT bottom_strip;
        if (MirrorBoardView()) {
            bottom_strip.left = layout.board.left - g_deviceAspect * i;
            bottom_strip.top = layout.board.bottom + i;
            bottom_strip.right = layout.board.right - g_deviceAspect * i;
            bottom_strip.bottom = layout.board.bottom + i + 1;
        } else {
            bottom_strip.left = layout.board.left + g_deviceAspect * i;
            bottom_strip.top = layout.board.bottom + i;
            bottom_strip.right = layout.board.right + g_deviceAspect * i;
            bottom_strip.bottom = layout.board.bottom + i + 1;
        }
        FillRect(hdc, &bottom_strip, slope);
    }

    HPEN edge = CreatePen(PS_SOLID, 1, g_edgeDarkColor);
    HGDIOBJ old_pen = SelectObject(hdc, edge);
    if (MirrorBoardView()) {
        MoveToEx(hdc, layout.board.left, layout.board.bottom, NULL);
        LineTo(hdc, layout.board.left, layout.board.top);
        LineTo(hdc, layout.board.left - depth_x, layout.board.top + depth_y);
        LineTo(hdc, layout.board.left - depth_x, layout.board.bottom + depth_y);
        LineTo(hdc, layout.board.right - depth_x, layout.board.bottom + depth_y);
        LineTo(hdc, layout.board.right, layout.board.bottom);
        LineTo(hdc, layout.board.left, layout.board.bottom);
        LineTo(hdc, layout.board.left - depth_x, layout.board.bottom + depth_y);
        MoveToEx(hdc, layout.board.left, layout.board.top, NULL);
        LineTo(hdc, layout.board.right, layout.board.top);
        LineTo(hdc, layout.board.right, layout.board.bottom);
    } else {
        MoveToEx(hdc, layout.board.right, layout.board.bottom, NULL);
        LineTo(hdc, layout.board.right, layout.board.top);
        LineTo(hdc, layout.board.right + depth_x, layout.board.top + depth_y);
        LineTo(hdc, layout.board.right + depth_x, layout.board.bottom + depth_y);
        LineTo(hdc, layout.board.left + depth_x, layout.board.bottom + depth_y);
        LineTo(hdc, layout.board.left, layout.board.bottom);
        LineTo(hdc, layout.board.right, layout.board.bottom);
        LineTo(hdc, layout.board.right + depth_x, layout.board.bottom + depth_y);
        MoveToEx(hdc, layout.board.right, layout.board.top, NULL);
        LineTo(hdc, layout.board.left, layout.board.top);
        LineTo(hdc, layout.board.left, layout.board.bottom);
    }

    SelectObject(hdc, old_pen);
    DeleteObject(edge);

    if (g_game.message[0]) {
        DrawMessageLine(hwnd, hdc, &layout);
    }
}

static void DrawGameBuffered(HWND hwnd, HDC hdc)
{
    RECT client;
    HDC mem_dc;
    HBITMAP bitmap;
    HGDIOBJ old_bitmap;
    int width;
    int height;

    GetClientRect(hwnd, &client);
    width = client.right - client.left;
    height = client.bottom - client.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    mem_dc = CreateCompatibleDC(hdc);
    if (!mem_dc) {
        DrawGame(hwnd, hdc);
        return;
    }

    bitmap = CreateCompatibleBitmap(hdc, width, height);
    if (!bitmap) {
        DeleteDC(mem_dc);
        DrawGame(hwnd, hdc);
        return;
    }

    old_bitmap = SelectObject(mem_dc, bitmap);
    DrawGame(hwnd, mem_dc);
    BitBlt(hdc, 0, 0, width, height, mem_dc, 0, 0, SRCCOPY);
    if (old_bitmap) {
        SelectObject(mem_dc, old_bitmap);
    }
    DeleteObject(bitmap);
    DeleteDC(mem_dc);
}

static void DrawSingleCell(HWND hwnd, HDC hdc, int row, int col, int hint)
{
    Layout layout;
    RECT cell;

    CalculateLayout(hwnd, &layout);
    cell = CellRect(&layout, row, col);
    if (EnsureCellCache(hdc, layout.cell_w, layout.cell_h)) {
        HDC cell_dc = CreateCompatibleDC(hdc);
        int piece = (hint && g_game.cells[row][col] == EMPTY) ? RED : g_game.cells[row][col];
        if (!DrawCachedCell(hdc, cell_dc, cell, piece, NULL)) {
            Draw3DCell(hdc, cell);
            DrawPieceRaw(hdc, cell, piece, 0);
        }
        if (cell_dc) {
            DeleteDC(cell_dc);
        }
    } else {
        Draw3DCell(hdc, cell);
        DrawPiece(hdc, cell, g_game.cells[row][col], NULL);
        if (hint && g_game.cells[row][col] == EMPTY) {
            DrawHintPiece(hdc, cell);
        }
    }
}

static void FlashHintCell(HWND hwnd, int row, int col)
{
    HDC hdc = GetDC(hwnd);
    HCURSOR old_cursor;
    int restore_layout = 0;
    DWORD old_layout;

    if (!hdc) {
        return;
    }

    old_layout = AppBeginLtrDrawing(hdc, &restore_layout);
    g_hintBlankCursor = 1;
    old_cursor = SetCursor(NULL);
    for (int i = 0; i < 4; ++i) {
        DrawSingleCell(hwnd, hdc, row, col, (i & 1) == 0);
        PumpFor(150);
    }
    g_hintBlankCursor = 0;
    SetCursor(old_cursor);
    AppEndLtrDrawing(hdc, old_layout, restore_layout);
    ReleaseDC(hwnd, hdc);
}

static void AnimateFlips(HWND hwnd, Flip *flips, int count, int to_piece)
{
    int delay = AnimationDelay();
    if (delay <= 0) {
        for (int i = 0; i < count; ++i) {
            g_game.cells[flips[i].row][flips[i].col] = to_piece;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        return;
    }

    for (int i = 0; i < count; ++i) {
        int r = flips[i].row;
        int c = flips[i].col;
        int from = g_game.cells[r][c];
        for (int step = 0; step <= 5; ++step) {
            g_anim.active = 1;
            g_anim.row = r;
            g_anim.col = c;
            g_anim.from = from;
            g_anim.to = to_piece;
            g_anim.step = step;
            InvalidateCell(hwnd, r, c);
            UpdateWindow(hwnd);
            PumpFor(delay);
        }
        g_anim.active = 0;
        g_game.cells[r][c] = to_piece;
        InvalidateCell(hwnd, r, c);
        UpdateWindow(hwnd);
    }
}

static void FlashComputerPlacement(HWND hwnd, int row, int col)
{
    int flashes = g_game.animation_cmd == IDM_ANIM_SLOW ? 7 : 3;
    for (int i = 0; i < flashes; ++i) {
        g_placeFlash.active = 1;
        g_placeFlash.row = row;
        g_placeFlash.col = col;
        g_placeFlash.piece = BLUE;
        g_placeFlash.visible = (i & 1) == 0;
        InvalidateCell(hwnd, row, col);
        UpdateWindow(hwnd);
        PumpFor(150);
    }
    g_placeFlash.active = 0;
}
