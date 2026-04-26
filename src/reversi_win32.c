#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include "resource.h"

#define BOARD_N 8
#define EMPTY 0
#define RED 1
#define BLUE 2

#define TIMER_FLASH 0x29A
#define PROCESS_SYSTEM_DPI_AWARE_LOCAL 1
#define HH_DISPLAY_TOPIC_LOCAL 0x0000
#define HH_DISPLAY_TOC_LOCAL 0x0001
#define HH_DISPLAY_INDEX_LOCAL 0x0002
#define HH_CLOSE_ALL_LOCAL 0x0012

#ifndef DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE ((HANDLE)-2)
#endif
#ifndef SM_XVIRTUALSCREEN
#define SM_XVIRTUALSCREEN 76
#define SM_YVIRTUALSCREEN 77
#define SM_CXVIRTUALSCREEN 78
#define SM_CYVIRTUALSCREEN 79
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
    int hint_row;
    int hint_col;
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
static int g_deviceAspect = 1;
static int g_systemDpi = 96;
static int g_enableSystemScaling = 0;
static int g_useWideCommands = 0;
static int g_cpuHasSse2 = 0;
static int g_cpuHasAvx = 0;
static int g_configSkillCmd = IDM_INTERMEDIATE;
static int g_configAnimationCmd = IDM_ANIM_FAST;
static int g_configHasWindowRect = 0;
static RECT g_configWindowRect;
static ModernFindBestMoveProc g_modernFindBestMove = NULL;
static HANDLE g_visualStylesActCtx = INVALID_HANDLE_VALUE;
static ULONG_PTR g_visualStylesCookie = 0;

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
typedef HANDLE (WINAPI *CreateActCtxProc)(const APP_ACTCTX *);
typedef BOOL (WINAPI *ActivateActCtxProc)(HANDLE, ULONG_PTR *);
typedef BOOL (WINAPI *DeactivateActCtxProc)(DWORD, ULONG_PTR);
typedef VOID (WINAPI *ReleaseActCtxProc)(HANDLE);
typedef BOOL (WINAPI *CheckMenuRadioItemProc)(HMENU, UINT, UINT, UINT, UINT);
typedef BOOL (WINAPI *WinHelpAProc)(HWND, LPCSTR, UINT, ULONG_PTR);
typedef BOOL (WINAPI *WinHelpWProc)(HWND, LPCWSTR, UINT, ULONG_PTR);
typedef HWND (WINAPI *HtmlHelpAProc)(HWND, LPCSTR, UINT, DWORD_PTR);
typedef HWND (WINAPI *HtmlHelpWProc)(HWND, LPCWSTR, UINT, DWORD_PTR);

static void AppZeroMemory(void *ptr, UINT_PTR size)
{
    BYTE *bytes = (BYTE *)ptr;
    while (size--) {
        *bytes++ = 0;
    }
}

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

static FARPROC AppGetProc(const char *module_name, const char *proc_name)
{
    HMODULE module = GetModuleHandleA(module_name);
    if (!module) {
        module = LoadLibraryA(module_name);
    }
    return module ? GetProcAddress(module, proc_name) : NULL;
}

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
    if (!QueryWindowsVersion(&version)) {
        g_enableSystemScaling = 0;
        return;
    }

    if (!IsWindowsVersionAtLeast(&version, 6, 0)) {
        g_enableSystemScaling = 0;
        return;
    }

    g_enableSystemScaling = 1;

    if (IsWindowsVersionAtLeast(&version, 10, 0)) {
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            SetProcessDpiAwarenessContextProc set_context =
                (SetProcessDpiAwarenessContextProc)GetProcAddress(
                    user32, "SetProcessDpiAwarenessContext");
            if (set_context && set_context(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)) {
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
                HRESULT hr = set_awareness(PROCESS_SYSTEM_DPI_AWARE_LOCAL);
                FreeLibrary(shcore);
                if (SUCCEEDED(hr) || hr == E_ACCESSDENIED) {
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
            if (set_dpi_aware) {
                set_dpi_aware();
            }
        }
    }
}

static void InitClassicColors(void)
{
    HDC hdc = GetDC(NULL);
    if (hdc) {
        if (g_enableSystemScaling) {
            g_systemDpi = GetDeviceCaps(hdc, LOGPIXELSX);
        }
        if (g_systemDpi <= 0 || !g_enableSystemScaling) {
            g_systemDpi = 96;
        }
        g_deviceAspect = GetDeviceCaps(hdc, VERTRES) == 200 ? 2 : 1;
        g_redColor = GetNearestColor(hdc, RGB(255, 0, 0));
        g_blueColor = GetNearestColor(hdc, RGB(0, 0, 255));
        g_windowGray = GetNearestColor(hdc, RGB(170, 170, 170));
        g_cellGray = GetNearestColor(hdc, RGB(192, 192, 192));
        g_darkGray = GetNearestColor(hdc, RGB(85, 85, 85));
        ReleaseDC(NULL, hdc);
    } else {
        g_systemDpi = 96;
        g_deviceAspect = 1;
        g_redColor = RGB(255, 0, 0);
        g_blueColor = RGB(0, 0, 255);
        g_windowGray = RGB(170, 170, 170);
        g_cellGray = RGB(192, 192, 192);
        g_darkGray = RGB(85, 85, 85);
    }
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
    return g_enableSystemScaling ? MulDiv(value, g_systemDpi, 96) : value;
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

static int AppBuildWindowsHelpPath(APP_CHAR *out, int cch, const APP_CHAR *file_name)
{
    UINT len = APP_GET_WINDOWS_DIRECTORY(out, (UINT)cch);
    if (!len || len >= (UINT)cch) {
        return 0;
    }
    AppEnsureTrailingSlash(out, cch);
    AppStrAppend(out, cch, APP_TEXT("Help\\"));
    AppStrAppend(out, cch, file_name);
    return 1;
}

static int AppResolveLocalHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (AppBuildModuleFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    AppStrCopy(out, cch, file_name);
    return AppFileExists(out);
}

static int AppResolveWindowsHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (AppBuildWindowsHelpPath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    AppStrCopy(out, cch, file_name);
    return AppFileExists(out);
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
            AppToWide(path, wide_path, MAX_PATH);
            return html_help(hwnd, wide_path, command, 0) != NULL;
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

static int OpenLocalChmHelp(HWND hwnd, UINT command)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveLocalHelpFile(IDS_HELP_CHM_FILE, help_path, MAX_PATH) &&
        AppHtmlHelp(hwnd, help_path, command);
}

static int OpenWindowsChmHelp(HWND hwnd, UINT command)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveWindowsHelpFile(IDS_WINDOWS_CHM_FILE, help_path, MAX_PATH) &&
        AppHtmlHelp(hwnd, help_path, command);
}

static int OpenLocalWinHelp(HWND hwnd, UINT command, ULONG_PTR data)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveLocalHelpFile(IDS_HELP_FILE, help_path, MAX_PATH) &&
        AppWinHelp(hwnd, help_path, command, data);
}

static int OpenWindowsWinHelp(HWND hwnd)
{
    APP_CHAR help_path[MAX_PATH];
    return AppResolveWindowsHelpFile(IDS_WINDOWS_HELP_FILE, help_path, MAX_PATH) &&
        AppWinHelp(hwnd, help_path, HELP_CONTENTS, 0);
}

static void CloseLegacyWinHelp(HWND hwnd)
{
    APP_CHAR help_path[MAX_PATH];
    if (AppResolveLocalHelpFile(IDS_HELP_FILE, help_path, MAX_PATH)) {
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
    DWORD type = REG_DWORD;
    DWORD cb = sizeof(DWORD);
#ifdef UNICODE
    return RegQueryValueExW(key, value_name, NULL, &type, (LPBYTE)value, &cb) == ERROR_SUCCESS &&
        type == REG_DWORD && cb == sizeof(DWORD) ? ERROR_SUCCESS : ERROR_FILE_NOT_FOUND;
#else
    LONG result;
    if (g_useWideCommands) {
        RegQueryValueExWProc query_value =
            (RegQueryValueExWProc)AppGetProc("advapi32.dll", "RegQueryValueExW");
        if (query_value) {
            WCHAR wide_value[128];
            AppToWide(value_name, wide_value, 128);
            result = query_value(key, wide_value, NULL, &type, (LPBYTE)value, &cb);
            return result == ERROR_SUCCESS && type == REG_DWORD && cb == sizeof(DWORD) ?
                ERROR_SUCCESS : ERROR_FILE_NOT_FOUND;
        }
    }

    result = RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)value, &cb);
    return result == ERROR_SUCCESS && type == REG_DWORD && cb == sizeof(DWORD) ?
        ERROR_SUCCESS : ERROR_FILE_NOT_FOUND;
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

static int QuerySettingsDword(HKEY key, UINT name_id, DWORD *value)
{
    APP_CHAR value_name[128];
    LoadText(name_id, value_name, 128);
    return value_name[0] && AppRegQueryDword(key, value_name, value) == ERROR_SUCCESS;
}

static void SetSettingsDword(HKEY key, UINT name_id, DWORD value)
{
    APP_CHAR value_name[128];
    LoadText(name_id, value_name, 128);
    if (value_name[0]) {
        AppRegSetDword(key, value_name, value);
    }
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

    if (width < ScaleForDpi(220) || height < ScaleForDpi(220)) {
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

static void LoadGameSettings(void)
{
    DWORD value;
    DWORD x;
    DWORD y;
    DWORD width;
    DWORD height;
    HKEY key;

    g_configSkillCmd = IDM_INTERMEDIATE;
    g_configAnimationCmd = IDM_ANIM_FAST;
    g_configHasWindowRect = 0;

    if (!OpenSettingsKey(&key, 0)) {
        return;
    }

    if (QuerySettingsDword(key, IDS_SKILL_KEY, &value)) {
        g_configSkillCmd = NormalizeSkillCommand((int)(LONG)value);
    }
    if (QuerySettingsDword(key, IDS_ANIMATION_KEY, &value)) {
        g_configAnimationCmd = NormalizeAnimationCommand((int)(LONG)value);
    }

    if (QuerySettingsDword(key, IDS_WINDOW_X_KEY, &x) &&
        QuerySettingsDword(key, IDS_WINDOW_Y_KEY, &y) &&
        QuerySettingsDword(key, IDS_WINDOW_WIDTH_KEY, &width) &&
        QuerySettingsDword(key, IDS_WINDOW_HEIGHT_KEY, &height)) {
        RECT saved;
        saved.left = (LONG)x;
        saved.top = (LONG)y;
        saved.right = saved.left + (LONG)width;
        saved.bottom = saved.top + (LONG)height;
        if (IsSavedWindowRectUsable(&saved)) {
            g_configWindowRect = saved;
            g_configHasWindowRect = 1;
        }
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

    SetSettingsDword(key, IDS_SKILL_KEY, (DWORD)g_game.skill_cmd);
    SetSettingsDword(key, IDS_ANIMATION_KEY, (DWORD)g_game.animation_cmd);

    if (hwnd && IsWindow(hwnd)) {
        RECT rect;
        if (GetWindowRectToSave(hwnd, &rect)) {
            SetSettingsDword(key, IDS_WINDOW_X_KEY, (DWORD)rect.left);
            SetSettingsDword(key, IDS_WINDOW_Y_KEY, (DWORD)rect.top);
            SetSettingsDword(key, IDS_WINDOW_WIDTH_KEY, (DWORD)(rect.right - rect.left));
            SetSettingsDword(key, IDS_WINDOW_HEIGHT_KEY, (DWORD)(rect.bottom - rect.top));
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
        if (APP_GET_TEXT_METRICS(hdc, &tm)) {
            text_h = tm.tmHeight;
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

static void UpdateMenuChecks(HWND hwnd)
{
    HMENU menu = GetMenu(hwnd);
    if (!menu) {
        return;
    }

    HMENU options = GetSubMenu(menu, 1);
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
    }

    EnableMenuItem(menu, IDM_PASS, MF_BYCOMMAND | (g_game.thinking ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, IDM_HINT, MF_BYCOMMAND | (g_game.thinking || g_game.game_over ? MF_GRAYED : MF_ENABLED));
    DrawMenuBar(hwnd);
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
    g_game.hint_row = -1;
    g_game.hint_col = -1;
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

static void Draw3DCell(HDC hdc, RECT rc)
{
    HBRUSH face = CreateSolidBrush(g_cellGray);
    FillRect(hdc, &rc, face);

    HPEN gray = CreatePen(PS_SOLID, 1, g_darkGray);
    HPEN white = CreatePen(PS_SOLID, 1, kClassicLight);
    HPEN black = CreatePen(PS_SOLID, 1, kClassicDark);
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
    DeleteObject(face);
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

    HBRUSH white_brush = CreateSolidBrush(kClassicLight);
    HBRUSH gray_brush = CreateSolidBrush(g_darkGray);
    HBRUSH main_brush = CreateSolidBrush(main_color);
    HPEN white_pen = CreatePen(PS_SOLID, 1, kClassicLight);
    HPEN gray_pen = CreatePen(PS_SOLID, 1, g_darkGray);
    HPEN black_pen = CreatePen(PS_SOLID, 1, kClassicDark);
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

static void DrawHint(HDC hdc, RECT rc)
{
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int cx = rc.left + w / 2;
    int cy = rc.top + h / 2;
    int arm = MinInt(w, h) / 4;

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    MoveToEx(hdc, cx - arm, cy, NULL);
    LineTo(hdc, cx + arm, cy);
    MoveToEx(hdc, cx, cy - arm, NULL);
    LineTo(hdc, cx, cy + arm);
    SelectObject(hdc, old_pen);
    DeleteObject(pen);
}

static void DrawMessageLine(HWND hwnd, HDC hdc, const Layout *layout)
{
    HBRUSH face = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdc, &layout->message, face);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, kClassicDark);

    HFONT font = (HFONT)GetStockObject(SYSTEM_FONT);
    HGDIOBJ old_font = SelectObject(hdc, font);
    int len = APP_LSTRLEN(g_game.message);
    SIZE text_size = {0, 0};
    APP_GET_TEXT_EXTENT(hdc, g_game.message, len, &text_size);
    int x = layout->message.left + ((layout->message.right - layout->message.left) - text_size.cx) / 2;
    APP_TEXT_OUT(hdc, x, layout->message.top, g_game.message, len);
    SelectObject(hdc, old_font);

    if (g_game.flash_ticks > 0 && (g_game.flash_ticks & 1)) {
        InvertRect(hdc, &layout->message);
    }
    (void)hwnd;
}

static void DrawGame(HWND hwnd, HDC hdc)
{
    Layout layout;
    CalculateLayout(hwnd, &layout);

    RECT client;
    GetClientRect(hwnd, &client);
    HBRUSH face = CreateSolidBrush(g_windowGray);
    FillRect(hdc, &client, face);
    DeleteObject(face);
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

            if (!g_game.game_over && !g_game.thinking &&
                !(g_placeFlash.active && g_placeFlash.row == r && g_placeFlash.col == c) &&
                g_game.hint_row == r && g_game.hint_col == c &&
                g_game.cells[r][c] == EMPTY) {
                DrawHint(hdc, cell);
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

    HPEN edge = CreatePen(PS_SOLID, 1, kClassicDark);
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

static void ClearHint(HWND hwnd)
{
    if (g_game.hint_row >= 0) {
        int r = g_game.hint_row;
        int c = g_game.hint_col;
        g_game.hint_row = -1;
        g_game.hint_col = -1;
        InvalidateCell(hwnd, r, c);
    }
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
            SetMessage(hwnd, NULL, 0);
        }
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
    ClearHint(hwnd);

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

static void PlayerMove(HWND hwnd, int row, int col)
{
    if (g_game.game_over || g_game.thinking || g_game.turn != RED || g_game.must_pass) {
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

    ClearHint(hwnd);
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
    if (g_game.game_over || g_game.thinking) {
        return;
    }

    if (HasLegalMove(RED)) {
        MessageBeep(MB_ICONWARNING);
        APP_CHAR message[192];
        APP_CHAR title[64];
        LoadText(IDS_CANNOT_PASS, message, 192);
        LoadText(IDS_APP_TITLE, title, 64);
        APP_MESSAGE_BOX(hwnd, message, title, MB_OK | MB_ICONINFORMATION);
        return;
    }

    g_game.must_pass = 0;
    g_game.turn = BLUE;
    APP_CHAR message[64];
    LoadText(IDS_PASS_TEXT, message, 64);
    SetMessage(hwnd, message, 1);
    FinishTurn(hwnd);
}

static void ShowHint(HWND hwnd)
{
    if (g_game.game_over || g_game.thinking || g_game.turn != RED || g_game.must_pass) {
        MessageBeep(MB_ICONWARNING);
        return;
    }

    Move move = FindBestMove(RED);
    if (move.row < 0) {
        TryPass(hwnd);
        return;
    }

    ClearHint(hwnd);
    g_game.practice_title = 1;
    SetTitle(hwnd);
    g_game.hint_row = move.row;
    g_game.hint_col = move.col;
    g_game.selected_row = move.row;
    g_game.selected_col = move.col;
    InvalidateCell(hwnd, move.row, move.col);

    Layout layout;
    CalculateLayout(hwnd, &layout);
    RECT rc = CellRect(&layout, move.row, move.col);
    POINT pt;
    pt.x = rc.left + layout.cell_w / 2;
    pt.y = rc.top + layout.cell_h / 2;
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
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
            opened = OpenWindowsChmHelp(hwnd, HH_DISPLAY_TOC_LOCAL);
        }
        if (!opened && !force_chm) {
            opened = AppWinHelp(hwnd, NULL, HELP_HELPONHELP, 0);
        }
        if (!opened && !force_chm) {
            opened = OpenWindowsWinHelp(hwnd);
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
            opened = OpenLocalChmHelp(hwnd, HH_DISPLAY_TOC_LOCAL);
        }
        if (!opened && !force_chm) {
            opened = OpenLocalWinHelp(hwnd, HELP_CONTENTS, 0);
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
    APP_CHAR text[512];
    APP_CHAR title[64];
    LoadText(text_id, text, 512);
    LoadText(IDS_HELP_TITLE, title, 64);
    APP_MESSAGE_BOX(hwnd, text, title, MB_OK | MB_ICONINFORMATION);
}

static void MoveSelection(HWND hwnd, int dr, int dc)
{
    if (g_game.game_over || g_game.thinking) {
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
    HCURSOR cursor = APP_LOAD_CURSOR(g_hinst, MAKEINTRESOURCE(IDC_REVERSI_CROSS));
    if (!cursor) {
        cursor = APP_LOAD_CURSOR(NULL, IDC_CROSS);
    }
    return cursor;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_CREATE:
        SetMenu(hwnd, APP_LOAD_MENU(g_hinst, MAKEINTRESOURCE(IDR_MAINMENU)));
        InitGameState();
        SetTitle(hwnd);
        UpdateMenuChecks(hwnd);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wparam)) {
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
            SetAnimation(hwnd, LOWORD(wparam));
            return 0;
        case IDM_HELP_CONTENTS:
        case IDM_HELP_SEARCH:
        case IDM_HELP_USING:
            ShowHelp(hwnd, LOWORD(wparam));
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

    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        HandleClick(hwnd, lparam);
        return 0;

    case WM_MOUSEMOVE: {
        int row;
        int col;
        if (!g_game.game_over && !g_game.thinking && !g_game.must_pass &&
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
            if (!g_game.game_over && !g_game.thinking && !g_game.must_pass &&
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

    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
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

    if (CountPieces(RED) != 2 || CountPieces(BLUE) != 2) {
        return 2;
    }
    if (CollectFlips(2, 3, RED, NULL, 0) != 1 || !HasLegalMove(RED)) {
        return 3;
    }
    int board[BOARD_N][BOARD_N];
    CopyBoard(board, g_game.cells);
    if (!ApplyMoveOnBoard(board, 2, 3, RED) || board[3][3] != RED) {
        return 4;
    }
    Move move = FindBestMove(BLUE);
    if (move.row < 0 || move.col < 0) {
        return 5;
    }
    return 0;
}

static int ReversiMain(HINSTANCE hinst, int show)
{
    g_hinst = hinst;
    InitWideCommandMode();
    InitModernDispatch();
    InitClassicVisualStyles();
    InitSystemDpiAwareness();
    InitClassicColors();
    LoadGameSettings();

    if (AppCommandHasSelfTest()) {
        return RunSelfTest();
    }

    const APP_CHAR class_name[] = APP_TEXT("NativeReversiWindow");
    APP_CHAR title[64];
    LoadText(IDS_APP_TITLE, title, 64);

    APP_WNDCLASS wc;
    AppZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hinst;
    wc.hCursor = APP_LOAD_CURSOR(NULL, IDC_ARROW);
    wc.hIcon = APP_LOAD_ICON(hinst, MAKEINTRESOURCE(IDI_REVERSI));
    wc.hbrBackground = CreateSolidBrush(g_windowGray);
    wc.lpszClassName = class_name;

    if (!APP_REGISTER_CLASS(&wc)) {
        APP_CHAR message[128];
        LoadText(IDS_REGISTER_FAIL, message, 128);
        APP_MESSAGE_BOX(NULL, message, title, MB_OK | MB_ICONERROR);
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
        ShutdownClassicVisualStyles();
        return 1;
    }

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    HACCEL accel = APP_LOAD_ACCELERATORS(hinst, MAKEINTRESOURCE(IDR_MAINACCEL));
    MSG msg;
    while (APP_GET_MESSAGE(&msg, NULL, 0, 0) > 0) {
        if (!accel || !APP_TRANSLATE_ACCELERATOR(hwnd, accel, &msg)) {
            TranslateMessage(&msg);
            APP_DISPATCH_MESSAGE(&msg);
        }
    }

    int exit_code = (int)msg.wParam;
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
