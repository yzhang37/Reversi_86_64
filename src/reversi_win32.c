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
#define MENU_POS_TEST 2
#define MIN_TRACK_SIZE 200
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
#define APP_MESSAGE_BOX MessageBoxW
#define APP_SHELL_ABOUT ShellAboutW
#define APP_GET_TEXT_METRICS GetTextMetricsW
#define APP_GET_TEXT_EXTENT GetTextExtentPoint32W
#define APP_TEXT_OUT TextOutW
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
void *__cdecl memset(void *dest, int value, UINT_PTR count)
{
    BYTE *out = (BYTE *)dest;
    BYTE fill = (BYTE)value;
    while (count--) {
        *out++ = fill;
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
    int invalid_tip_shown;
    int selected_row;
    int selected_col;
    int flash_ticks;
    APP_CHAR message[192];
} Game;

static Game g_game;
static AnimCell g_anim;
static PlaceFlash g_placeFlash;
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

static void AppZeroMemory(void *ptr, UINT_PTR size)
{
    BYTE *bytes = (BYTE *)ptr;
    while (size--) {
        *bytes++ = 0;
    }
}

static FARPROC AppGetProc(const char *module_name, const char *proc_name)
{
    HMODULE module = GetModuleHandleA(module_name);
    if (!module) {
        module = LoadLibraryA(module_name);
    }
    return module ? GetProcAddress(module, proc_name) : NULL;
}

#ifdef UNICODE
static HICON AppLoadIconSized(HINSTANCE instance, LPCWSTR name, int cx, int cy)
{
    if (cx <= 0 || cy <= 0) {
        return LoadIconW(instance, name);
    }

    LoadImageWProc load_image =
        (LoadImageWProc)AppGetProc("user32.dll", "LoadImageW");
    if (load_image) {
        return (HICON)load_image(
            instance, name, IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR | LR_SHARED);
    }
    return LoadIconW(instance, name);
}
#endif

#ifndef UNICODE
typedef int (WINAPI *LoadStringWProc)(HINSTANCE, UINT, LPWSTR, int);
typedef HICON (WINAPI *LoadIconWProc)(HINSTANCE, LPCWSTR);
typedef HCURSOR (WINAPI *LoadCursorWProc)(HINSTANCE, LPCWSTR);
typedef HMENU (WINAPI *LoadMenuWProc)(HINSTANCE, LPCWSTR);
typedef HACCEL (WINAPI *LoadAcceleratorsWProc)(HINSTANCE, LPCWSTR);
typedef ATOM (WINAPI *RegisterClassWProc)(const WNDCLASSW *);
typedef HWND (WINAPI *CreateWindowExWProc)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef LRESULT (WINAPI *DefWindowProcWProc)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (WINAPI *GetMessageWProc)(LPMSG, HWND, UINT, UINT);
typedef BOOL (WINAPI *PeekMessageWProc)(LPMSG, HWND, UINT, UINT, UINT);
typedef LRESULT (WINAPI *DispatchMessageWProc)(const MSG *);
typedef int (WINAPI *TranslateAcceleratorWProc)(HWND, HACCEL, LPMSG);
typedef BOOL (WINAPI *SetWindowTextWProc)(HWND, LPCWSTR);
typedef int (WINAPI *MessageBoxWProc)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef int (WINAPI *ShellAboutWProc)(HWND, LPCWSTR, LPCWSTR, HICON);
typedef BOOL (WINAPI *GetTextExtentPoint32WProc)(HDC, LPCWSTR, int, LPSIZE);
typedef BOOL (WINAPI *TextOutWProc)(HDC, int, int, LPCWSTR, int);
typedef LPWSTR (WINAPI *GetCommandLineWProc)(void);
typedef DWORD (WINAPI *GetModuleFileNameWProc)(HMODULE, LPWSTR, DWORD);
typedef DWORD (WINAPI *GetFileAttributesWProc)(LPCWSTR);
typedef UINT (WINAPI *GetWindowsDirectoryWProc)(LPWSTR, UINT);
typedef LONG (WINAPI *RegCreateKeyExWProc)(
    HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, const SECURITY_ATTRIBUTES *, PHKEY, LPDWORD);
typedef LONG (WINAPI *RegOpenKeyExWProc)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
typedef LONG (WINAPI *RegQueryValueExWProc)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LONG (WINAPI *RegSetValueExWProc)(HKEY, LPCWSTR, DWORD, DWORD, const BYTE *, DWORD);

static void AppToWide(const char *src, WCHAR *dst, int cch)
{
    if (!dst || cch <= 0) {
        return;
    }
    dst[0] = 0;
    if (!src) {
        return;
    }
    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, cch);
    dst[cch - 1] = 0;
}

static void AppFromWide(const WCHAR *src, char *dst, int cch)
{
    if (!dst || cch <= 0) {
        return;
    }
    dst[0] = 0;
    if (!src) {
        return;
    }
    WideCharToMultiByte(CP_ACP, 0, src, -1, dst, cch, NULL, NULL);
    dst[cch - 1] = 0;
}

static int AppWideLen(const WCHAR *text)
{
    int len = 0;
    while (text && text[len]) {
        ++len;
    }
    return len;
}

static int AppWideStringContains(const WCHAR *text, const WCHAR *needle)
{
    if (!text || !needle || !needle[0]) {
        return 0;
    }

    for (; *text; ++text) {
        const WCHAR *scan = text;
        const WCHAR *match = needle;
        while (*scan && *match && *scan == *match) {
            ++scan;
            ++match;
        }
        if (!*match) {
            return 1;
        }
    }
    return 0;
}

static LPCWSTR AppResourceNameToWide(LPCSTR name, WCHAR *buffer, int cch)
{
    if (IS_INTRESOURCE(name)) {
        return MAKEINTRESOURCEW((ULONG_PTR)name);
    }
    AppToWide(name, buffer, cch);
    return buffer;
}

static DWORD AppGetModuleFileName(HMODULE module, char *buffer, DWORD cch)
{
    if (g_useWideCommands) {
        GetModuleFileNameWProc get_module_file_name =
            (GetModuleFileNameWProc)AppGetProc("kernel32.dll", "GetModuleFileNameW");
        if (get_module_file_name) {
            WCHAR wide[MAX_PATH];
            DWORD got = get_module_file_name(module, wide, MAX_PATH);
            if (got) {
                AppFromWide(wide, buffer, (int)cch);
                return (DWORD)APP_LSTRLEN(buffer);
            }
        }
    }
    return GetModuleFileNameA(module, buffer, cch);
}

static DWORD AppGetFileAttributes(const char *path)
{
    if (g_useWideCommands) {
        GetFileAttributesWProc get_file_attributes =
            (GetFileAttributesWProc)AppGetProc("kernel32.dll", "GetFileAttributesW");
        if (get_file_attributes) {
            WCHAR wide[MAX_PATH];
            AppToWide(path, wide, MAX_PATH);
            return get_file_attributes(wide);
        }
    }
    return GetFileAttributesA(path);
}

static UINT AppGetWindowsDirectory(char *buffer, UINT cch)
{
    if (g_useWideCommands) {
        GetWindowsDirectoryWProc get_windows_directory =
            (GetWindowsDirectoryWProc)AppGetProc("kernel32.dll", "GetWindowsDirectoryW");
        if (get_windows_directory) {
            WCHAR wide[MAX_PATH];
            UINT got = get_windows_directory(wide, MAX_PATH);
            if (got) {
                AppFromWide(wide, buffer, (int)cch);
                return (UINT)APP_LSTRLEN(buffer);
            }
        }
    }
    return GetWindowsDirectoryA(buffer, cch);
}

static int AppLoadString(HINSTANCE instance, UINT id, char *buffer, int cch)
{
    if (g_useWideCommands) {
        LoadStringWProc load_string =
            (LoadStringWProc)AppGetProc("user32.dll", "LoadStringW");
        if (load_string) {
            WCHAR wide[512];
            int got = load_string(instance, id, wide, (int)(sizeof(wide) / sizeof(wide[0])));
            if (got) {
                AppFromWide(wide, buffer, cch);
                return APP_LSTRLEN(buffer);
            }
        }
    }
    return LoadStringA(instance, id, buffer, cch);
}

static HICON AppLoadIcon(HINSTANCE instance, LPCSTR name)
{
    if (g_useWideCommands) {
        LoadIconWProc load_icon = (LoadIconWProc)AppGetProc("user32.dll", "LoadIconW");
        if (load_icon) {
            WCHAR wide_name[128];
            return load_icon(instance, AppResourceNameToWide(name, wide_name, 128));
        }
    }
    return LoadIconA(instance, name);
}

static HICON AppLoadIconSized(HINSTANCE instance, LPCSTR name, int cx, int cy)
{
    if (cx <= 0 || cy <= 0) {
        return AppLoadIcon(instance, name);
    }

    if (g_useWideCommands) {
        LoadImageWProc load_image =
            (LoadImageWProc)AppGetProc("user32.dll", "LoadImageW");
        if (load_image) {
            WCHAR wide_name[128];
            return (HICON)load_image(
                instance,
                AppResourceNameToWide(name, wide_name, 128),
                IMAGE_ICON,
                cx,
                cy,
                LR_DEFAULTCOLOR | LR_SHARED);
        }
    }

    LoadImageAProc load_image = (LoadImageAProc)AppGetProc("user32.dll", "LoadImageA");
    if (load_image) {
        return (HICON)load_image(
            instance, name, IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR | LR_SHARED);
    }
    return AppLoadIcon(instance, name);
}

static HCURSOR AppLoadCursor(HINSTANCE instance, LPCSTR name)
{
    if (g_useWideCommands) {
        LoadCursorWProc load_cursor =
            (LoadCursorWProc)AppGetProc("user32.dll", "LoadCursorW");
        if (load_cursor) {
            WCHAR wide_name[128];
            return load_cursor(instance, AppResourceNameToWide(name, wide_name, 128));
        }
    }
    return LoadCursorA(instance, name);
}

static HMENU AppLoadMenu(HINSTANCE instance, LPCSTR name)
{
    if (g_useWideCommands) {
        LoadMenuWProc load_menu = (LoadMenuWProc)AppGetProc("user32.dll", "LoadMenuW");
        if (load_menu) {
            WCHAR wide_name[128];
            return load_menu(instance, AppResourceNameToWide(name, wide_name, 128));
        }
    }
    return LoadMenuA(instance, name);
}

static HACCEL AppLoadAccelerators(HINSTANCE instance, LPCSTR name)
{
    if (g_useWideCommands) {
        LoadAcceleratorsWProc load_accel =
            (LoadAcceleratorsWProc)AppGetProc("user32.dll", "LoadAcceleratorsW");
        if (load_accel) {
            WCHAR wide_name[128];
            return load_accel(instance, AppResourceNameToWide(name, wide_name, 128));
        }
    }
    return LoadAcceleratorsA(instance, name);
}

static ATOM AppRegisterClass(const WNDCLASSA *wc)
{
    if (g_useWideCommands) {
        RegisterClassWProc register_class =
            (RegisterClassWProc)AppGetProc("user32.dll", "RegisterClassW");
        if (register_class) {
            WCHAR class_name[128];
            WCHAR menu_name[128];
            WNDCLASSW wide_wc;
            AppZeroMemory(&wide_wc, sizeof(wide_wc));
            wide_wc.style = wc->style;
            wide_wc.lpfnWndProc = wc->lpfnWndProc;
            wide_wc.cbClsExtra = wc->cbClsExtra;
            wide_wc.cbWndExtra = wc->cbWndExtra;
            wide_wc.hInstance = wc->hInstance;
            wide_wc.hIcon = wc->hIcon;
            wide_wc.hCursor = wc->hCursor;
            wide_wc.hbrBackground = wc->hbrBackground;
            wide_wc.lpszMenuName = wc->lpszMenuName ?
                AppResourceNameToWide(wc->lpszMenuName, menu_name, 128) : NULL;
            wide_wc.lpszClassName = AppResourceNameToWide(wc->lpszClassName, class_name, 128);
            return register_class(&wide_wc);
        }
    }
    return RegisterClassA(wc);
}

static HWND AppCreateWindowEx(
    DWORD ex_style, LPCSTR class_name, LPCSTR window_name, DWORD style,
    int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
{
    if (g_useWideCommands) {
        CreateWindowExWProc create_window =
            (CreateWindowExWProc)AppGetProc("user32.dll", "CreateWindowExW");
        if (create_window) {
            WCHAR wide_class[128];
            WCHAR wide_title[256];
            return create_window(
                ex_style,
                AppResourceNameToWide(class_name, wide_class, 128),
                window_name ? (AppToWide(window_name, wide_title, 256), wide_title) : NULL,
                style, x, y, width, height, parent, menu, instance, param);
        }
    }
    return CreateWindowExA(
        ex_style, class_name, window_name, style,
        x, y, width, height, parent, menu, instance, param);
}

static LRESULT AppDefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (g_useWideCommands) {
        DefWindowProcWProc def_proc =
            (DefWindowProcWProc)AppGetProc("user32.dll", "DefWindowProcW");
        if (def_proc) {
            return def_proc(hwnd, msg, wparam, lparam);
        }
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

static BOOL AppGetMessage(LPMSG msg, HWND hwnd, UINT min_filter, UINT max_filter)
{
    if (g_useWideCommands) {
        GetMessageWProc get_message = (GetMessageWProc)AppGetProc("user32.dll", "GetMessageW");
        if (get_message) {
            return get_message(msg, hwnd, min_filter, max_filter);
        }
    }
    return GetMessageA(msg, hwnd, min_filter, max_filter);
}

static BOOL AppPeekMessage(LPMSG msg, HWND hwnd, UINT min_filter, UINT max_filter, UINT remove)
{
    if (g_useWideCommands) {
        PeekMessageWProc peek_message =
            (PeekMessageWProc)AppGetProc("user32.dll", "PeekMessageW");
        if (peek_message) {
            return peek_message(msg, hwnd, min_filter, max_filter, remove);
        }
    }
    return PeekMessageA(msg, hwnd, min_filter, max_filter, remove);
}

static LRESULT AppDispatchMessage(const MSG *msg)
{
    if (g_useWideCommands) {
        DispatchMessageWProc dispatch_message =
            (DispatchMessageWProc)AppGetProc("user32.dll", "DispatchMessageW");
        if (dispatch_message) {
            return dispatch_message(msg);
        }
    }
    return DispatchMessageA(msg);
}

static int AppTranslateAccelerator(HWND hwnd, HACCEL accel, LPMSG msg)
{
    if (g_useWideCommands) {
        TranslateAcceleratorWProc translate_accel =
            (TranslateAcceleratorWProc)AppGetProc("user32.dll", "TranslateAcceleratorW");
        if (translate_accel) {
            return translate_accel(hwnd, accel, msg);
        }
    }
    return TranslateAcceleratorA(hwnd, accel, msg);
}

static BOOL AppSetWindowText(HWND hwnd, const char *text)
{
    if (g_useWideCommands) {
        SetWindowTextWProc set_text =
            (SetWindowTextWProc)AppGetProc("user32.dll", "SetWindowTextW");
        if (set_text) {
            WCHAR wide[256];
            AppToWide(text, wide, 256);
            return set_text(hwnd, wide);
        }
    }
    return SetWindowTextA(hwnd, text);
}

static int AppMessageBox(HWND hwnd, const char *text, const char *caption, UINT type)
{
    if (g_useWideCommands) {
        MessageBoxWProc message_box =
            (MessageBoxWProc)AppGetProc("user32.dll", "MessageBoxW");
        if (message_box) {
            WCHAR wide_text[512];
            WCHAR wide_caption[128];
            AppToWide(text, wide_text, 512);
            AppToWide(caption, wide_caption, 128);
            return message_box(hwnd, wide_text, wide_caption, type);
        }
    }
    return MessageBoxA(hwnd, text, caption, type);
}

static int AppShellAbout(HWND hwnd, const char *title, const char *text, HICON icon)
{
    if (g_useWideCommands) {
        ShellAboutWProc shell_about =
            (ShellAboutWProc)AppGetProc("shell32.dll", "ShellAboutW");
        if (shell_about) {
            WCHAR wide_title[128];
            WCHAR wide_text[256];
            AppToWide(title, wide_title, 128);
            AppToWide(text, wide_text, 256);
            return shell_about(hwnd, wide_title, wide_text, icon);
        }
    }
    return ShellAboutA(hwnd, title, text, icon);
}

static BOOL AppGetTextExtentPoint(HDC hdc, const char *text, int len, LPSIZE size)
{
    if (g_useWideCommands) {
        GetTextExtentPoint32WProc get_extent =
            (GetTextExtentPoint32WProc)AppGetProc("gdi32.dll", "GetTextExtentPoint32W");
        if (get_extent) {
            WCHAR wide[256];
            AppToWide(text, wide, 256);
            return get_extent(hdc, wide, AppWideLen(wide), size);
        }
    }
    return GetTextExtentPoint32A(hdc, text, len, size);
}

static BOOL AppTextOut(HDC hdc, int x, int y, const char *text, int len)
{
    if (g_useWideCommands) {
        TextOutWProc text_out = (TextOutWProc)AppGetProc("gdi32.dll", "TextOutW");
        if (text_out) {
            WCHAR wide[256];
            AppToWide(text, wide, 256);
            return text_out(hdc, x, y, wide, AppWideLen(wide));
        }
    }
    return TextOutA(hdc, x, y, text, len);
}
#endif

static int AppStringContains(const APP_CHAR *text, const APP_CHAR *needle);

static int AppCommandHasSelfTest(void)
{
#ifdef UNICODE
    return AppStringContains(APP_GET_COMMAND_LINE(), APP_TEXT("--self-test"));
#else
    if (g_useWideCommands) {
        GetCommandLineWProc get_command_line =
            (GetCommandLineWProc)AppGetProc("kernel32.dll", "GetCommandLineW");
        if (get_command_line) {
            return AppWideStringContains(get_command_line(), L"--self-test");
        }
    }
    return AppStringContains(GetCommandLineA(), "--self-test");
#endif
}

static int QueryWindowsVersion(OSVERSIONINFOW *version)
{
    AppZeroMemory(version, sizeof(*version));
    version->dwOSVersionInfoSize = sizeof(*version);

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll) {
        RtlGetVersionProc rtl_get_version =
            (RtlGetVersionProc)GetProcAddress(ntdll, "RtlGetVersion");
        if (rtl_get_version && rtl_get_version(version) == 0) {
            return 1;
        }
    }

#pragma warning(push)
#pragma warning(disable: 4996)
    OSVERSIONINFOA ansi_version;
    AppZeroMemory(&ansi_version, sizeof(ansi_version));
    ansi_version.dwOSVersionInfoSize = sizeof(ansi_version);
    if (!GetVersionExA(&ansi_version)) {
        return 0;
    }
    version->dwMajorVersion = ansi_version.dwMajorVersion;
    version->dwMinorVersion = ansi_version.dwMinorVersion;
    version->dwBuildNumber = ansi_version.dwBuildNumber;
    version->dwPlatformId = ansi_version.dwPlatformId;
    return 1;
#pragma warning(pop)
}

static int IsWindowsVersionAtLeast(const OSVERSIONINFOW *version, DWORD major, DWORD minor)
{
    return version->dwMajorVersion > major ||
        (version->dwMajorVersion == major && version->dwMinorVersion >= minor);
}

static int IsWindowsBuildAtLeast(const OSVERSIONINFOW *version, DWORD major, DWORD minor, DWORD build)
{
    if (!IsWindowsVersionAtLeast(version, major, minor)) {
        return 0;
    }
    if (version->dwMajorVersion != major || version->dwMinorVersion != minor) {
        return 1;
    }
    return version->dwBuildNumber >= build;
}

static void InitWideCommandMode(void)
{
#ifdef UNICODE
    g_useWideCommands = 1;
#else
    OSVERSIONINFOW version;
    g_useWideCommands =
        QueryWindowsVersion(&version) &&
        version.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        IsWindowsVersionAtLeast(&version, 4, 0);
#endif
}

static void InitModernDispatch(void)
{
#if defined(_M_IX86)
    OSVERSIONINFOW version;
    int has_avx = 0;
    if (!QueryWindowsVersion(&version) ||
        version.dwPlatformId != VER_PLATFORM_WIN32_NT ||
        !IsWindowsVersionAtLeast(&version, 5, 0)) {
        return;
    }

    g_cpuHasSse2 = ReversiDetectModernCpu(&has_avx);
    g_cpuHasAvx = has_avx;
    if (g_cpuHasSse2) {
        g_modernFindBestMove = ReversiModernFindBestMove;
    }
#else
    g_cpuHasSse2 = 1;
    g_cpuHasAvx = 0;
#endif
}

static int AppCanUseGdiPlus(void)
{
    OSVERSIONINFOW version;
    return QueryWindowsVersion(&version) &&
        version.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        IsWindowsVersionAtLeast(&version, 5, 0);
}

static DWORD GdiPlusColor(COLORREF color)
{
    return 0xFF000000u |
        ((DWORD)GetRValue(color) << 16) |
        ((DWORD)GetGValue(color) << 8) |
        (DWORD)GetBValue(color);
}

static void InitGdiPlus(void)
{
    HMODULE module;
    GdiplusStartupProc startup;
    GdiplusStartupInputLocal input;

    AppZeroMemory(&g_gdiPlus, sizeof(g_gdiPlus));
    g_gdiPlusReady = 0;

    if (!AppCanUseGdiPlus()) {
        return;
    }

    module = LoadLibraryA("gdiplus.dll");
    if (!module) {
        return;
    }

    startup = (GdiplusStartupProc)GetProcAddress(module, "GdiplusStartup");
    g_gdiPlus.shutdown = (GdiplusShutdownProc)GetProcAddress(module, "GdiplusShutdown");
    g_gdiPlus.create_from_hdc =
        (GdipCreateFromHDCProc)GetProcAddress(module, "GdipCreateFromHDC");
    g_gdiPlus.delete_graphics =
        (GdipDeleteGraphicsProc)GetProcAddress(module, "GdipDeleteGraphics");
    g_gdiPlus.set_smoothing_mode =
        (GdipSetSmoothingModeProc)GetProcAddress(module, "GdipSetSmoothingMode");
    g_gdiPlus.set_pixel_offset_mode =
        (GdipSetPixelOffsetModeProc)GetProcAddress(module, "GdipSetPixelOffsetMode");
    g_gdiPlus.create_solid_fill =
        (GdipCreateSolidFillProc)GetProcAddress(module, "GdipCreateSolidFill");
    g_gdiPlus.delete_brush =
        (GdipDeleteBrushProc)GetProcAddress(module, "GdipDeleteBrush");
    g_gdiPlus.create_pen = (GdipCreatePen1Proc)GetProcAddress(module, "GdipCreatePen1");
    g_gdiPlus.delete_pen = (GdipDeletePenProc)GetProcAddress(module, "GdipDeletePen");
    g_gdiPlus.fill_ellipse =
        (GdipFillEllipseIProc)GetProcAddress(module, "GdipFillEllipseI");
    g_gdiPlus.draw_ellipse =
        (GdipDrawEllipseIProc)GetProcAddress(module, "GdipDrawEllipseI");

    if (!startup || !g_gdiPlus.shutdown || !g_gdiPlus.create_from_hdc ||
        !g_gdiPlus.delete_graphics || !g_gdiPlus.set_smoothing_mode ||
        !g_gdiPlus.set_pixel_offset_mode || !g_gdiPlus.create_solid_fill ||
        !g_gdiPlus.delete_brush || !g_gdiPlus.create_pen || !g_gdiPlus.delete_pen ||
        !g_gdiPlus.fill_ellipse || !g_gdiPlus.draw_ellipse) {
        FreeLibrary(module);
        AppZeroMemory(&g_gdiPlus, sizeof(g_gdiPlus));
        return;
    }

    AppZeroMemory(&input, sizeof(input));
    input.GdiplusVersion = 1;
    if (startup(&g_gdiPlus.token, &input, NULL) != 0) {
        FreeLibrary(module);
        AppZeroMemory(&g_gdiPlus, sizeof(g_gdiPlus));
        return;
    }

    g_gdiPlus.module = module;
    g_gdiPlusReady = 1;
}

static void ShutdownGdiPlus(void)
{
    if (g_gdiPlusReady && g_gdiPlus.shutdown) {
        g_gdiPlus.shutdown(g_gdiPlus.token);
    }
    if (g_gdiPlus.module) {
        FreeLibrary(g_gdiPlus.module);
    }
    AppZeroMemory(&g_gdiPlus, sizeof(g_gdiPlus));
    g_gdiPlusReady = 0;
}

static void InitClassicVisualStyles(void)
{
    OSVERSIONINFOW version;
    if (!QueryWindowsVersion(&version)) {
        return;
    }
    if (!IsWindowsVersionAtLeast(&version, 5, 1)) {
        return;
    }

    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (!kernel32) {
        return;
    }

    CreateActCtxProc create_actctx =
        (CreateActCtxProc)GetProcAddress(kernel32, APP_CREATE_ACTCTX_NAME);
    ActivateActCtxProc activate_actctx =
        (ActivateActCtxProc)GetProcAddress(kernel32, "ActivateActCtx");
    ReleaseActCtxProc release_actctx =
        (ReleaseActCtxProc)GetProcAddress(kernel32, "ReleaseActCtx");
    if (!create_actctx || !activate_actctx || !release_actctx) {
        return;
    }

    APP_CHAR module_path[MAX_PATH];
    if (!APP_GET_MODULE_FILENAME(g_hinst, module_path, MAX_PATH)) {
        return;
    }

    APP_ACTCTX actctx;
    AppZeroMemory(&actctx, sizeof(actctx));
    actctx.cbSize = sizeof(actctx);
    actctx.dwFlags = APP_ACTCTX_FLAG_RESOURCE_NAME_VALID;
    actctx.lpSource = module_path;
    actctx.lpResourceName = MAKEINTRESOURCE(IDR_VISUAL_STYLES_MANIFEST);

    HANDLE handle = create_actctx(&actctx);
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }

    ULONG_PTR cookie = 0;
    if (!activate_actctx(handle, &cookie)) {
        release_actctx(handle);
        return;
    }

    g_visualStylesActCtx = handle;
    g_visualStylesCookie = cookie;
}

static void ShutdownClassicVisualStyles(void)
{
    if (g_visualStylesActCtx == INVALID_HANDLE_VALUE) {
        return;
    }

    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (kernel32) {
        DeactivateActCtxProc deactivate_actctx =
            (DeactivateActCtxProc)GetProcAddress(kernel32, "DeactivateActCtx");
        ReleaseActCtxProc release_actctx =
            (ReleaseActCtxProc)GetProcAddress(kernel32, "ReleaseActCtx");
        if (deactivate_actctx && g_visualStylesCookie) {
            deactivate_actctx(0, g_visualStylesCookie);
        }
        if (release_actctx) {
            release_actctx(g_visualStylesActCtx);
        }
    }

    g_visualStylesActCtx = INVALID_HANDLE_VALUE;
    g_visualStylesCookie = 0;
}

static void InitSystemDpiAwareness(void)
{
    OSVERSIONINFOW version;
    g_enableSystemScaling = 0;
    g_perMonitorDpiAware = 0;

    if (!QueryWindowsVersion(&version)) {
        return;
    }

    if (!IsWindowsVersionAtLeast(&version, 6, 0)) {
        return;
    }

    if (IsWindowsBuildAtLeast(&version, 10, 0, 15063)) {
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            SetProcessDpiAwarenessContextProc set_context =
                (SetProcessDpiAwarenessContextProc)GetProcAddress(
                    user32, "SetProcessDpiAwarenessContext");
            if (set_context &&
                set_context(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
                g_enableSystemScaling = 1;
                g_perMonitorDpiAware = 1;
                return;
            }
        }
    }

    if (IsWindowsVersionAtLeast(&version, 6, 3)) {
        HMODULE shcore = LoadLibraryA("shcore.dll");
        if (shcore) {
            SetProcessDpiAwarenessProc set_awareness =
                (SetProcessDpiAwarenessProc)GetProcAddress(
                    shcore, "SetProcessDpiAwareness");
            if (set_awareness) {
                HRESULT hr = set_awareness(PROCESS_PER_MONITOR_DPI_AWARE_LOCAL);
                FreeLibrary(shcore);
                if (SUCCEEDED(hr)) {
                    g_enableSystemScaling = 1;
                    g_perMonitorDpiAware = 1;
                    return;
                }
            } else {
                FreeLibrary(shcore);
            }
        }
    }

    if (IsWindowsVersionAtLeast(&version, 6, 0)) {
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            SetProcessDPIAwareProc set_dpi_aware =
                (SetProcessDPIAwareProc)GetProcAddress(user32, "SetProcessDPIAware");
            if (set_dpi_aware && set_dpi_aware()) {
                g_enableSystemScaling = 1;
            }
        }
    }
}

static int NormalizeDpi(int dpi)
{
    return dpi >= 24 && dpi <= 960 ? dpi : 96;
}

static int IsValidDpi(int dpi)
{
    return dpi >= 24 && dpi <= 960;
}

static int DpiFromHdc(HDC hdc)
{
    if (!hdc) {
        return 96;
    }
    return NormalizeDpi(GetDeviceCaps(hdc, LOGPIXELSX));
}

static int QuerySystemDpi(void)
{
    if (!g_enableSystemScaling) {
        return 96;
    }

    GetDpiForSystemProc get_dpi_for_system =
        (GetDpiForSystemProc)AppGetProc("user32.dll", "GetDpiForSystem");
    if (get_dpi_for_system) {
        return NormalizeDpi((int)get_dpi_for_system());
    }

    HDC hdc = GetDC(NULL);
    if (hdc) {
        int dpi = DpiFromHdc(hdc);
        ReleaseDC(NULL, hdc);
        return dpi;
    }
    return 96;
}

static HMONITOR AppMonitorFromWindow(HWND hwnd)
{
    MonitorFromWindowProc monitor_from_window =
        (MonitorFromWindowProc)AppGetProc("user32.dll", "MonitorFromWindow");
    return monitor_from_window && hwnd ?
        monitor_from_window(hwnd, MONITOR_DEFAULTTONEAREST) : NULL;
}

static int QueryMonitorDpi(HMONITOR monitor)
{
    if (!g_enableSystemScaling || !g_perMonitorDpiAware || !monitor) {
        return 0;
    }

    GetDpiForMonitorProc get_dpi_for_monitor =
        (GetDpiForMonitorProc)AppGetProc("shcore.dll", "GetDpiForMonitor");
    if (get_dpi_for_monitor) {
        UINT dpi_x = 0;
        UINT dpi_y = 0;
        if (SUCCEEDED(get_dpi_for_monitor(monitor, MDT_EFFECTIVE_DPI_LOCAL, &dpi_x, &dpi_y))) {
            return NormalizeDpi((int)(dpi_x ? dpi_x : dpi_y));
        }
    }
    return 0;
}

static int QueryWindowDpi(HWND hwnd)
{
    if (!g_enableSystemScaling) {
        return 96;
    }

    if (g_perMonitorDpiAware && hwnd) {
        GetDpiForWindowProc get_dpi_for_window =
            (GetDpiForWindowProc)AppGetProc("user32.dll", "GetDpiForWindow");
        if (get_dpi_for_window) {
            int dpi = (int)get_dpi_for_window(hwnd);
            if (IsValidDpi(dpi)) {
                return dpi;
            }
        }

        int monitor_dpi = QueryMonitorDpi(AppMonitorFromWindow(hwnd));
        if (monitor_dpi) {
            return monitor_dpi;
        }
    }

    return QuerySystemDpi();
}

static int UpdateWindowDpi(HWND hwnd)
{
    int dpi = QueryWindowDpi(hwnd);
    if (dpi != g_currentDpi) {
        g_currentDpi = dpi;
        return 1;
    }
    return 0;
}

static int AppShouldUseLegacyMonoDisplayFallback(void)
{
    OSVERSIONINFOW version;
    if (!QueryWindowsVersion(&version)) {
        return 0;
    }

    if (version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        return 1;
    }

    return version.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        !IsWindowsVersionAtLeast(&version, 5, 1);
}

static void RefreshClassicColors(void)
{
    HDC hdc = GetDC(NULL);
    if (hdc) {
        COLORREF nearest_red = GetNearestColor(hdc, RGB(255, 0, 0));
        COLORREF nearest_blue = GetNearestColor(hdc, RGB(0, 0, 255));
        int planes = GetDeviceCaps(hdc, PLANES);
        int bits_per_pixel = GetDeviceCaps(hdc, BITSPIXEL);
        int color_count = GetDeviceCaps(hdc, NUMCOLORS);
        int mono_caps =
            (planes > 0 && bits_per_pixel > 0 && planes * bits_per_pixel <= 1) ||
            (color_count > 0 && color_count <= 2) ||
            nearest_red == nearest_blue;

        g_deviceAspect = GetDeviceCaps(hdc, VERTRES) == 200 ? 2 : 1;
        g_monoDisplay = AppShouldUseLegacyMonoDisplayFallback() && mono_caps;
        if (g_monoDisplay) {
            g_redColor = kClassicLight;
            g_blueColor = kClassicDark;
            g_windowGray = RGB(128, 128, 128);
            g_cellGray = RGB(192, 192, 192);
            g_darkGray = kClassicDark;
            g_edgeLightColor = kClassicLight;
            g_edgeDarkColor = kClassicDark;
            g_messageBackColor = kClassicLight;
            g_messageTextColor = kClassicDark;
            g_hintColor = kClassicDark;
        } else {
            g_redColor = nearest_red;
            g_blueColor = nearest_blue;
            g_windowGray = GetNearestColor(hdc, g_darkModeEnabled ? RGB(96, 96, 96) : RGB(170, 170, 170));
            g_cellGray = GetNearestColor(hdc, g_darkModeEnabled ? RGB(150, 150, 150) : RGB(192, 192, 192));
            g_darkGray = GetNearestColor(hdc, RGB(85, 85, 85));
            g_edgeLightColor = GetNearestColor(hdc, kClassicLight);
            g_edgeDarkColor = GetNearestColor(hdc, kClassicDark);
            g_messageBackColor = GetNearestColor(hdc, g_darkModeEnabled ? RGB(64, 64, 64) : kClassicLight);
            g_messageTextColor = GetNearestColor(hdc, g_darkModeEnabled ? RGB(240, 240, 240) : kClassicDark);
            g_hintColor = GetNearestColor(hdc, g_darkModeEnabled ? RGB(240, 240, 240) : kClassicDark);
        }
        ReleaseDC(NULL, hdc);
    } else {
        g_deviceAspect = 1;
        g_monoDisplay = 0;
        g_redColor = RGB(255, 0, 0);
        g_blueColor = RGB(0, 0, 255);
        g_windowGray = g_darkModeEnabled ? RGB(96, 96, 96) : RGB(170, 170, 170);
        g_cellGray = g_darkModeEnabled ? RGB(150, 150, 150) : RGB(192, 192, 192);
        g_darkGray = RGB(85, 85, 85);
        g_edgeLightColor = kClassicLight;
        g_edgeDarkColor = kClassicDark;
        g_messageBackColor = g_darkModeEnabled ? RGB(64, 64, 64) : kClassicLight;
        g_messageTextColor = g_darkModeEnabled ? RGB(240, 240, 240) : kClassicDark;
        g_hintColor = g_darkModeEnabled ? RGB(240, 240, 240) : kClassicDark;
    }
}

static void InitClassicColors(void)
{
    g_currentDpi = QuerySystemDpi();
    RefreshClassicColors();
}

static void LoadText(UINT id, APP_CHAR *buffer, int cch)
{
    if (!APP_LOAD_STRING(g_hinst, id, buffer, cch)) {
        if (cch > 0) {
            buffer[0] = 0;
        }
    }
}

static int MinInt(int a, int b)
{
    return a < b ? a : b;
}

static int MaxInt(int a, int b)
{
    return a > b ? a : b;
}

static int ScaleForDpi(int value)
{
    return g_enableSystemScaling ? MulDiv(value, g_currentDpi, 96) : value;
}

static int CurrentWindowDpi(void)
{
    return g_enableSystemScaling && g_currentDpi > 0 ? g_currentDpi : 96;
}

static void BuildFallbackMessageLogFont(LOGFONT *log_font, int dpi)
{
    AppZeroMemory(log_font, sizeof(*log_font));
    log_font->lfHeight = -MulDiv(9, IsValidDpi(dpi) ? dpi : 96, 72);
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

#ifdef UNICODE
    if (IsValidDpi(dpi)) {
        SystemParametersInfoForDpiProc get_for_dpi =
            (SystemParametersInfoForDpiProc)AppGetProc("user32.dll", "SystemParametersInfoForDpi");
        if (get_for_dpi &&
            get_for_dpi(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0, (UINT)dpi)) {
            got_metrics = 1;
            got_dpi_metrics = 1;
        }
    }
#endif

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
        log_font->lfHeight = -MulDiv(9, IsValidDpi(dpi) ? dpi : 96, 72);
    }
    log_font->lfWeight = FW_BOLD;
    return 1;
}

static void DestroyMessageFont(void)
{
    if (g_messageFont) {
        DeleteObject(g_messageFont);
        g_messageFont = NULL;
    }
    g_messageFontDpi = 0;
}

static void RefreshMessageFont(void)
{
    int dpi = CurrentWindowDpi();
    LOGFONT log_font;
    HFONT font;

    if (!QueryCaptionLogFontForDpi(&log_font, dpi)) {
        BuildFallbackMessageLogFont(&log_font, dpi);
    }

    font = CreateFontIndirect(&log_font);
    if (!font) {
        return;
    }

    DestroyMessageFont();
    g_messageFont = font;
    g_messageFontDpi = dpi;
}

static HFONT MessageFont(void)
{
    HFONT font;
    int dpi = CurrentWindowDpi();

    if (!g_messageFont || g_messageFontDpi != dpi) {
        RefreshMessageFont();
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

static int AppStringContains(const APP_CHAR *text, const APP_CHAR *needle)
{
    if (!text || !needle || !needle[0]) {
        return 0;
    }

    for (; *text; ++text) {
        const APP_CHAR *scan = text;
        const APP_CHAR *match = needle;
        while (*scan && *match && *scan == *match) {
            ++scan;
            ++match;
        }
        if (!*match) {
            return 1;
        }
    }
    return 0;
}

static int AppStrLen(const APP_CHAR *text)
{
    int len = 0;
    while (text && text[len]) {
        ++len;
    }
    return len;
}

static void AppStrCopy(APP_CHAR *dst, int cch, const APP_CHAR *src)
{
    int i = 0;
    if (!dst || cch <= 0) {
        return;
    }
    if (src) {
        for (; i < cch - 1 && src[i]; ++i) {
            dst[i] = src[i];
        }
    }
    dst[i] = 0;
}

static void AppStrAppend(APP_CHAR *dst, int cch, const APP_CHAR *src)
{
    int len = AppStrLen(dst);
    if (len < cch) {
        AppStrCopy(dst + len, cch - len, src);
    }
}

static int AppFileExists(const APP_CHAR *path)
{
    DWORD attrs = APP_GET_FILE_ATTRIBUTES(path);
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static void AppEnsureTrailingSlash(APP_CHAR *path, int cch)
{
    int len = AppStrLen(path);
    if (len > 0 && len < cch - 1 &&
        path[len - 1] != APP_TEXT('\\') && path[len - 1] != APP_TEXT('/')) {
        path[len] = APP_TEXT('\\');
        path[len + 1] = 0;
    }
}

static int AppBuildModuleFilePath(APP_CHAR *out, int cch, const APP_CHAR *file_name)
{
    if (!APP_GET_MODULE_FILENAME(g_hinst, out, (DWORD)cch)) {
        return 0;
    }

    int slash = -1;
    for (int i = 0; out[i]; ++i) {
        if (out[i] == APP_TEXT('\\') || out[i] == APP_TEXT('/')) {
            slash = i;
        }
    }
    if (slash >= 0) {
        out[slash + 1] = 0;
    } else {
        out[0] = 0;
    }
    AppStrAppend(out, cch, file_name);
    return 1;
}

static int AppResolveLocalHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (!file_name[0]) {
        return 0;
    }
    if (AppBuildModuleFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    if (AppFileExists(file_name)) {
        AppStrCopy(out, cch, file_name);
        return 1;
    }
    return 0;
}

static int AppResolveWinHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (!file_name[0]) {
        return 0;
    }
    if (AppBuildModuleFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    AppStrCopy(out, cch, file_name);
    return 1;
}

static int AppResolveWindowsHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (!file_name[0]) {
        return 0;
    }
    AppStrCopy(out, cch, file_name);
    return 1;
}

static int AppIsVistaOrNewer(void)
{
    OSVERSIONINFOW version;
    return QueryWindowsVersion(&version) &&
        version.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        IsWindowsVersionAtLeast(&version, 6, 0);
}

static int AppCanPreferChmHelp(void)
{
    OSVERSIONINFOW version;
    if (!QueryWindowsVersion(&version)) {
        return 0;
    }

    if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        return IsWindowsVersionAtLeast(&version, 5, 0);
    }

    if (version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        return IsWindowsVersionAtLeast(&version, 4, 10);
    }

    return 0;
}

static HMODULE AppGetHtmlHelpModule(int allow_load)
{
    static int tried = 0;
    static HMODULE module = NULL;
    if (!allow_load && !tried) {
        return NULL;
    }
    if (!tried) {
        module = LoadLibraryA("hhctrl.ocx");
        tried = 1;
    }
    return module;
}

static BOOL AppHtmlHelp(HWND hwnd, const APP_CHAR *path, UINT command)
{
    HMODULE module = AppGetHtmlHelpModule(1);
    if (!module) {
        return FALSE;
    }

#ifdef UNICODE
    HtmlHelpWProc html_help = (HtmlHelpWProc)GetProcAddress(module, "HtmlHelpW");
    if (html_help) {
        return html_help(hwnd, path, command, 0) != NULL;
    }
#else
    if (g_useWideCommands) {
        HtmlHelpWProc html_help = (HtmlHelpWProc)GetProcAddress(module, "HtmlHelpW");
        if (html_help) {
            WCHAR wide_path[MAX_PATH];
            LPCWSTR use_path = NULL;
            if (path) {
                AppToWide(path, wide_path, MAX_PATH);
                use_path = wide_path;
            }
            return html_help(hwnd, use_path, command, 0) != NULL;
        }
    }

    HtmlHelpAProc html_help = (HtmlHelpAProc)GetProcAddress(module, "HtmlHelpA");
    if (html_help) {
        return html_help(hwnd, path, command, 0) != NULL;
    }
#endif
    return FALSE;
}

static void AppCloseHtmlHelp(void)
{
    HMODULE module = AppGetHtmlHelpModule(0);
    if (!module) {
        return;
    }

#ifdef UNICODE
    HtmlHelpWProc html_help = (HtmlHelpWProc)GetProcAddress(module, "HtmlHelpW");
    if (html_help) {
        html_help(NULL, NULL, HH_CLOSE_ALL_LOCAL, 0);
    }
#else
    if (g_useWideCommands) {
        HtmlHelpWProc html_help = (HtmlHelpWProc)GetProcAddress(module, "HtmlHelpW");
        if (html_help) {
            html_help(NULL, NULL, HH_CLOSE_ALL_LOCAL, 0);
            return;
        }
    }

    HtmlHelpAProc html_help = (HtmlHelpAProc)GetProcAddress(module, "HtmlHelpA");
    if (html_help) {
        html_help(NULL, NULL, HH_CLOSE_ALL_LOCAL, 0);
    }
#endif
}

static BOOL AppWinHelp(HWND hwnd, const APP_CHAR *path, UINT command, ULONG_PTR data)
{
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) {
        return FALSE;
    }

#ifdef UNICODE
    WinHelpWProc win_help = (WinHelpWProc)GetProcAddress(user32, "WinHelpW");
    if (win_help) {
        return win_help(hwnd, path, command, data);
    }
#else
    if (g_useWideCommands) {
        WinHelpWProc win_help = (WinHelpWProc)GetProcAddress(user32, "WinHelpW");
        if (win_help) {
            WCHAR wide_path[MAX_PATH];
            WCHAR wide_data[128];
            LPCWSTR use_path = NULL;
            ULONG_PTR use_data = data;
            if (path) {
                AppToWide(path, wide_path, MAX_PATH);
                use_path = wide_path;
            }
            if (command == HELP_PARTIALKEY && data) {
                AppToWide((const char *)data, wide_data, 128);
                use_data = (ULONG_PTR)wide_data;
            }
            return win_help(hwnd, use_path, command, use_data);
        }
    }

    WinHelpAProc win_help = (WinHelpAProc)GetProcAddress(user32, "WinHelpA");
    if (win_help) {
        return win_help(hwnd, path, command, data);
    }
#endif
    return FALSE;
}

static APP_NOINLINE int OpenLocalChmHelp(HWND hwnd, UINT command)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveLocalHelpFile(IDS_HELP_CHM_FILE, help_path, MAX_PATH) &&
        AppHtmlHelp(hwnd, help_path, command);
}

static APP_NOINLINE int OpenWindowsChmHelp(HWND hwnd, UINT command)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveWindowsHelpFile(IDS_WINDOWS_CHM_FILE, help_path, MAX_PATH) &&
        AppHtmlHelp(hwnd, help_path, command);
}

static APP_NOINLINE int OpenHelpViewerChmHelp(HWND hwnd)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveWindowsHelpFile(IDS_HELP_VIEWER_CHM_FILE, help_path, MAX_PATH) &&
        AppHtmlHelp(hwnd, help_path, HH_DISPLAY_TOPIC_LOCAL);
}

static APP_NOINLINE int OpenLocalWinHelp(HWND hwnd, UINT command, ULONG_PTR data)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveWinHelpFile(IDS_HELP_FILE, help_path, MAX_PATH) &&
        AppWinHelp(hwnd, help_path, command, data);
}

static void CloseLegacyWinHelp(HWND hwnd)
{
    APP_CHAR help_path[MAX_PATH];
    if (AppResolveWinHelpFile(IDS_HELP_FILE, help_path, MAX_PATH)) {
        AppWinHelp(hwnd, help_path, HELP_QUIT, 0);
    }
}

static LONG AppRegCreateSettingsKey(const APP_CHAR *subkey, HKEY *key)
{
#ifdef UNICODE
    return RegCreateKeyExW(
        HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, key, NULL);
#else
    if (g_useWideCommands) {
        RegCreateKeyExWProc create_key =
            (RegCreateKeyExWProc)AppGetProc("advapi32.dll", "RegCreateKeyExW");
        if (create_key) {
            WCHAR wide_subkey[256];
            AppToWide(subkey, wide_subkey, 256);
            return create_key(
                HKEY_CURRENT_USER, wide_subkey, 0, NULL,
                REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, key, NULL);
        }
    }
    return RegCreateKeyExA(
        HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, key, NULL);
#endif
}

static LONG AppRegOpenSettingsKey(const APP_CHAR *subkey, HKEY *key)
{
#ifdef UNICODE
    return RegOpenKeyExW(HKEY_CURRENT_USER, subkey, 0, KEY_READ, key);
#else
    if (g_useWideCommands) {
        RegOpenKeyExWProc open_key =
            (RegOpenKeyExWProc)AppGetProc("advapi32.dll", "RegOpenKeyExW");
        if (open_key) {
            WCHAR wide_subkey[256];
            AppToWide(subkey, wide_subkey, 256);
            return open_key(HKEY_CURRENT_USER, wide_subkey, 0, KEY_READ, key);
        }
    }
    return RegOpenKeyExA(HKEY_CURRENT_USER, subkey, 0, KEY_READ, key);
#endif
}

static LONG AppRegQueryDword(HKEY key, const APP_CHAR *value_name, DWORD *value)
{
    DWORD type = 0;
    DWORD cb = sizeof(DWORD);
    DWORD read_value = 0;
#ifdef UNICODE
    LONG result = RegQueryValueExW(key, value_name, NULL, &type, (LPBYTE)&read_value, &cb);
    if (result != ERROR_SUCCESS) {
        return result;
    }
    if (type != REG_DWORD || cb != sizeof(DWORD)) {
        return ERROR_INVALID_DATA;
    }
    *value = read_value;
    return ERROR_SUCCESS;
#else
    LONG result;
    if (g_useWideCommands) {
        RegQueryValueExWProc query_value =
            (RegQueryValueExWProc)AppGetProc("advapi32.dll", "RegQueryValueExW");
        if (query_value) {
            WCHAR wide_value[128];
            AppToWide(value_name, wide_value, 128);
            result = query_value(key, wide_value, NULL, &type, (LPBYTE)&read_value, &cb);
            if (result != ERROR_SUCCESS) {
                return result;
            }
            if (type != REG_DWORD || cb != sizeof(DWORD)) {
                return ERROR_INVALID_DATA;
            }
            *value = read_value;
            return ERROR_SUCCESS;
        }
    }

    result = RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&read_value, &cb);
    if (result != ERROR_SUCCESS) {
        return result;
    }
    if (type != REG_DWORD || cb != sizeof(DWORD)) {
        return ERROR_INVALID_DATA;
    }
    *value = read_value;
    return ERROR_SUCCESS;
#endif
}

static LONG AppRegSetDword(HKEY key, const APP_CHAR *value_name, DWORD value)
{
#ifdef UNICODE
    return RegSetValueExW(key, value_name, 0, REG_DWORD, (const BYTE *)&value, sizeof(value));
#else
    if (g_useWideCommands) {
        RegSetValueExWProc set_value =
            (RegSetValueExWProc)AppGetProc("advapi32.dll", "RegSetValueExW");
        if (set_value) {
            WCHAR wide_value[128];
            AppToWide(value_name, wide_value, 128);
            return set_value(key, wide_value, 0, REG_DWORD, (const BYTE *)&value, sizeof(value));
        }
    }
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&value, sizeof(value));
#endif
}

static int OpenSettingsKey(HKEY *key, int create)
{
    APP_CHAR subkey[256];
    LoadText(IDS_REGISTRY_KEY, subkey, 256);
    if (!subkey[0]) {
        return 0;
    }
    if (create) {
        return AppRegCreateSettingsKey(subkey, key) == ERROR_SUCCESS;
    }
    return AppRegOpenSettingsKey(subkey, key) == ERROR_SUCCESS;
}

enum {
    SETTINGS_VALUE_MISSING = 0,
    SETTINGS_VALUE_OK = 1,
    SETTINGS_VALUE_INVALID = 2
};

static int QuerySettingsDwordState(HKEY key, UINT name_id, DWORD *value)
{
    APP_CHAR value_name[128];
    LONG result;
    LoadText(name_id, value_name, 128);
    if (!value_name[0]) {
        return SETTINGS_VALUE_MISSING;
    }
    result = AppRegQueryDword(key, value_name, value);
    if (result == ERROR_SUCCESS) {
        return SETTINGS_VALUE_OK;
    }
    return result == ERROR_FILE_NOT_FOUND ? SETTINGS_VALUE_MISSING : SETTINGS_VALUE_INVALID;
}

static void SetSettingsDword(HKEY key, UINT name_id, DWORD value)
{
    APP_CHAR value_name[128];
    LoadText(name_id, value_name, 128);
    if (value_name[0]) {
        AppRegSetDword(key, value_name, value);
    }
}

static void ReloadMainMenu(HWND hwnd, int update_checks);

static int AppDarkModeOsSupported(void)
{
    OSVERSIONINFOW version;
    if (!QueryWindowsVersion(&version)) {
        return 0;
    }
    return version.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        IsWindowsVersionAtLeast(&version, 10, 0);
}

static int AppIsHighContrastOn(void)
{
    HIGHCONTRASTA high_contrast;
    AppZeroMemory(&high_contrast, sizeof(high_contrast));
    high_contrast.cbSize = sizeof(high_contrast);
    if (!SystemParametersInfoA(SPI_GETHIGHCONTRAST, sizeof(high_contrast), &high_contrast, 0)) {
        return 0;
    }
    return (high_contrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

static int QuerySystemDarkMode(void)
{
    HKEY key;
    DWORD apps_use_light_theme = 1;

    if (!g_darkModeSupported || AppIsHighContrastOn()) {
        return 0;
    }

    if (AppRegOpenSettingsKey(
            APP_TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
            &key) != ERROR_SUCCESS) {
        return 0;
    }

    if (AppRegQueryDword(key, APP_TEXT("AppsUseLightTheme"), &apps_use_light_theme) !=
        ERROR_SUCCESS) {
        apps_use_light_theme = 1;
    }
    RegCloseKey(key);
    return apps_use_light_theme == 0;
}

static void InitAppTheme(void)
{
    g_darkModeSupported = AppDarkModeOsSupported();
    g_darkModeEnabled = QuerySystemDarkMode();
}

static void ApplyDwmDarkMode(HWND hwnd)
{
    OSVERSIONINFOW version;
    DwmSetWindowAttributeProc set_window_attribute;
    BOOL enabled;
    HRESULT hr = E_FAIL;

    if (!hwnd || !g_darkModeSupported || !QueryWindowsVersion(&version) ||
        !IsWindowsBuildAtLeast(&version, 10, 0, 17763)) {
        return;
    }

    set_window_attribute =
        (DwmSetWindowAttributeProc)AppGetProc("dwmapi.dll", "DwmSetWindowAttribute");
    if (!set_window_attribute) {
        return;
    }

    enabled = g_darkModeEnabled ? TRUE : FALSE;
    if (IsWindowsBuildAtLeast(&version, 10, 0, 18362)) {
        hr = set_window_attribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL,
            &enabled,
            sizeof(enabled));
    }
    if (FAILED(hr)) {
        set_window_attribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE_OLD,
            &enabled,
            sizeof(enabled));
    }
}

static void ApplyUxThemeDarkMode(HWND hwnd)
{
    OSVERSIONINFOW version;
    HMODULE uxtheme;
    FARPROC preferred_mode;
    AllowDarkModeForWindowProc allow_window;
    FlushMenuThemesProc flush_menus;

    if (!g_darkModeSupported || !QueryWindowsVersion(&version) ||
        !IsWindowsBuildAtLeast(&version, 10, 0, 17763)) {
        return;
    }

    uxtheme = LoadLibraryA("uxtheme.dll");
    if (!uxtheme) {
        return;
    }

    preferred_mode = GetProcAddress(uxtheme, MAKEINTRESOURCEA(135));
    if (preferred_mode) {
        if (IsWindowsBuildAtLeast(&version, 10, 0, 18362)) {
            ((SetPreferredAppModeProc)preferred_mode)(APP_PREFERRED_APP_MODE_ALLOW_DARK);
        } else {
            ((AllowDarkModeForAppProc)preferred_mode)(TRUE);
        }
    }

    allow_window = (AllowDarkModeForWindowProc)GetProcAddress(uxtheme, MAKEINTRESOURCEA(133));
    if (allow_window && hwnd) {
        allow_window(hwnd, g_darkModeEnabled ? TRUE : FALSE);
    }

    flush_menus = (FlushMenuThemesProc)GetProcAddress(uxtheme, MAKEINTRESOURCEA(136));
    if (flush_menus) {
        flush_menus();
    }
    FreeLibrary(uxtheme);
}

static void ApplyDarkThemeToWindow(HWND hwnd)
{
    OSVERSIONINFOW version;
    HMODULE uxtheme;
    AllowDarkModeForWindowProc allow_window;
    SetWindowThemeProc set_window_theme;

    if (!hwnd || !g_darkModeSupported || !QueryWindowsVersion(&version) ||
        !IsWindowsBuildAtLeast(&version, 10, 0, 17763)) {
        return;
    }

    uxtheme = LoadLibraryA("uxtheme.dll");
    if (!uxtheme) {
        return;
    }

    allow_window = (AllowDarkModeForWindowProc)GetProcAddress(uxtheme, MAKEINTRESOURCEA(133));
    if (allow_window) {
        allow_window(hwnd, g_darkModeEnabled ? TRUE : FALSE);
    }

    set_window_theme = (SetWindowThemeProc)GetProcAddress(uxtheme, "SetWindowTheme");
    if (set_window_theme) {
        if (g_darkModeEnabled) {
            set_window_theme(hwnd, L"DarkMode_Explorer", NULL);
        } else {
            set_window_theme(hwnd, NULL, NULL);
        }
    }
    FreeLibrary(uxtheme);
}

static void ApplyAppThemeToWindow(HWND hwnd)
{
    ApplyUxThemeDarkMode(hwnd);
    ApplyDwmDarkMode(hwnd);
    if (hwnd) {
        DrawMenuBar(hwnd);
        SetWindowPos(
            hwnd,
            NULL,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }
}

static void RefreshAppTheme(HWND hwnd)
{
    g_darkModeSupported = AppDarkModeOsSupported();
    g_darkModeEnabled = QuerySystemDarkMode();
    RefreshClassicColors();
    if (hwnd && hwnd == g_mainWindow) {
        ReloadMainMenu(hwnd, 1);
    }
    ApplyAppThemeToWindow(hwnd);
}

static int SkillDepthForCommand(int cmd)
{
    switch (cmd) {
    case IDM_BEGINNER:
        return 1;
    case IDM_EXPERT:
        return 4;
    case IDM_MASTER:
        return 6;
    case IDM_INTERMEDIATE:
    default:
        return 2;
    }
}

static int NormalizeSkillCommand(int cmd)
{
    switch (cmd) {
    case IDM_BEGINNER:
    case IDM_INTERMEDIATE:
    case IDM_EXPERT:
    case IDM_MASTER:
        return cmd;
    default:
        return IDM_INTERMEDIATE;
    }
}

static int NormalizeAnimationCommand(int cmd)
{
    switch (cmd) {
    case IDM_ANIM_NONE:
    case IDM_ANIM_SLOW:
    case IDM_ANIM_FAST:
        return cmd;
    default:
        return IDM_ANIM_FAST;
    }
}

static int SkillCommandToSetting(int cmd)
{
    switch (NormalizeSkillCommand(cmd)) {
    case IDM_BEGINNER:
        return 0;
    case IDM_INTERMEDIATE:
        return 1;
    case IDM_EXPERT:
        return 2;
    case IDM_MASTER:
        return 3;
    default:
        return 1;
    }
}

static int IsSkillSettingValid(int value)
{
    return value >= 0 && value <= 3;
}

static int SkillSettingToCommand(int value)
{
    switch (value) {
    case 0:
        return IDM_BEGINNER;
    case 1:
        return IDM_INTERMEDIATE;
    case 2:
        return IDM_EXPERT;
    case 3:
        return IDM_MASTER;
    default:
        return IDM_INTERMEDIATE;
    }
}

static int AnimationCommandToSetting(int cmd)
{
    switch (NormalizeAnimationCommand(cmd)) {
    case IDM_ANIM_NONE:
        return 0;
    case IDM_ANIM_SLOW:
        return 1;
    case IDM_ANIM_FAST:
        return 2;
    default:
        return 2;
    }
}

static int IsAnimationSettingValid(int value)
{
    return value >= 0 && value <= 2;
}

static int AnimationSettingToCommand(int value)
{
    switch (value) {
    case 0:
        return IDM_ANIM_NONE;
    case 1:
        return IDM_ANIM_SLOW;
    case 2:
        return IDM_ANIM_FAST;
    default:
        return IDM_ANIM_FAST;
    }
}

static void GetDesktopBounds(RECT *desktop)
{
    int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (width <= 0 || height <= 0) {
        left = 0;
        top = 0;
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
    }

    desktop->left = left;
    desktop->top = top;
    desktop->right = left + width;
    desktop->bottom = top + height;
}

static int IsSavedWindowRectUsable(const RECT *rect)
{
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;
    RECT desktop;
    GetDesktopBounds(&desktop);

    if (width < ScaleForDpi(MIN_TRACK_SIZE) || height < ScaleForDpi(MIN_TRACK_SIZE)) {
        return 0;
    }
    if (desktop.right <= desktop.left || desktop.bottom <= desktop.top) {
        return 0;
    }

    return rect->left >= desktop.left &&
        rect->top >= desktop.top &&
        rect->right <= desktop.right &&
        rect->bottom <= desktop.bottom;
}

static int BuildSavedWindowRect(
    DWORD x,
    DWORD y,
    DWORD width,
    DWORD height,
    DWORD saved_dpi,
    RECT *rect)
{
    LONG left = WindowValueFromRegistry(x, saved_dpi);
    LONG top = WindowValueFromRegistry(y, saved_dpi);
    LONG scaled_width = WindowValueFromRegistry(width, saved_dpi);
    LONG scaled_height = WindowValueFromRegistry(height, saved_dpi);
    LONGLONG right = (LONGLONG)left + scaled_width;
    LONGLONG bottom = (LONGLONG)top + scaled_height;

    if (scaled_width <= 0 || scaled_height <= 0 ||
        right < LONG_MIN || right > LONG_MAX ||
        bottom < LONG_MIN || bottom > LONG_MAX) {
        return 0;
    }

    rect->left = left;
    rect->top = top;
    rect->right = (LONG)right;
    rect->bottom = (LONG)bottom;
    return 1;
}

static void LoadGameSettings(void)
{
    DWORD value;
    DWORD x;
    DWORD y;
    DWORD width;
    DWORD height;
    DWORD dpi;
    int state;
    int x_state;
    int y_state;
    int width_state;
    int height_state;
    int dpi_state;
    HKEY key;

    g_configSkillCmd = IDM_INTERMEDIATE;
    g_configAnimationCmd = IDM_ANIM_FAST;
    g_configHasWindowRect = 0;
    g_configRepairSettings = 0;

    if (!OpenSettingsKey(&key, 0)) {
        return;
    }

    state = QuerySettingsDwordState(key, IDS_SKILL_KEY, &value);
    if (state == SETTINGS_VALUE_OK) {
        int setting = (int)(LONG)value;
        if (IsSkillSettingValid(setting)) {
            g_configSkillCmd = SkillSettingToCommand(setting);
        } else {
            g_configRepairSettings = 1;
        }
    } else {
        g_configRepairSettings = 1;
    }
    state = QuerySettingsDwordState(key, IDS_ANIMATION_KEY, &value);
    if (state == SETTINGS_VALUE_OK) {
        int setting = (int)(LONG)value;
        if (IsAnimationSettingValid(setting)) {
            g_configAnimationCmd = AnimationSettingToCommand(setting);
        } else {
            g_configRepairSettings = 1;
        }
    } else {
        g_configRepairSettings = 1;
    }

    x_state = QuerySettingsDwordState(key, IDS_WINDOW_X_KEY, &x);
    y_state = QuerySettingsDwordState(key, IDS_WINDOW_Y_KEY, &y);
    width_state = QuerySettingsDwordState(key, IDS_WINDOW_WIDTH_KEY, &width);
    height_state = QuerySettingsDwordState(key, IDS_WINDOW_HEIGHT_KEY, &height);
    dpi_state = QuerySettingsDwordState(key, IDS_WINDOW_DPI_KEY, &dpi);

    if (x_state == SETTINGS_VALUE_OK &&
        y_state == SETTINGS_VALUE_OK &&
        width_state == SETTINGS_VALUE_OK &&
        height_state == SETTINGS_VALUE_OK) {
        DWORD saved_dpi = 0;
        int window_invalid = 0;
        RECT saved;
        if (dpi_state == SETTINGS_VALUE_OK) {
            if (IsValidDpi((int)dpi)) {
                saved_dpi = dpi;
            } else {
                window_invalid = 1;
            }
        } else {
            if (dpi_state == SETTINGS_VALUE_INVALID) {
                window_invalid = 1;
            }
            g_configRepairSettings = 1;
        }
        if (!window_invalid) {
            if (BuildSavedWindowRect(x, y, width, height, saved_dpi, &saved) &&
                IsSavedWindowRectUsable(&saved)) {
                g_configWindowRect = saved;
                g_configHasWindowRect = 1;
            } else {
                window_invalid = 1;
            }
        }
        if (window_invalid) {
            g_configRepairSettings = 1;
        }
    } else if (x_state != SETTINGS_VALUE_MISSING ||
        y_state != SETTINGS_VALUE_MISSING ||
        width_state != SETTINGS_VALUE_MISSING ||
        height_state != SETTINGS_VALUE_MISSING ||
        dpi_state != SETTINGS_VALUE_MISSING) {
        g_configRepairSettings = 1;
    } else {
        g_configRepairSettings = 1;
    }

    RegCloseKey(key);
}

static int GetWindowRectToSave(HWND hwnd, RECT *rect)
{
    WINDOWPLACEMENT placement;
    AppZeroMemory(&placement, sizeof(placement));
    placement.length = sizeof(placement);
    if (GetWindowPlacement(hwnd, &placement)) {
        if (placement.showCmd == SW_SHOWMINIMIZED || placement.showCmd == SW_SHOWMAXIMIZED) {
            *rect = placement.rcNormalPosition;
            return 1;
        }
    }
    return GetWindowRect(hwnd, rect);
}

static void SaveGameSettings(HWND hwnd)
{
    HKEY key;
    if (!OpenSettingsKey(&key, 1)) {
        return;
    }

    SetSettingsDword(key, IDS_SKILL_KEY, (DWORD)SkillCommandToSetting(g_game.skill_cmd));
    SetSettingsDword(key, IDS_ANIMATION_KEY, (DWORD)AnimationCommandToSetting(g_game.animation_cmd));

    if (hwnd && IsWindow(hwnd)) {
        RECT rect;
        if (GetWindowRectToSave(hwnd, &rect)) {
            SetSettingsDword(key, IDS_WINDOW_X_KEY, (DWORD)WindowValueToRegistry(rect.left));
            SetSettingsDword(key, IDS_WINDOW_Y_KEY, (DWORD)WindowValueToRegistry(rect.top));
            SetSettingsDword(key, IDS_WINDOW_WIDTH_KEY, (DWORD)WindowValueToRegistry(rect.right - rect.left));
            SetSettingsDword(key, IDS_WINDOW_HEIGHT_KEY, (DWORD)WindowValueToRegistry(rect.bottom - rect.top));
            SetSettingsDword(key, IDS_WINDOW_DPI_KEY, 96);
        }
    }

    RegCloseKey(key);
}

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

static void CalculateLayout(HWND hwnd, Layout *layout)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int text_h = 0;
    HDC hdc = GetDC(hwnd);
    if (hdc) {
        TEXTMETRIC tm;
        HFONT font = MessageFont();
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

    int calc_width = width;
    int calc_height = height;
    int cell_w;
    int cell_h;

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

    int board_w = cell_w * BOARD_N;
    int board_h = cell_h * BOARD_N;
    int depth_y = board_h / 30;
    int depth_x = g_deviceAspect * depth_y;

    layout->cell = cell_w;
    layout->cell_w = cell_w;
    layout->cell_h = cell_h;
    layout->message.left = 0;
    layout->message.top = 0;
    layout->message.right = width;
    layout->message.bottom = text_h;
    layout->depth_x = depth_x;
    layout->depth_y = depth_y;
    layout->board.left = width > board_w ? (width - board_w) / 2 : 0;
    layout->board.top = height > board_h ? (height - board_h) / 2 : 0;
    layout->board.right = layout->board.left + board_w;
    layout->board.bottom = layout->board.top + board_h;
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
    EnableMenuItem(menu, MENU_POS_TEST, MF_BYPOSITION | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_NEW, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_EXIT, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_PASS, MF_BYCOMMAND | (busy ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HINT, MF_BYCOMMAND | (busy || g_game.game_over ? MF_GRAYED : MF_ENABLED));
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

static int IsTestCommand(UINT cmd)
{
    return cmd >= IDM_TEST_PLAYER_PASS && cmd <= IDM_TEST_LOSE;
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

    SelectObject(hdc, old_pen);
    DeleteObject(gray);
    DeleteObject(white);
    DeleteObject(black);
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

static int DrawPieceGdiPlus(HDC hdc, const RECT *white_rc, const RECT *gray_rc,
    const RECT *color_rc, COLORREF main_color)
{
    GpGraphics *graphics = NULL;

    if (!g_gdiPlusReady || g_monoDisplay) {
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

static void DrawPiece(HDC hdc, RECT cell, int piece, const AnimCell *anim)
{
    int draw_piece = piece;
    int shrink_x = 0;
    if (anim && anim->active) {
        if (anim->step <= 2) {
            draw_piece = anim->from;
            shrink_x = (anim->step + 1) * (cell.right - cell.left) / 8;
        } else {
            draw_piece = anim->to;
            shrink_x = (6 - anim->step) * (cell.right - cell.left) / 8;
        }
    }

    if (draw_piece == EMPTY) {
        return;
    }

    COLORREF main_color = draw_piece == RED ? g_redColor : g_blueColor;
    RECT white_rc = cell;
    RECT gray_rc = cell;
    RECT color_rc = cell;
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

static void DrawHintPiece(HDC hdc, RECT rc)
{
    DrawPiece(hdc, rc, RED, NULL);
}

static void DrawMessageLine(HWND hwnd, HDC hdc, const Layout *layout)
{
    HBRUSH face = CreateSolidBrush(g_messageBackColor);
    FillRect(hdc, &layout->message, face);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_messageTextColor);

    HFONT font = MessageFont();
    HGDIOBJ old_font = font ? SelectObject(hdc, font) : NULL;
    int len = APP_LSTRLEN(g_game.message);
    SIZE text_size = {0, 0};
    APP_GET_TEXT_EXTENT(hdc, g_game.message, len, &text_size);
    int x = layout->message.left + ((layout->message.right - layout->message.left) - text_size.cx) / 2;
    APP_TEXT_OUT(hdc, x, layout->message.top, g_game.message, len);
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

    for (int r = 0; r < BOARD_N; ++r) {
        for (int c = 0; c < BOARD_N; ++c) {
            RECT cell = CellRect(&layout, r, c);
            Draw3DCell(hdc, cell);

            AnimCell local_anim;
            AnimCell *anim = NULL;
            if (g_anim.active && g_anim.row == r && g_anim.col == c) {
                local_anim = g_anim;
                anim = &local_anim;
            }
            if (g_placeFlash.active && g_placeFlash.row == r && g_placeFlash.col == c) {
                DrawPiece(hdc, cell, g_placeFlash.visible ? g_placeFlash.piece : EMPTY, NULL);
            } else {
                DrawPiece(hdc, cell, g_game.cells[r][c], anim);
            }

        }
    }

    RECT bottom_shadow = {
        layout.board.left + depth_x * 2,
        layout.board.bottom,
        layout.board.right + depth_x * 2,
        layout.board.bottom + depth_y * 2
    };
    RECT side_shadow = {
        layout.board.right,
        layout.board.top + depth_y * 2,
        layout.board.right + depth_x * 2,
        layout.board.bottom + depth_y * 2
    };
    HBRUSH black = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hdc, &bottom_shadow, black);
    FillRect(hdc, &side_shadow, black);

    HBRUSH slope = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    for (int i = 1; i < depth_x; ++i) {
        RECT side_strip = {
            layout.board.right + i,
            layout.board.top + i / g_deviceAspect,
            layout.board.right + i + 1,
            layout.board.bottom + i / g_deviceAspect
        };
        FillRect(hdc, &side_strip, slope);
    }
    for (int i = 1; i < depth_y; ++i) {
        RECT bottom_strip = {
            layout.board.left + g_deviceAspect * i,
            layout.board.bottom + i,
            layout.board.right + g_deviceAspect * i,
            layout.board.bottom + i + 1
        };
        FillRect(hdc, &bottom_strip, slope);
    }

    HPEN edge = CreatePen(PS_SOLID, 1, g_edgeDarkColor);
    HGDIOBJ old_pen = SelectObject(hdc, edge);
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

    SelectObject(hdc, old_pen);
    DeleteObject(edge);

    if (g_game.message[0]) {
        DrawMessageLine(hwnd, hdc, &layout);
    }
}

static void DrawSingleCell(HWND hwnd, HDC hdc, int row, int col, int hint)
{
    Layout layout;
    RECT cell;

    CalculateLayout(hwnd, &layout);
    cell = CellRect(&layout, row, col);
    Draw3DCell(hdc, cell);
    DrawPiece(hdc, cell, g_game.cells[row][col], NULL);
    if (hint && g_game.cells[row][col] == EMPTY) {
        DrawHintPiece(hdc, cell);
    }
}

static void FlashHintCell(HWND hwnd, int row, int col)
{
    HDC hdc = GetDC(hwnd);
    HCURSOR old_cursor;

    if (!hdc) {
        return;
    }

    g_hintBlankCursor = 1;
    old_cursor = SetCursor(NULL);
    for (int i = 0; i < 4; ++i) {
        DrawSingleCell(hwnd, hdc, row, col, (i & 1) == 0);
        PumpFor(150);
    }
    g_hintBlankCursor = 0;
    SetCursor(old_cursor);
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

static void PlayerMove(HWND hwnd, int row, int col)
{
    if (g_game.game_over || IsGameBusy() || g_game.turn != RED || g_game.must_pass) {
        MessageBeep(MB_ICONWARNING);
        return;
    }

    if (!ApplyMove(hwnd, row, col, RED, 1)) {
        MessageBeep(MB_ICONWARNING);
        if (!g_game.invalid_tip_shown) {
            APP_CHAR message[192];
            APP_CHAR title[64];
            LoadText(IDS_INVALID_MOVE, message, 192);
            LoadText(IDS_APP_TITLE, title, 64);
            APP_MESSAGE_BOX(hwnd, message, title, MB_OK | MB_ICONINFORMATION);
            g_game.invalid_tip_shown = 1;
        }
        return;
    }

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

static void HandleClick(HWND hwnd, LPARAM lparam)
{
    int row;
    int col;
    if (PointToCell(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), &row, &col)) {
        g_game.selected_row = row;
        g_game.selected_col = col;
        PlayerMove(hwnd, row, col);
    }
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
    LoadText(text_id, text, 512);
    LoadText(IDS_HELP_TITLE, title, 64);
    APP_MESSAGE_BOX(hwnd, text, title, MB_OK | MB_ICONINFORMATION);
}

static void MoveSelection(HWND hwnd, int dr, int dc)
{
    if (g_game.game_over || IsGameBusy() || g_game.turn != RED || g_game.must_pass) {
        return;
    }

    int row = g_game.selected_row;
    int col = g_game.selected_col;
    for (int i = 0; i < BOARD_N * BOARD_N; ++i) {
        row = (row + dr + BOARD_N) % BOARD_N;
        col = (col + dc + BOARD_N) % BOARD_N;
        if (CollectFlips(row, col, RED, NULL, 0) > 0) {
            g_game.selected_row = row;
            g_game.selected_col = col;
            Layout layout;
            CalculateLayout(hwnd, &layout);
            RECT rc = CellRect(&layout, row, col);
            POINT pt;
            pt.x = rc.left + layout.cell_w / 2;
            pt.y = rc.top + layout.cell_h / 2;
            ClientToScreen(hwnd, &pt);
            SetCursorPos(pt.x, pt.y);
            return;
        }
    }
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
        PlayerMove(hwnd, g_game.selected_row, g_game.selected_col);
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
        RefreshMessageFont();
        InitGameState();
        SetTitle(hwnd);
        UpdateMenuChecks(hwnd);
        return 0;

    case WM_ERASEBKGND: {
        RECT client;
        GetClientRect(hwnd, &client);
        FillClassicFace((HDC)wparam, &client, g_windowGray, 0);
        return 1;
    }

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

    case WM_MOUSEMOVE: {
        int row;
        int col;
        if (g_hintBlankCursor) {
            SetCursor(NULL);
        } else if (IsGameBusy()) {
            SetCursor(APP_LOAD_CURSOR(NULL, IDC_WAIT));
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

    case WM_DPICHANGED: {
        RECT *suggested = (RECT *)lparam;
        int dpi = (int)LOWORD(wparam);
        if (!IsValidDpi(dpi)) {
            dpi = (int)HIWORD(wparam);
        }
        g_currentDpi = NormalizeDpi(dpi);
        UpdateWindowIcons(hwnd);
        RefreshMessageFont();
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
            UpdateWindowIcons(hwnd);
            RefreshMessageFont();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_WINDOWPOSCHANGED:
        if (g_perMonitorDpiAware && UpdateWindowDpi(hwnd)) {
            UpdateWindowIcons(hwnd);
            RefreshMessageFont();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_DISPLAYCHANGE:
        if (g_perMonitorDpiAware) {
            if (UpdateWindowDpi(hwnd)) {
                UpdateWindowIcons(hwnd);
                RefreshMessageFont();
            }
        } else {
            g_currentDpi = QuerySystemDpi();
            UpdateWindowIcons(hwnd);
            RefreshMessageFont();
        }
        RefreshAppTheme(hwnd);
        return 0;

    case WM_SETTINGCHANGE:
        if (!g_perMonitorDpiAware) {
            g_currentDpi = QuerySystemDpi();
            UpdateWindowIcons(hwnd);
        }
        RefreshMessageFont();
        RefreshAppTheme(hwnd);
        return 0;

    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
        RefreshMessageFont();
        RefreshAppTheme(hwnd);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawGame(hwnd, hdc);
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
        DestroyMessageFont();
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
