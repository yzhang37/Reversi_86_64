#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <limits.h>
#include "resource.h"

#define BOARD_N 8
#define EMPTY 0
#define RED 1
#define BLUE 2

#define TIMER_FLASH 0x29A
#define MENU_POS_GAME 0
#define MENU_POS_OPTIONS 1
#define MENU_POS_HELP 2
#define MIN_TRACK_SIZE 200
#define INVALID_MOVE_TIP_REPEAT_COUNT 5
#define INVALID_MOVE_TIP_REPEAT_MS 5000
#define TEST_MENU_LABEL_LIMIT 64
#define PROCESS_SYSTEM_DPI_AWARE_LOCAL 1
#define PROCESS_PER_MONITOR_DPI_AWARE_LOCAL 2
#define MDT_EFFECTIVE_DPI_LOCAL 0
#define HH_DISPLAY_TOPIC_LOCAL 0x0000
#define HH_DISPLAY_TOC_LOCAL 0x0001
#define HH_DISPLAY_INDEX_LOCAL 0x0002
#define HH_CLOSE_ALL_LOCAL 0x0012
#ifndef HELP_FINDER
#define HELP_FINDER 0x000b
#endif
#ifndef MB_EXCLAMATION
#define MB_EXCLAMATION MB_ICONEXCLAMATION
#endif

#ifndef DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE ((HANDLE)(LONG_PTR)-2)
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE)(LONG_PTR)-4)
#endif
#ifndef SM_XVIRTUALSCREEN
#define SM_XVIRTUALSCREEN 76
#define SM_YVIRTUALSCREEN 77
#define SM_CXVIRTUALSCREEN 78
#define SM_CYVIRTUALSCREEN 79
#endif
#ifndef MONITOR_DEFAULTTONEAREST
DECLARE_HANDLE(HMONITOR);
#define MONITOR_DEFAULTTONEAREST 2
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif
#ifndef WM_ENTERSIZEMOVE
#define WM_ENTERSIZEMOVE 0x0231
#endif
#ifndef WM_EXITSIZEMOVE
#define WM_EXITSIZEMOVE 0x0232
#endif

#define DWMWA_USE_IMMERSIVE_DARK_MODE_OLD 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL 20
#define APP_PREFERRED_APP_MODE_DEFAULT 0
#define APP_PREFERRED_APP_MODE_ALLOW_DARK 1

#ifndef SPI_GETHIGHCONTRAST
#define SPI_GETHIGHCONTRAST 0x0042
#endif
#ifndef HCF_HIGHCONTRASTON
#define HCF_HIGHCONTRASTON 0x00000001
#endif

#if defined(_MSC_VER)
#define APP_NOINLINE __declspec(noinline)
#else
#define APP_NOINLINE
#endif

#ifdef UNICODE
typedef WCHAR APP_CHAR;
#define APP_TEXT(value) L##value
#define APP_GET_MODULE_HANDLE GetModuleHandleW
#define APP_GET_MODULE_FILENAME GetModuleFileNameW
#define APP_GET_FILE_ATTRIBUTES GetFileAttributesW
#define APP_GET_WINDOWS_DIRECTORY GetWindowsDirectoryW
#define APP_GET_COMMAND_LINE GetCommandLineW
#define APP_GET_STARTUP_INFO GetStartupInfoW
#define APP_GET_VERSION_EX GetVersionExW
#define APP_LOAD_LIBRARY LoadLibraryW
#define APP_LOAD_STRING LoadStringW
#define APP_LOAD_ICON LoadIconW
#define APP_LOAD_CURSOR LoadCursorW
#define APP_LOAD_MENU LoadMenuW
#define APP_LOAD_ACCELERATORS LoadAcceleratorsW
#define APP_REGISTER_CLASS RegisterClassW
#define APP_CREATE_WINDOW_EX CreateWindowExW
#define APP_DEF_WINDOW_PROC DefWindowProcW
#define APP_GET_MESSAGE GetMessageW
#define APP_PEEK_MESSAGE PeekMessageW
#define APP_DISPATCH_MESSAGE DispatchMessageW
#define APP_TRANSLATE_ACCELERATOR TranslateAcceleratorW
#define APP_SET_WINDOW_TEXT SetWindowTextW
#define APP_MESSAGE_BOX AppMessageBox
#define APP_SHELL_ABOUT ShellAboutW
#define APP_GET_TEXT_METRICS GetTextMetricsW
#define APP_GET_TEXT_EXTENT GetTextExtentPoint32W
#define APP_TEXT_OUT TextOutW
#define APP_DRAW_TEXT DrawTextW
#define APP_LSTRCPYN lstrcpynW
#define APP_LSTRLEN lstrlenW
#define APP_WSPRINTF wsprintfW
#define APP_WNDCLASS WNDCLASSW
#define APP_CREATE_ACTCTX_NAME "CreateActCtxW"
#else
typedef CHAR APP_CHAR;
#define APP_TEXT(value) value
#define APP_GET_MODULE_HANDLE GetModuleHandleA
#define APP_GET_MODULE_FILENAME AppGetModuleFileName
#define APP_GET_FILE_ATTRIBUTES AppGetFileAttributes
#define APP_GET_WINDOWS_DIRECTORY AppGetWindowsDirectory
#define APP_GET_COMMAND_LINE GetCommandLineA
#define APP_GET_STARTUP_INFO GetStartupInfoA
#define APP_GET_VERSION_EX GetVersionExA
#define APP_LOAD_LIBRARY LoadLibraryA
#define APP_LOAD_STRING AppLoadString
#define APP_LOAD_ICON AppLoadIcon
#define APP_LOAD_CURSOR AppLoadCursor
#define APP_LOAD_MENU AppLoadMenu
#define APP_LOAD_ACCELERATORS AppLoadAccelerators
#define APP_REGISTER_CLASS AppRegisterClass
#define APP_CREATE_WINDOW_EX AppCreateWindowEx
#define APP_DEF_WINDOW_PROC AppDefWindowProc
#define APP_GET_MESSAGE AppGetMessage
#define APP_PEEK_MESSAGE AppPeekMessage
#define APP_DISPATCH_MESSAGE AppDispatchMessage
#define APP_TRANSLATE_ACCELERATOR AppTranslateAccelerator
#define APP_SET_WINDOW_TEXT AppSetWindowText
#define APP_MESSAGE_BOX AppMessageBox
#define APP_SHELL_ABOUT AppShellAbout
#define APP_GET_TEXT_METRICS GetTextMetricsA
#define APP_GET_TEXT_EXTENT AppGetTextExtentPoint
#define APP_TEXT_OUT AppTextOut
#define APP_DRAW_TEXT AppDrawText
#define APP_LSTRCPYN lstrcpynA
#define APP_LSTRLEN lstrlenA
#define APP_WSPRINTF wsprintfA
#define APP_WNDCLASS WNDCLASSA
#define APP_CREATE_ACTCTX_NAME "CreateActCtxA"
#endif

#define APP_ACTCTX_FLAG_RESOURCE_NAME_VALID 0x00000008

typedef struct AppActCtx {
    ULONG cbSize;
    DWORD dwFlags;
    const APP_CHAR *lpSource;
    USHORT wProcessorArchitecture;
    LANGID wLangId;
    const APP_CHAR *lpAssemblyDirectory;
    const APP_CHAR *lpResourceName;
    const APP_CHAR *lpApplicationName;
    HMODULE hModule;
} APP_ACTCTX;

#pragma function(memset)
#pragma function(memcpy)
void *__cdecl memset(void *dest, int value, UINT_PTR count)
{
    BYTE *out = (BYTE *)dest;
    BYTE fill = (BYTE)value;
    while (count--) {
        *out++ = fill;
    }
    return dest;
}

void *__cdecl memcpy(void *dest, const void *src, UINT_PTR count)
{
    BYTE *out = (BYTE *)dest;
    const BYTE *in = (const BYTE *)src;
    while (count--) {
        *out++ = *in++;
    }
    return dest;
}

int _fltused = 0;

typedef struct Move {
    int row;
    int col;
    int score;
} Move;

typedef int (__cdecl *ModernFindBestMoveProc)(const int *cells, int player, int depth, Move *best);

#if defined(_M_IX86)
extern int __cdecl ReversiDetectModernCpu(int *has_avx);
extern int __cdecl ReversiModernFindBestMove(const int *cells, int player, int depth, Move *best);
#endif

typedef struct Flip {
    int row;
    int col;
} Flip;

typedef struct Layout {
    RECT message;
    RECT board;
    int cell;
    int cell_w;
    int cell_h;
    int depth_x;
    int depth_y;
} Layout;

typedef struct AnimCell {
    int active;
    int row;
    int col;
    int from;
    int to;
    int step;
} AnimCell;

typedef struct PlaceFlash {
    int active;
    int row;
    int col;
    int piece;
    int visible;
} PlaceFlash;

typedef struct Game {
    int cells[BOARD_N][BOARD_N];
    int turn;
    int game_over;
    int must_pass;
    int thinking;
    int skill_cmd;
    int search_depth;
    int animation_cmd;
    int opening_pass_available;
    int practice_title;
    int invalid_tip_count;
    int invalid_tip_error_mode;
    DWORD invalid_tip_last_tick;
    DWORD invalid_click_last_tick;
    int invalid_click_last_x;
    int invalid_click_last_y;
    int selected_row;
    int selected_col;
    int flash_ticks;
    APP_CHAR message[192];
} Game;

#define CELL_CACHE_SHRINKS 4
#define CELL_CACHE_BITMAPS 9

typedef struct CellCache {
    int valid;
    int cell_w;
    int cell_h;
    int mono_display;
    int gdiplus_ready;
    COLORREF window_gray;
    COLORREF cell_gray;
    COLORREF dark_gray;
    COLORREF red_color;
    COLORREF blue_color;
    COLORREF edge_light_color;
    COLORREF edge_dark_color;
    HBITMAP bitmap[CELL_CACHE_BITMAPS];
} CellCache;

static Game g_game;
static AnimCell g_anim;
static PlaceFlash g_placeFlash;
static CellCache g_cellCache;
static HINSTANCE g_hinst;
static COLORREF g_windowGray;
static COLORREF g_cellGray;
static COLORREF g_darkGray;
static COLORREF g_redColor;
static COLORREF g_blueColor;
static COLORREF g_edgeLightColor;
static COLORREF g_edgeDarkColor;
static COLORREF g_messageBackColor;
static COLORREF g_messageTextColor;
static COLORREF g_hintColor;
static int g_deviceAspect = 1;
static int g_monoDisplay = 0;
static int g_currentDpi = 96;
static int g_enableSystemScaling = 0;
static int g_perMonitorDpiAware = 0;
static int g_useWideCommands = 0;
static int g_darkModeSupported = 0;
static int g_darkModeEnabled = 0;
static int g_inputLock = 0;
static int g_hintBlankCursor = 0;
static int g_inSizeMove = 0;
static int g_sizedDuringSizeMove = 0;
static int g_builtGdiCacheDuringSizeMove = 0;
static int g_forceGdiPieces = 0;
static int g_scratchBoard = 0;
static int g_cpuHasSse2 = 0;
static int g_cpuHasAvx = 0;
static int g_configSkillCmd = IDM_INTERMEDIATE;
static int g_configAnimationCmd = IDM_ANIM_FAST;
static int g_configHasWindowRect = 0;
static int g_configRepairSettings = 0;
static RECT g_configWindowRect;
static ModernFindBestMoveProc g_modernFindBestMove = NULL;
static HANDLE g_visualStylesActCtx = INVALID_HANDLE_VALUE;
static ULONG_PTR g_visualStylesCookie = 0;
static HWND g_mainWindow = NULL;
static HFONT g_messageFont = NULL;
static int g_messageFontDpi = 0;
static int g_messageFontHeight = 0;
static HMENU g_testContextMenu = NULL;
static HHOOK g_messageBoxHook = NULL;
static HWND g_messageBoxOwner = NULL;

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

static const COLORREF kClassicLight = RGB(255, 255, 255);
static const COLORREF kClassicDark = RGB(0, 0, 0);

typedef LONG (WINAPI *RtlGetVersionProc)(OSVERSIONINFOW *);
typedef BOOL (WINAPI *SetProcessDpiAwarenessContextProc)(HANDLE);
typedef HRESULT (WINAPI *SetProcessDpiAwarenessProc)(int);
typedef BOOL (WINAPI *SetProcessDPIAwareProc)(void);
typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
typedef UINT (WINAPI *GetDpiForSystemProc)(void);
typedef int (WINAPI *GetSystemMetricsForDpiProc)(int, UINT);
typedef BOOL (WINAPI *SystemParametersInfoForDpiProc)(UINT, UINT, PVOID, UINT, UINT);
typedef HRESULT (WINAPI *GetDpiForMonitorProc)(HMONITOR, int, UINT *, UINT *);
typedef HMONITOR (WINAPI *MonitorFromWindowProc)(HWND, DWORD);
typedef HANDLE (WINAPI *LoadImageAProc)(HINSTANCE, LPCSTR, UINT, int, int, UINT);
typedef HANDLE (WINAPI *LoadImageWProc)(HINSTANCE, LPCWSTR, UINT, int, int, UINT);
typedef HANDLE (WINAPI *CreateActCtxProc)(const APP_ACTCTX *);
typedef BOOL (WINAPI *ActivateActCtxProc)(HANDLE, ULONG_PTR *);
typedef BOOL (WINAPI *DeactivateActCtxProc)(DWORD, ULONG_PTR);
typedef VOID (WINAPI *ReleaseActCtxProc)(HANDLE);
typedef BOOL (WINAPI *CheckMenuRadioItemProc)(HMENU, UINT, UINT, UINT, UINT);
typedef BOOL (WINAPI *WinHelpAProc)(HWND, LPCSTR, UINT, ULONG_PTR);
typedef BOOL (WINAPI *WinHelpWProc)(HWND, LPCWSTR, UINT, ULONG_PTR);
typedef HWND (WINAPI *HtmlHelpAProc)(HWND, LPCSTR, UINT, DWORD_PTR);
typedef HWND (WINAPI *HtmlHelpWProc)(HWND, LPCWSTR, UINT, DWORD_PTR);
typedef HRESULT (WINAPI *DwmSetWindowAttributeProc)(HWND, DWORD, LPCVOID, DWORD);
typedef int (WINAPI *SetPreferredAppModeProc)(int);
typedef BOOL (WINAPI *AllowDarkModeForAppProc)(BOOL);
typedef BOOL (WINAPI *AllowDarkModeForWindowProc)(HWND, BOOL);
typedef VOID (WINAPI *FlushMenuThemesProc)(void);
typedef HRESULT (WINAPI *SetWindowThemeProc)(HWND, LPCWSTR, LPCWSTR);
typedef struct GpGraphics GpGraphics;
typedef struct GpBrush GpBrush;
typedef struct GpPen GpPen;
typedef struct GdiplusStartupInputLocal {
    UINT32 GdiplusVersion;
    VOID *DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
} GdiplusStartupInputLocal;
typedef int (WINAPI *GdiplusStartupProc)(ULONG_PTR *, const GdiplusStartupInputLocal *, VOID *);
typedef VOID (WINAPI *GdiplusShutdownProc)(ULONG_PTR);
typedef int (WINAPI *GdipCreateFromHDCProc)(HDC, GpGraphics **);
typedef int (WINAPI *GdipDeleteGraphicsProc)(GpGraphics *);
typedef int (WINAPI *GdipSetSmoothingModeProc)(GpGraphics *, int);
typedef int (WINAPI *GdipSetPixelOffsetModeProc)(GpGraphics *, int);
typedef int (WINAPI *GdipCreateSolidFillProc)(DWORD, GpBrush **);
typedef int (WINAPI *GdipDeleteBrushProc)(GpBrush *);
typedef int (WINAPI *GdipCreatePen1Proc)(DWORD, float, int, GpPen **);
typedef int (WINAPI *GdipDeletePenProc)(GpPen *);
typedef int (WINAPI *GdipFillEllipseIProc)(GpGraphics *, GpBrush *, int, int, int, int);
typedef int (WINAPI *GdipDrawEllipseIProc)(GpGraphics *, GpPen *, int, int, int, int);

typedef struct GdiPlusApi {
    HMODULE module;
    ULONG_PTR token;
    GdiplusShutdownProc shutdown;
    GdipCreateFromHDCProc create_from_hdc;
    GdipDeleteGraphicsProc delete_graphics;
    GdipSetSmoothingModeProc set_smoothing_mode;
    GdipSetPixelOffsetModeProc set_pixel_offset_mode;
    GdipCreateSolidFillProc create_solid_fill;
    GdipDeleteBrushProc delete_brush;
    GdipCreatePen1Proc create_pen;
    GdipDeletePenProc delete_pen;
    GdipFillEllipseIProc fill_ellipse;
    GdipDrawEllipseIProc draw_ellipse;
} GdiPlusApi;

static GdiPlusApi g_gdiPlus;
static int g_gdiPlusReady = 0;

#include "reversi_platform.inc"

static int MinInt(int a, int b)
{
    return a < b ? a : b;
}

static int MaxInt(int a, int b)
{
    return a > b ? a : b;
}

static int AbsInt(int value)
{
    return value < 0 ? -value : value;
}

static int ScaleForDpi(int value)
{
    return g_enableSystemScaling ? MulDiv(value, g_currentDpi, 96) : value;
}

static int CurrentWindowDpi(void)
{
    return g_enableSystemScaling && g_currentDpi > 0 ? g_currentDpi : 96;
}

static void CalculateBoardLayout(int width, int height, Layout *layout)
{
    int calc_width = width;
    int calc_height = height;
    int cell_w;
    int cell_h;
    int board_w;
    int board_h;
    int depth_y;
    int depth_x;

    if (g_deviceAspect * calc_height > calc_width) {
        int min_width = g_deviceAspect * 45;
        if (calc_width < min_width) {
            calc_width = min_width;
        }
        cell_w = calc_width / 10;
        cell_h = cell_w / g_deviceAspect;
    } else {
        if (calc_height < 45) {
            calc_height = 45;
        }
        cell_h = calc_height / 10;
        cell_w = cell_h * g_deviceAspect;
    }
    if ((cell_w & 1) == 0) {
        --cell_w;
    }
    if ((cell_h & 1) == 0) {
        --cell_h;
    }
    if (cell_w < 1) {
        cell_w = 1;
    }
    if (cell_h < 1) {
        cell_h = 1;
    }

    board_w = cell_w * BOARD_N;
    board_h = cell_h * BOARD_N;
    depth_y = board_h / 30;
    depth_x = g_deviceAspect * depth_y;

    layout->cell = cell_w;
    layout->cell_w = cell_w;
    layout->cell_h = cell_h;
    layout->depth_x = depth_x;
    layout->depth_y = depth_y;
    layout->board.left = width > board_w ? (width - board_w) / 2 : 0;
    layout->board.top = height > board_h ? (height - board_h) / 2 : 0;
    layout->board.right = layout->board.left + board_w;
    layout->board.bottom = layout->board.top + board_h;
}

static int MessageTopSpaceForWindow(HWND hwnd)
{
    RECT rc;
    Layout layout;

    if (!hwnd || !GetClientRect(hwnd, &rc)) {
        return 0;
    }
    CalculateBoardLayout(rc.right - rc.left, rc.bottom - rc.top, &layout);
    return layout.board.top;
}

static void BuildFallbackMessageLogFont(LOGFONT *log_font, int dpi)
{
    AppZeroMemory(log_font, sizeof(*log_font));
    log_font->lfHeight = -MulDiv(12, IsValidDpi(dpi) ? dpi : 96, 72);
    log_font->lfWeight = FW_BOLD;
    log_font->lfCharSet = DEFAULT_CHARSET;
    log_font->lfOutPrecision = OUT_DEFAULT_PRECIS;
    log_font->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    log_font->lfQuality = DEFAULT_QUALITY;
    log_font->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
}

static int QueryCaptionLogFontForDpi(LOGFONT *log_font, int dpi)
{
    NONCLIENTMETRICS metrics;
    int got_metrics = 0;
    int got_dpi_metrics = 0;

    AppZeroMemory(&metrics, sizeof(metrics));
    metrics.cbSize = sizeof(metrics);

    if (IsValidDpi(dpi)) {
        SystemParametersInfoForDpiProc get_for_dpi =
            (SystemParametersInfoForDpiProc)AppGetProc("user32.dll", "SystemParametersInfoForDpi");
        if (get_for_dpi &&
            get_for_dpi(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0, (UINT)dpi)) {
            got_metrics = 1;
            got_dpi_metrics = 1;
        }
    }

    if (!got_metrics) {
        AppZeroMemory(&metrics, sizeof(metrics));
        metrics.cbSize = sizeof(metrics);
        if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0)) {
            got_metrics = 1;
        }
    }

    if (!got_metrics) {
        return 0;
    }

    *log_font = metrics.lfCaptionFont;
    if (!got_dpi_metrics && g_enableSystemScaling && IsValidDpi(dpi)) {
        int system_dpi = QuerySystemDpi();
        if (IsValidDpi(system_dpi) && system_dpi != dpi && log_font->lfHeight != 0) {
            log_font->lfHeight = MulDiv(log_font->lfHeight, dpi, system_dpi);
        }
    }
    if (log_font->lfHeight == 0) {
        log_font->lfHeight = -MulDiv(12, IsValidDpi(dpi) ? dpi : 96, 72);
    }
    log_font->lfWeight = FW_BOLD;
    return 1;
}

static int MessageFontHeightForWindow(HWND hwnd, int dpi, const LOGFONT *log_font)
{
    int safe_dpi = IsValidDpi(dpi) ? dpi : 96;
    int height = log_font && log_font->lfHeight ? AbsInt(log_font->lfHeight) : 0;
    int min_height = MulDiv(16, safe_dpi, 96);
    int scaled_height = 0;
    int top_space = MessageTopSpaceForWindow(hwnd);
    RECT rc;

    if (hwnd && GetClientRect(hwnd, &rc)) {
        int width = rc.right - rc.left;
        int height_px = rc.bottom - rc.top;
        int short_side = MinInt(width, height_px);
        if (short_side > 0) {
            scaled_height = short_side / 24;
        }
    }

    if (height < min_height) {
        height = min_height;
    }
    if (height < scaled_height) {
        height = scaled_height;
    }
    if (top_space > 0 && height > top_space) {
        height = top_space;
    }
    return height > 0 ? height : min_height;
}

static void ScaleMessageLogFont(HWND hwnd, LOGFONT *log_font, int dpi)
{
    int height = MessageFontHeightForWindow(hwnd, dpi, log_font);
    log_font->lfHeight = log_font->lfHeight > 0 ? height : -height;
    log_font->lfWeight = FW_BOLD;
}

static void DestroyMessageFont(void)
{
    if (g_messageFont) {
        DeleteObject(g_messageFont);
        g_messageFont = NULL;
    }
    g_messageFontDpi = 0;
    g_messageFontHeight = 0;
}

static void RefreshMessageFont(HWND hwnd)
{
    int dpi = CurrentWindowDpi();
    LOGFONT log_font;
    int height;
    HFONT font;

    if (!QueryCaptionLogFontForDpi(&log_font, dpi)) {
        BuildFallbackMessageLogFont(&log_font, dpi);
    }
    ScaleMessageLogFont(hwnd, &log_font, dpi);
    height = AbsInt(log_font.lfHeight);

    font = CreateFontIndirect(&log_font);
    if (!font) {
        return;
    }

    DestroyMessageFont();
    g_messageFont = font;
    g_messageFontDpi = dpi;
    g_messageFontHeight = height;
}

static HFONT MessageFont(HWND hwnd)
{
    HFONT font;
    int dpi = CurrentWindowDpi();
    LOGFONT log_font;
    int height;

    if (!QueryCaptionLogFontForDpi(&log_font, dpi)) {
        BuildFallbackMessageLogFont(&log_font, dpi);
    }
    height = MessageFontHeightForWindow(hwnd, dpi, &log_font);

    if (!g_messageFont || g_messageFontDpi != dpi || g_messageFontHeight != height) {
        RefreshMessageFont(hwnd);
    }
    if (g_messageFont) {
        return g_messageFont;
    }

    font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    return font ? font : (HFONT)GetStockObject(SYSTEM_FONT);
}

static int AppSystemMetricForDpi(int metric, int dpi)
{
    int value;
    int system_dpi;
    GetSystemMetricsForDpiProc get_metric_for_dpi =
        (GetSystemMetricsForDpiProc)AppGetProc("user32.dll", "GetSystemMetricsForDpi");
    if (get_metric_for_dpi && IsValidDpi(dpi)) {
        value = get_metric_for_dpi(metric, (UINT)dpi);
        if (value > 0) {
            return value;
        }
    }

    value = GetSystemMetrics(metric);
    if (value <= 0 || !g_enableSystemScaling || !IsValidDpi(dpi)) {
        return value;
    }

    system_dpi = QuerySystemDpi();
    if (!IsValidDpi(system_dpi) || system_dpi == dpi) {
        return value;
    }
    return MulDiv(value, dpi, system_dpi);
}

static void UpdateWindowIcons(HWND hwnd)
{
    int dpi = CurrentWindowDpi();
    int small_w = AppSystemMetricForDpi(SM_CXSMICON, dpi);
    int small_h = AppSystemMetricForDpi(SM_CYSMICON, dpi);
    int big_w = AppSystemMetricForDpi(SM_CXICON, dpi);
    int big_h = AppSystemMetricForDpi(SM_CYICON, dpi);
    HICON small_icon = AppLoadIconSized(
        g_hinst, MAKEINTRESOURCE(IDI_REVERSI), small_w, small_h);
    HICON big_icon = AppLoadIconSized(
        g_hinst, MAKEINTRESOURCE(IDI_REVERSI), big_w, big_h);

    if (small_icon) {
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)small_icon);
    }
    if (big_icon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)big_icon);
    }
}

static LONG ScaleWindowValueForDpi(LONG value, int from_dpi, int to_dpi)
{
    if (from_dpi <= 0 || to_dpi <= 0 || from_dpi == to_dpi) {
        return value;
    }
    return (LONG)MulDiv(value, to_dpi, from_dpi);
}

static LONG WindowValueToRegistry(LONG value)
{
    return ScaleWindowValueForDpi(value, CurrentWindowDpi(), 96);
}

static LONG WindowValueFromRegistry(DWORD value, DWORD saved_dpi)
{
    LONG signed_value = (LONG)value;
    if (saved_dpi < 24 || saved_dpi > 960) {
        return signed_value;
    }
    return ScaleWindowValueForDpi(signed_value, (int)saved_dpi, CurrentWindowDpi());
}

#include "reversi_help.inc"

#include "reversi_settings.inc"

static BOOL AppCheckMenuRadioItem(HMENU menu, UINT first, UINT last, UINT check)
{
    static int loaded = 0;
    static CheckMenuRadioItemProc check_radio = NULL;
    if (!loaded) {
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            check_radio = (CheckMenuRadioItemProc)GetProcAddress(user32, "CheckMenuRadioItem");
        }
        loaded = 1;
    }

    if (check_radio) {
        return check_radio(menu, first, last, check, MF_BYCOMMAND);
    }
    return FALSE;
}

#include "reversi_rules.inc"

static void CalculateLayout(HWND hwnd, Layout *layout)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    CalculateBoardLayout(width, height, layout);

    int text_h = 0;
    HDC hdc = GetDC(hwnd);
    if (hdc) {
        TEXTMETRIC tm;
        HFONT font = MessageFont(hwnd);
        HGDIOBJ old_font = font ? SelectObject(hdc, font) : NULL;
        if (APP_GET_TEXT_METRICS(hdc, &tm)) {
            text_h = tm.tmHeight;
        }
        if (old_font) {
            SelectObject(hdc, old_font);
        }
        ReleaseDC(hwnd, hdc);
    }
    if (text_h <= 0) {
        text_h = 16;
    }
    if (text_h > layout->board.top) {
        text_h = layout->board.top;
    }
    if (text_h < 0) {
        text_h = 0;
    }

    layout->message.left = 0;
    layout->message.top = 0;
    layout->message.right = width;
    layout->message.bottom = text_h;
}

static RECT CellRect(const Layout *layout, int row, int col)
{
    RECT rc;
    rc.left = layout->board.left + col * layout->cell_w;
    rc.top = layout->board.top + row * layout->cell_h;
    rc.right = rc.left + layout->cell_w;
    rc.bottom = rc.top + layout->cell_h;
    return rc;
}

static void InvalidateCell(HWND hwnd, int row, int col)
{
    Layout layout;
    CalculateLayout(hwnd, &layout);
    RECT rc = CellRect(&layout, row, col);
    InflateRect(&rc, 2, 2);
    InvalidateRect(hwnd, &rc, FALSE);
}

static void PumpFor(int ms)
{
    DWORD end = GetTickCount() + (DWORD)ms;
    MSG msg;
    while ((int)(end - GetTickCount()) > 0) {
        while (APP_PEEK_MESSAGE(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            APP_DISPATCH_MESSAGE(&msg);
        }
        Sleep(5);
    }
}

static int AnimationDelay(void)
{
    if (g_game.animation_cmd == IDM_ANIM_SLOW) {
        return 150;
    }
    if (g_game.animation_cmd == IDM_ANIM_FAST) {
        return 55;
    }
    return 0;
}

static void SetTitle(HWND hwnd)
{
    APP_CHAR title[64];
    LoadText(g_game.practice_title ? IDS_PRACTICE_TITLE : IDS_APP_TITLE, title, 64);
    APP_SET_WINDOW_TEXT(hwnd, title);
}

static void SetMessage(HWND hwnd, const APP_CHAR *message, int flash)
{
    if (message) {
        APP_LSTRCPYN(g_game.message, message, (int)(sizeof(g_game.message) / sizeof(g_game.message[0])));
    } else {
        g_game.message[0] = 0;
    }
    if (flash) {
        g_game.flash_ticks = 8;
        SetTimer(hwnd, TIMER_FLASH, 200, NULL);
    }

    Layout layout;
    CalculateLayout(hwnd, &layout);
    InvalidateRect(hwnd, &layout.message, TRUE);
}

static void SetGameOverMessage(HWND hwnd)
{
    int red = CountPieces(RED);
    int blue = CountPieces(BLUE);
    APP_CHAR buffer[128];

    if (red > blue) {
        APP_CHAR fmt[64];
        LoadText(IDS_WON_FMT, fmt, 64);
        APP_WSPRINTF(buffer, fmt, red - blue);
    } else if (blue > red) {
        APP_CHAR fmt[64];
        LoadText(IDS_LOST_FMT, fmt, 64);
        APP_WSPRINTF(buffer, fmt, blue - red);
    } else {
        LoadText(IDS_DRAW, buffer, 128);
    }
    SetMessage(hwnd, buffer, 1);
}

static int IsGameBusy(void)
{
    return g_inputLock > 0 || g_game.thinking || g_anim.active || g_placeFlash.active;
}

static void UpdateMenuChecks(HWND hwnd)
{
    HMENU menu = GetMenu(hwnd);
    if (!menu) {
        return;
    }

    int busy = IsGameBusy();
    HMENU options = GetSubMenu(menu, MENU_POS_OPTIONS);
    if (options) {
        if (!AppCheckMenuRadioItem(options, IDM_BEGINNER, IDM_MASTER, (UINT)g_game.skill_cmd)) {
            CheckMenuItem(options, IDM_BEGINNER, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, IDM_INTERMEDIATE, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, IDM_EXPERT, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, IDM_MASTER, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, (UINT)g_game.skill_cmd, MF_BYCOMMAND | MF_CHECKED);
        }
        if (!AppCheckMenuRadioItem(options, IDM_ANIM_NONE, IDM_ANIM_FAST, (UINT)g_game.animation_cmd)) {
            CheckMenuItem(options, IDM_ANIM_NONE, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, IDM_ANIM_SLOW, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, IDM_ANIM_FAST, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem(options, (UINT)g_game.animation_cmd, MF_BYCOMMAND | MF_CHECKED);
        }
        EnableMenuItem(options, IDM_BEGINNER, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_INTERMEDIATE, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_EXPERT, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_MASTER, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_ANIM_NONE, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_ANIM_SLOW, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
        EnableMenuItem(options, IDM_ANIM_FAST, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    }

    EnableMenuItem(menu, MENU_POS_GAME, MF_BYPOSITION | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, MENU_POS_OPTIONS, MF_BYPOSITION | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, MENU_POS_HELP, MF_BYPOSITION | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_NEW, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_EXIT, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_PASS, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HINT, MF_BYCOMMAND | (busy || g_game.game_over ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HELP_CONTENTS, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HELP_SEARCH, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HELP_USING, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_ABOUT, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    DrawMenuBar(hwnd);
}

static void ReloadMainMenu(HWND hwnd, int update_checks)
{
    HMENU old_menu;
    HMENU new_menu;

    if (!hwnd) {
        return;
    }

    new_menu = APP_LOAD_MENU(g_hinst, MAKEINTRESOURCE(IDR_MAINMENU));
    if (!new_menu) {
        return;
    }

    old_menu = GetMenu(hwnd);
    SetMenu(hwnd, new_menu);
    if (old_menu) {
        DestroyMenu(old_menu);
    }

    if (update_checks) {
        UpdateMenuChecks(hwnd);
    } else {
        DrawMenuBar(hwnd);
    }
}

static void BeginInputLock(HWND hwnd)
{
    ++g_inputLock;
    if (g_inputLock == 1) {
        UpdateMenuChecks(hwnd);
    }
}

static void EndInputLock(HWND hwnd)
{
    if (g_inputLock > 0) {
        --g_inputLock;
    }
    if (g_inputLock == 0) {
        UpdateMenuChecks(hwnd);
    }
}

static void InitGameState(void)
{
    AppZeroMemory(&g_game, sizeof(g_game));
    g_game.cells[3][3] = BLUE;
    g_game.cells[3][4] = RED;
    g_game.cells[4][3] = RED;
    g_game.cells[4][4] = BLUE;
    g_game.turn = RED;
    g_game.skill_cmd = NormalizeSkillCommand(g_configSkillCmd);
    g_game.search_depth = SkillDepthForCommand(g_game.skill_cmd);
    g_game.animation_cmd = NormalizeAnimationCommand(g_configAnimationCmd);
    g_game.opening_pass_available = 1;
    g_game.selected_row = 3;
    g_game.selected_col = 4;
}

static void NewGame(HWND hwnd)
{
    int skill_cmd = g_game.skill_cmd ? g_game.skill_cmd : IDM_INTERMEDIATE;
    int depth = g_game.search_depth ? g_game.search_depth : 2;
    int animation_cmd = g_game.animation_cmd ? g_game.animation_cmd : IDM_ANIM_FAST;

    InitGameState();
    g_game.skill_cmd = skill_cmd;
    g_game.search_depth = depth;
    g_game.animation_cmd = animation_cmd;
    g_game.message[0] = 0;
    g_game.practice_title = 0;
    SetTitle(hwnd);
    UpdateMenuChecks(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
}

enum {
    IDM_TEST_SCRATCH_BOARD = WM_APP + 1,
    IDM_TEST_PLAYER_PASS,
    IDM_TEST_COMPUTER_PASS,
    IDM_TEST_CANNOT_PASS,
    IDM_TEST_MUST_PASS,
    IDM_TEST_COMPUTER_MUST_PASS,
    IDM_TEST_NO_MOVES,
    IDM_TEST_WIN,
    IDM_TEST_LOSE
};

#include "reversi_test_menu.inc"

#include "reversi_paint.inc"

static int ApplyMove(HWND hwnd, int row, int col, int player, int animate)
{
    Flip flips[64];
    int count = CollectFlips(row, col, player, flips, 64);
    if (count <= 0) {
        return 0;
    }

    BeginInputLock(hwnd);

    if (player == BLUE) {
        FlashComputerPlacement(hwnd, row, col);
    }

    g_game.cells[row][col] = player;
    InvalidateCell(hwnd, row, col);
    UpdateWindow(hwnd);

    if (animate) {
        AnimateFlips(hwnd, flips, count, player);
    } else {
        for (int i = 0; i < count; ++i) {
            g_game.cells[flips[i].row][flips[i].col] = player;
        }
        InvalidateRect(hwnd, NULL, FALSE);
    }
    EndInputLock(hwnd);
    return 1;
}

static void FinishTurn(HWND hwnd);

static void ComputerTurn(HWND hwnd)
{
    if (g_game.game_over) {
        return;
    }

    if (!HasLegalMove(BLUE)) {
        if (!HasLegalMove(RED)) {
            g_game.game_over = 1;
            SetGameOverMessage(hwnd);
        } else {
            g_game.turn = RED;
            g_game.must_pass = 0;
            APP_CHAR message[128];
            LoadText(IDS_COMPUTER_PASS, message, 128);
            SetMessage(hwnd, message, 1);
        }
        UpdateMenuChecks(hwnd);
        return;
    }

    g_game.thinking = 1;
    UpdateMenuChecks(hwnd);
    HCURSOR wait_cursor = APP_LOAD_CURSOR(NULL, IDC_WAIT);
    HCURSOR old_cursor = SetCursor(wait_cursor);

    Move move = FindBestMove(BLUE);
    if (move.row >= 0) {
        ApplyMove(hwnd, move.row, move.col, BLUE, 1);
    }

    SetCursor(old_cursor);
    g_game.thinking = 0;
    g_game.turn = RED;
    FinishTurn(hwnd);
    UpdateMenuChecks(hwnd);
}

static void FinishTurn(HWND hwnd)
{
    if (!HasLegalMove(RED) && !HasLegalMove(BLUE)) {
        g_game.game_over = 1;
        SetGameOverMessage(hwnd);
        return;
    }

    if (g_game.turn == RED && !HasLegalMove(RED)) {
        g_game.must_pass = 1;
        APP_CHAR message[128];
        LoadText(IDS_MUST_PASS, message, 128);
        SetMessage(hwnd, message, 1);
        return;
    }

    g_game.must_pass = 0;
    if (g_game.turn == BLUE) {
        ComputerTurn(hwnd);
    } else {
        SetMessage(hwnd, NULL, 0);
    }
}

static int CanPlayerPass(void)
{
    return !g_game.game_over &&
        g_game.turn == RED &&
        (g_game.opening_pass_available || !HasLegalMove(RED));
}

static int CanAcceptPlayerMoveInput(void)
{
    return !g_game.game_over && g_game.turn == RED && !g_game.must_pass;
}

static void ResetInvalidMoveTip(void)
{
    g_game.invalid_tip_count = 0;
    g_game.invalid_tip_error_mode = 0;
    g_game.invalid_click_last_tick = 0;
}

static void ResetInvalidMoveRun(void)
{
    g_game.invalid_tip_count = 0;
    g_game.invalid_click_last_tick = 0;
}

static int IsInvalidMoveDoubleClick(int x, int y)
{
    DWORD now = GetTickCount();
    DWORD elapsed = now - g_game.invalid_click_last_tick;
    int max_dx = MaxInt(1, GetSystemMetrics(SM_CXDOUBLECLK));
    int max_dy = MaxInt(1, GetSystemMetrics(SM_CYDOUBLECLK));
    int is_double = g_game.invalid_click_last_tick &&
        elapsed <= GetDoubleClickTime() &&
        AbsInt(x - g_game.invalid_click_last_x) <= max_dx &&
        AbsInt(y - g_game.invalid_click_last_y) <= max_dy;

    g_game.invalid_click_last_tick = now;
    g_game.invalid_click_last_x = x;
    g_game.invalid_click_last_y = y;
    return is_double;
}

static int ShouldShowInvalidMoveTip(int force_tip)
{
    DWORD now = GetTickCount();

    ++g_game.invalid_tip_count;
    if (!g_game.invalid_tip_last_tick) {
        g_game.invalid_tip_last_tick = now;
        return 1;
    }

    if (force_tip) {
        g_game.invalid_tip_error_mode = 1;
        g_game.invalid_tip_last_tick = now;
        return 1;
    }

    if (g_game.invalid_tip_error_mode) {
        g_game.invalid_tip_last_tick = now;
        return 1;
    }

    if (g_game.invalid_tip_count >= INVALID_MOVE_TIP_REPEAT_COUNT &&
        (g_game.invalid_tip_count > INVALID_MOVE_TIP_REPEAT_COUNT ||
            (DWORD)(now - g_game.invalid_tip_last_tick) >= INVALID_MOVE_TIP_REPEAT_MS)) {
        g_game.invalid_tip_error_mode = 1;
        g_game.invalid_tip_last_tick = now;
        return 1;
    }

    return 0;
}

static void ShowInvalidMoveTip(HWND hwnd)
{
    APP_CHAR message[192];
    APP_CHAR title[64];
    LoadText(IDS_INVALID_MOVE, message, 192);
    LoadText(IDS_APP_TITLE, title, 64);
    APP_MESSAGE_BOX(hwnd, message, title, MB_OK | MB_ICONINFORMATION);
}

static void ReportInvalidMove(HWND hwnd, int force_tip)
{
    if (ShouldShowInvalidMoveTip(force_tip)) {
        ShowInvalidMoveTip(hwnd);
    } else {
        MessageBeep(MB_OK);
    }
}

static void PlayerMove(HWND hwnd, int row, int col, int force_tip)
{
    if (g_game.game_over || IsGameBusy() || g_game.turn != RED || g_game.must_pass) {
        MessageBeep(MB_ICONWARNING);
        return;
    }

    if (!ApplyMove(hwnd, row, col, RED, 1)) {
        ReportInvalidMove(hwnd, force_tip);
        return;
    }

    ResetInvalidMoveTip();
    g_game.opening_pass_available = 0;
    g_game.turn = BLUE;
    FinishTurn(hwnd);
}

static int PointToCell(HWND hwnd, int x, int y, int *row, int *col)
{
    Layout layout;
    CalculateLayout(hwnd, &layout);
    if (x < layout.board.left || x >= layout.board.right ||
        y < layout.board.top || y >= layout.board.bottom) {
        return 0;
    }

    *col = (x - layout.board.left) / layout.cell_w;
    *row = (y - layout.board.top) / layout.cell_h;
    return OnBoard(*row, *col);
}

static void ToggleScratchBoard(HWND hwnd)
{
    g_scratchBoard = !g_scratchBoard;
    if (g_testContextMenu) {
        CheckMenuItem(
            g_testContextMenu,
            IDM_TEST_SCRATCH_BOARD,
            MF_BYCOMMAND | (g_scratchBoard ? MF_CHECKED : MF_UNCHECKED));
    }
    SetCursor(APP_LOAD_CURSOR(NULL, IDC_ARROW));
    InvalidateRect(hwnd, NULL, FALSE);
}

static void ScratchBoardClick(HWND hwnd, int row, int col)
{
    int piece = g_game.cells[row][col];

    if (piece == EMPTY) {
        piece = RED;
    } else if (piece == RED) {
        piece = BLUE;
    } else {
        piece = EMPTY;
    }

    g_game.cells[row][col] = piece;
    g_game.selected_row = row;
    g_game.selected_col = col;
    InvalidateCell(hwnd, row, col);
}

static void HandleClick(HWND hwnd, LPARAM lparam)
{
    int x = GET_X_LPARAM(lparam);
    int y = GET_Y_LPARAM(lparam);
    int row;
    int col;
    if (PointToCell(hwnd, x, y, &row, &col)) {
        if (g_scratchBoard) {
            ScratchBoardClick(hwnd, row, col);
            return;
        }
        int force_tip = 0;
        if (CanAcceptPlayerMoveInput() && CollectFlips(row, col, RED, NULL, 0) <= 0) {
            force_tip = IsInvalidMoveDoubleClick(x, y);
        }
        g_game.selected_row = row;
        g_game.selected_col = col;
        PlayerMove(hwnd, row, col, force_tip);
    } else if (CanAcceptPlayerMoveInput()) {
        int force_tip = g_game.invalid_tip_error_mode || IsInvalidMoveDoubleClick(x, y);
        if (force_tip) {
            ReportInvalidMove(hwnd, 1);
        }
    }
}

static int IsPointInRectInclusive(const RECT *rc, int x, int y)
{
    return x >= rc->left && x < rc->right && y >= rc->top && y < rc->bottom;
}

static int IsBlankClientPoint(HWND hwnd, int x, int y)
{
    Layout layout;
    RECT occupied;

    CalculateLayout(hwnd, &layout);
    if (IsPointInRectInclusive(&layout.message, x, y)) {
        return 0;
    }

    occupied = layout.board;
    occupied.right += layout.depth_x * 2;
    occupied.bottom += layout.depth_y * 2;
    return !IsPointInRectInclusive(&occupied, x, y);
}

static void ShowTestContextMenu(HWND hwnd, LPARAM lparam)
{
    POINT pt;

    if (IsGameBusy() || !IsBlankClientPoint(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam))) {
        return;
    }
    if (!g_testContextMenu) {
        return;
    }
    CheckMenuItem(
        g_testContextMenu,
        IDM_TEST_SCRATCH_BOARD,
        MF_BYCOMMAND | (g_scratchBoard ? MF_CHECKED : MF_UNCHECKED));

    pt.x = GET_X_LPARAM(lparam);
    pt.y = GET_Y_LPARAM(lparam);
    ClientToScreen(hwnd, &pt);
    TrackPopupMenu(
        g_testContextMenu,
        TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN,
        pt.x,
        pt.y,
        0,
        hwnd,
        NULL);
}

static void TryPass(HWND hwnd)
{
    if (g_game.game_over || IsGameBusy() || g_game.turn != RED) {
        return;
    }

    if (!CanPlayerPass()) {
        MessageBeep(MB_ICONWARNING);
        APP_CHAR message[192];
        APP_CHAR title[64];
        LoadText(IDS_CANNOT_PASS, message, 192);
        LoadText(IDS_APP_TITLE, title, 64);
        APP_MESSAGE_BOX(hwnd, message, title, MB_OK | MB_ICONINFORMATION);
        return;
    }

    g_game.opening_pass_available = 0;
    g_game.must_pass = 0;
    ResetInvalidMoveRun();
    g_game.turn = BLUE;
    APP_CHAR message[64];
    LoadText(IDS_PASS_TEXT, message, 64);
    SetMessage(hwnd, message, 1);
    FinishTurn(hwnd);
}

static HCURSOR LoadLegalMoveCursor(void);

static void ShowHint(HWND hwnd)
{
    if (g_game.game_over || IsGameBusy() || g_game.turn != RED || g_game.must_pass) {
        MessageBeep(MB_ICONWARNING);
        return;
    }

    BeginInputLock(hwnd);
    SetCursor(APP_LOAD_CURSOR(NULL, IDC_WAIT));

    Move move = FindBestMove(RED);
    if (move.row < 0) {
        EndInputLock(hwnd);
        TryPass(hwnd);
        return;
    }

    g_game.practice_title = 1;
    ResetInvalidMoveRun();
    SetTitle(hwnd);
    g_game.selected_row = move.row;
    g_game.selected_col = move.col;

    Layout layout;
    CalculateLayout(hwnd, &layout);
    RECT rc = CellRect(&layout, move.row, move.col);
    POINT pt;
    pt.x = rc.left + layout.cell_w / 2;
    pt.y = rc.top + layout.cell_h / 2;
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
    FlashHintCell(hwnd, move.row, move.col);
    EndInputLock(hwnd);
    SetCursor(LoadLegalMoveCursor());
}

static void SetSkill(HWND hwnd, int cmd)
{
    g_game.skill_cmd = NormalizeSkillCommand(cmd);
    g_game.search_depth = SkillDepthForCommand(g_game.skill_cmd);
    g_configSkillCmd = g_game.skill_cmd;
    SaveGameSettings(hwnd);
    UpdateMenuChecks(hwnd);
}

static void SetAnimation(HWND hwnd, int cmd)
{
    g_game.animation_cmd = NormalizeAnimationCommand(cmd);
    g_configAnimationCmd = g_game.animation_cmd;
    SaveGameSettings(hwnd);
    UpdateMenuChecks(hwnd);
}

static void ShowHelp(HWND hwnd, int cmd)
{
    int opened = 0;
    int force_chm = AppIsVistaOrNewer();
    int prefer_chm = force_chm || AppCanPreferChmHelp();
    const APP_CHAR empty_keyword[] = APP_TEXT("");

    if (cmd == IDM_HELP_USING) {
        if (prefer_chm) {
            opened = OpenHelpViewerChmHelp(hwnd);
        }
        if (!opened && !force_chm) {
            opened = AppWinHelp(hwnd, NULL, HELP_HELPONHELP, 0);
        }
    } else if (cmd == IDM_HELP_SEARCH) {
        if (prefer_chm) {
            opened = OpenLocalChmHelp(hwnd, HH_DISPLAY_INDEX_LOCAL);
        }
        if (!opened && !force_chm) {
            opened = OpenLocalWinHelp(hwnd, HELP_PARTIALKEY, (ULONG_PTR)empty_keyword);
        }
    } else {
        if (prefer_chm) {
            opened = OpenLocalChmHelp(hwnd, HH_DISPLAY_TOPIC_LOCAL);
        }
        if (!opened && !force_chm) {
            opened = OpenLocalWinHelp(hwnd, HELP_FINDER, 0);
        }
    }

    if (opened) {
        return;
    }

    UINT text_id = IDS_HELP_CONTENTS_TEXT;
    if (cmd == IDM_HELP_SEARCH) {
        text_id = IDS_HELP_SEARCH_TEXT;
    } else if (cmd == IDM_HELP_USING) {
        text_id = IDS_HELP_USING_TEXT;
    }
    static APP_CHAR text[512];
    static APP_CHAR title[64];
    UINT icon = MB_ICONINFORMATION;
    if (cmd == IDM_HELP_SEARCH || cmd == IDM_HELP_USING) {
        icon = MB_EXCLAMATION;
    }
    LoadText(text_id, text, 512);
    LoadText(IDS_HELP_TITLE, title, 64);
    APP_MESSAGE_BOX(hwnd, text, title, MB_OK | icon);
}

static void MoveCursorToCell(HWND hwnd, int row, int col)
{
    Layout layout;
    RECT rc;
    POINT pt;

    CalculateLayout(hwnd, &layout);
    rc = CellRect(&layout, row, col);
    pt.x = rc.left + layout.cell_w / 2;
    pt.y = rc.top + layout.cell_h / 2;
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

static void MoveSelection(HWND hwnd, int dr, int dc)
{
    int row = g_game.selected_row;
    int col = g_game.selected_col;

    if (IsGameBusy()) {
        return;
    }
    if (!OnBoard(row, col)) {
        row = 0;
        col = 0;
    }

    row = (row + dr + BOARD_N) % BOARD_N;
    col = (col + dc + BOARD_N) % BOARD_N;
    g_game.selected_row = row;
    g_game.selected_col = col;
    MoveCursorToCell(hwnd, row, col);
}

static void HandleKey(HWND hwnd, WPARAM key)
{
    if (IsGameBusy()) {
        return;
    }

    switch (key) {
    case VK_F1:
        ShowHelp(hwnd, IDM_HELP_CONTENTS);
        break;
    case VK_F2:
        NewGame(hwnd);
        break;
    case VK_BACK:
        ShowHint(hwnd);
        break;
    case VK_SPACE:
        if (g_scratchBoard && OnBoard(g_game.selected_row, g_game.selected_col)) {
            ScratchBoardClick(hwnd, g_game.selected_row, g_game.selected_col);
        } else {
            PlayerMove(hwnd, g_game.selected_row, g_game.selected_col, 0);
        }
        break;
    case VK_LEFT:
        MoveSelection(hwnd, 0, -1);
        break;
    case VK_RIGHT:
        MoveSelection(hwnd, 0, 1);
        break;
    case VK_UP:
        MoveSelection(hwnd, -1, 0);
        break;
    case VK_DOWN:
        MoveSelection(hwnd, 1, 0);
        break;
    default:
        break;
    }
}

static HCURSOR LoadLegalMoveCursor(void)
{
    HCURSOR cursor = APP_LOAD_CURSOR(NULL, IDC_CROSS);
    if (!cursor) {
        cursor = APP_LOAD_CURSOR(NULL, IDC_ARROW);
    }
    return cursor;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_NCCREATE:
        g_mainWindow = hwnd;
        UpdateWindowDpi(hwnd);
        ApplyUxThemeDarkMode(hwnd);
        ApplyDwmDarkMode(hwnd);
        return TRUE;

    case WM_CREATE:
        UpdateWindowDpi(hwnd);
        ReloadMainMenu(hwnd, 0);
        ApplyAppThemeToWindow(hwnd);
        UpdateWindowIcons(hwnd);
        RefreshMessageFont(hwnd);
        InitTestContextMenu();
        InitGameState();
        SetTitle(hwnd);
        UpdateMenuChecks(hwnd);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_COMMAND:
    {
        UINT cmd = LOWORD(wparam);
        if (IsGameBusy()) {
            if (IsTestCommand(cmd)) {
                return 0;
            }
            switch (cmd) {
            case IDM_NEW:
            case IDM_PASS:
            case IDM_HINT:
            case IDM_EXIT:
            case IDM_BEGINNER:
            case IDM_INTERMEDIATE:
            case IDM_EXPERT:
            case IDM_MASTER:
            case IDM_ANIM_NONE:
            case IDM_ANIM_SLOW:
            case IDM_ANIM_FAST:
            case IDM_HELP_CONTENTS:
            case IDM_HELP_SEARCH:
            case IDM_HELP_USING:
            case IDM_ABOUT:
                return 0;
            default:
                break;
            }
        }
        switch (cmd) {
        case IDM_NEW:
            NewGame(hwnd);
            return 0;
        case IDM_PASS:
            TryPass(hwnd);
            return 0;
        case IDM_HINT:
            ShowHint(hwnd);
            return 0;
        case IDM_EXIT:
            DestroyWindow(hwnd);
            return 0;
        case IDM_BEGINNER:
        case IDM_INTERMEDIATE:
        case IDM_EXPERT:
        case IDM_MASTER:
            SetSkill(hwnd, LOWORD(wparam));
            return 0;
        case IDM_ANIM_NONE:
        case IDM_ANIM_SLOW:
        case IDM_ANIM_FAST:
            SetAnimation(hwnd, cmd);
            return 0;
        case IDM_TEST_SCRATCH_BOARD:
            ToggleScratchBoard(hwnd);
            return 0;
        case IDM_TEST_PLAYER_PASS:
        case IDM_TEST_COMPUTER_PASS:
        case IDM_TEST_CANNOT_PASS:
        case IDM_TEST_MUST_PASS:
        case IDM_TEST_COMPUTER_MUST_PASS:
        case IDM_TEST_NO_MOVES:
        case IDM_TEST_WIN:
        case IDM_TEST_LOSE:
            ApplyTestScenario(hwnd, cmd);
            return 0;
        case IDM_HELP_CONTENTS:
        case IDM_HELP_SEARCH:
        case IDM_HELP_USING:
            ShowHelp(hwnd, cmd);
            return 0;
        case IDM_ABOUT:
        {
            APP_CHAR title[64];
            APP_CHAR text[128];
            HICON icon = APP_LOAD_ICON(g_hinst, MAKEINTRESOURCE(IDI_REVERSI));
            LoadText(IDS_APP_TITLE, title, 64);
            LoadText(IDS_ABOUT_TEXT, text, 128);
            APP_SHELL_ABOUT(hwnd, title, text, icon);
            return 0;
        }
        default:
            break;
        }
        break;
    }

    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        if (!IsGameBusy()) {
            HandleClick(hwnd, lparam);
        }
        return 0;

    case WM_RBUTTONUP:
        SetFocus(hwnd);
        ShowTestContextMenu(hwnd, lparam);
        return 0;

    case WM_MOUSEMOVE: {
        int row;
        int col;
        if (g_hintBlankCursor) {
            SetCursor(NULL);
        } else if (IsGameBusy()) {
            SetCursor(APP_LOAD_CURSOR(NULL, IDC_WAIT));
        } else if (g_scratchBoard &&
            PointToCell(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), &row, &col)) {
            SetCursor(LoadLegalMoveCursor());
        } else if (!g_game.game_over && g_game.turn == RED && !g_game.must_pass &&
            PointToCell(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), &row, &col) &&
            CollectFlips(row, col, RED, NULL, 0) > 0) {
            SetCursor(LoadLegalMoveCursor());
        } else {
            SetCursor(APP_LOAD_CURSOR(NULL, IDC_ARROW));
        }
        return 0;
    }

    case WM_SETCURSOR:
        if (LOWORD(lparam) == HTCLIENT) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            int row;
            int col;
            if (g_hintBlankCursor) {
                SetCursor(NULL);
            } else if (IsGameBusy()) {
                SetCursor(APP_LOAD_CURSOR(NULL, IDC_WAIT));
            } else if (!g_game.game_over && g_game.turn == RED && !g_game.must_pass &&
                PointToCell(hwnd, pt.x, pt.y, &row, &col) &&
                CollectFlips(row, col, RED, NULL, 0) > 0) {
                SetCursor(LoadLegalMoveCursor());
            } else {
                SetCursor(APP_LOAD_CURSOR(NULL, IDC_ARROW));
            }
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
        HandleKey(hwnd, wparam);
        return 0;

    case WM_TIMER:
        if (wparam == TIMER_FLASH) {
            if (g_game.flash_ticks > 0) {
                --g_game.flash_ticks;
                Layout layout;
                CalculateLayout(hwnd, &layout);
                InvalidateRect(hwnd, &layout.message, FALSE);
            } else {
                KillTimer(hwnd, TIMER_FLASH);
            }
            return 0;
        }
        break;

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = (MINMAXINFO *)lparam;
        int min_track = ScaleForDpi(MIN_TRACK_SIZE);
        mmi->ptMinTrackSize.x = min_track;
        mmi->ptMinTrackSize.y = min_track;
        return 0;
    }

    case WM_ENTERSIZEMOVE:
        g_inSizeMove = 1;
        g_sizedDuringSizeMove = 0;
        g_builtGdiCacheDuringSizeMove = 0;
        return 0;

    case WM_EXITSIZEMOVE: {
        int should_redraw = g_sizedDuringSizeMove || g_builtGdiCacheDuringSizeMove;
        g_inSizeMove = 0;
        g_sizedDuringSizeMove = 0;
        g_builtGdiCacheDuringSizeMove = 0;
        if (should_redraw) {
            DestroyCellCache();
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
        }
        return 0;
    }

    case WM_DPICHANGED: {
        RECT *suggested = (RECT *)lparam;
        int dpi = (int)LOWORD(wparam);
        if (!IsValidDpi(dpi)) {
            dpi = (int)HIWORD(wparam);
        }
        g_currentDpi = NormalizeDpi(dpi);
        DestroyCellCache();
        UpdateWindowIcons(hwnd);
        RefreshMessageFont(hwnd);
        if (suggested) {
            SetWindowPos(
                hwnd,
                NULL,
                suggested->left,
                suggested->top,
                suggested->right - suggested->left,
                suggested->bottom - suggested->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_MOVE:
        if (g_perMonitorDpiAware && UpdateWindowDpi(hwnd)) {
            DestroyCellCache();
            UpdateWindowIcons(hwnd);
            RefreshMessageFont(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_WINDOWPOSCHANGED:
        if (g_perMonitorDpiAware && UpdateWindowDpi(hwnd)) {
            DestroyCellCache();
            UpdateWindowIcons(hwnd);
            RefreshMessageFont(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_SIZE:
        if (g_inSizeMove) {
            g_sizedDuringSizeMove = 1;
        }
        DestroyCellCache();
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_DISPLAYCHANGE:
        DestroyCellCache();
        if (g_perMonitorDpiAware) {
            if (UpdateWindowDpi(hwnd)) {
                UpdateWindowIcons(hwnd);
                RefreshMessageFont(hwnd);
            }
        } else {
            g_currentDpi = QuerySystemDpi();
            UpdateWindowIcons(hwnd);
            RefreshMessageFont(hwnd);
        }
        RefreshAppTheme(hwnd);
        return 0;

    case WM_SETTINGCHANGE:
        DestroyCellCache();
        if (!g_perMonitorDpiAware) {
            g_currentDpi = QuerySystemDpi();
            UpdateWindowIcons(hwnd);
        }
        RefreshMessageFont(hwnd);
        RefreshAppTheme(hwnd);
        return 0;

    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
        DestroyCellCache();
        RefreshMessageFont(hwnd);
        RefreshAppTheme(hwnd);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawGameBuffered(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        SaveGameSettings(hwnd);
        AppCloseHtmlHelp();
        if (!AppIsVistaOrNewer()) {
            CloseLegacyWinHelp(hwnd);
        }
        if (g_mainWindow == hwnd) {
            g_mainWindow = NULL;
        }
        DestroyCellCache();
        DestroyMessageFont();
        DestroyTestContextMenu();
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return APP_DEF_WINDOW_PROC(hwnd, msg, wparam, lparam);
}

static int RunSelfTest(void)
{
    InitGameState();

    if (SkillSettingToCommand(SkillCommandToSetting(IDM_MASTER)) != IDM_MASTER ||
        AnimationSettingToCommand(AnimationCommandToSetting(IDM_ANIM_SLOW)) != IDM_ANIM_SLOW ||
        !IsSkillSettingValid(0) ||
        !IsSkillSettingValid(3) ||
        IsSkillSettingValid(-1) ||
        IsSkillSettingValid(4) ||
        !IsAnimationSettingValid(0) ||
        !IsAnimationSettingValid(2) ||
        IsAnimationSettingValid(-1) ||
        IsAnimationSettingValid(3) ||
        SkillSettingToCommand(-1) != IDM_INTERMEDIATE ||
        SkillSettingToCommand(4) != IDM_INTERMEDIATE ||
        AnimationSettingToCommand(-1) != IDM_ANIM_FAST ||
        AnimationSettingToCommand(3) != IDM_ANIM_FAST) {
        return 1;
    }

    if (!HasLegalMove(RED) || !CanPlayerPass()) {
        return 2;
    }
    g_game.opening_pass_available = 0;
    if (CanPlayerPass()) {
        return 3;
    }

    if (CountPieces(RED) != 2 || CountPieces(BLUE) != 2) {
        return 4;
    }
    if (CollectFlips(2, 3, RED, NULL, 0) != 1 || !HasLegalMove(RED)) {
        return 5;
    }
    int board[BOARD_N][BOARD_N];
    CopyBoard(board, g_game.cells);
    if (!ApplyMoveOnBoard(board, 2, 3, RED) || board[3][3] != RED) {
        return 6;
    }
    Move move = FindBestMove(BLUE);
    if (move.row < 0 || move.col < 0) {
        return 7;
    }

    AppZeroMemory(g_game.cells, sizeof(g_game.cells));
    g_game.cells[0][0] = BLUE;
    g_game.cells[0][1] = RED;
    g_game.turn = RED;
    g_game.game_over = 0;
    g_game.opening_pass_available = 0;
    if (HasLegalMove(RED) || !HasLegalMove(BLUE) || !CanPlayerPass()) {
        return 8;
    }

    for (int row = 0; row < BOARD_N; ++row) {
        for (int col = 0; col < BOARD_N; ++col) {
            g_game.cells[row][col] = RED;
        }
    }
    g_game.turn = RED;
    g_game.game_over = 0;
    g_game.opening_pass_available = 0;
    if (HasLegalMove(RED) || HasLegalMove(BLUE)) {
        return 9;
    }
    return 0;
}

static int ReversiMain(HINSTANCE hinst, int show)
{
    g_hinst = hinst;
    InitWideCommandMode();
    InitModernDispatch();
    InitSystemDpiAwareness();
    InitClassicVisualStyles();
    InitAppTheme();
    InitClassicColors();
    LoadGameSettings();

    if (AppCommandHasSelfTest()) {
        return RunSelfTest();
    }

    InitGdiPlus();

    const APP_CHAR class_name[] = APP_TEXT("NativeReversiWindow");
    APP_CHAR title[64];
    LoadText(IDS_APP_TITLE, title, 64);

    APP_WNDCLASS wc;
    AppZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hinst;
    wc.hCursor = APP_LOAD_CURSOR(NULL, IDC_ARROW);
    wc.hIcon = AppLoadIconSized(
        hinst,
        MAKEINTRESOURCE(IDI_REVERSI),
        AppSystemMetricForDpi(SM_CXICON, CurrentWindowDpi()),
        AppSystemMetricForDpi(SM_CYICON, CurrentWindowDpi()));
    wc.hbrBackground = g_monoDisplay ?
        (HBRUSH)GetStockObject(GRAY_BRUSH) :
        CreateSolidBrush(g_windowGray);
    wc.lpszClassName = class_name;

    if (!APP_REGISTER_CLASS(&wc)) {
        APP_CHAR message[128];
        LoadText(IDS_REGISTER_FAIL, message, 128);
        APP_MESSAGE_BOX(NULL, message, title, MB_OK | MB_ICONERROR);
        ShutdownGdiPlus();
        ShutdownClassicVisualStyles();
        return 1;
    }

    int window_x = CW_USEDEFAULT;
    int window_y = CW_USEDEFAULT;
    int window_width = ScaleForDpi(500);
    int window_height = ScaleForDpi(540);
    if (g_configHasWindowRect) {
        window_x = g_configWindowRect.left;
        window_y = g_configWindowRect.top;
        window_width = g_configWindowRect.right - g_configWindowRect.left;
        window_height = g_configWindowRect.bottom - g_configWindowRect.top;
    }

    HWND hwnd = APP_CREATE_WINDOW_EX(
        0,
        class_name,
        title,
        WS_OVERLAPPEDWINDOW,
        window_x,
        window_y,
        window_width,
        window_height,
        NULL,
        NULL,
        hinst,
        NULL);

    if (!hwnd) {
        APP_CHAR message[128];
        LoadText(IDS_CREATE_FAIL, message, 128);
        APP_MESSAGE_BOX(NULL, message, title, MB_OK | MB_ICONERROR);
        ShutdownGdiPlus();
        ShutdownClassicVisualStyles();
        return 1;
    }

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);
    if (g_configRepairSettings) {
        SaveGameSettings(hwnd);
        g_configRepairSettings = 0;
    }

    HACCEL accel = APP_LOAD_ACCELERATORS(hinst, MAKEINTRESOURCE(IDR_MAINACCEL));
    MSG msg;
    while (APP_GET_MESSAGE(&msg, NULL, 0, 0) > 0) {
        if (!accel || !APP_TRANSLATE_ACCELERATOR(hwnd, accel, &msg)) {
            TranslateMessage(&msg);
            APP_DISPATCH_MESSAGE(&msg);
        }
    }

    int exit_code = (int)msg.wParam;
    ShutdownGdiPlus();
    ShutdownClassicVisualStyles();
    return exit_code;
}

void WINAPI ReversiEntry(void)
{
    STARTUPINFO startup_info;
    AppZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    APP_GET_STARTUP_INFO(&startup_info);

    int show = SW_SHOWDEFAULT;
    if (startup_info.dwFlags & STARTF_USESHOWWINDOW) {
        show = startup_info.wShowWindow;
    }

    ExitProcess((UINT)ReversiMain(APP_GET_MODULE_HANDLE(NULL), show));
}
