/* Included by reversi_win32.c. Keeps Win32 compatibility, DPI, color, and GDI+ glue together. */

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

static RECT AppVirtualScreenRect(void)
{
    RECT rc;
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (width > 0 && height > 0) {
        rc.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        rc.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        rc.right = rc.left + width;
        rc.bottom = rc.top + height;
    } else {
        rc.left = 0;
        rc.top = 0;
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    return rc;
}

static void CenterDialogOnOwner(HWND dialog, HWND owner)
{
    RECT owner_rect;
    RECT dialog_rect;
    RECT screen_rect;
    int width;
    int height;
    int x;
    int y;

    if (!dialog || !owner || !IsWindow(owner)) {
        return;
    }
    if (!GetWindowRect(owner, &owner_rect) || !GetWindowRect(dialog, &dialog_rect)) {
        return;
    }

    width = dialog_rect.right - dialog_rect.left;
    height = dialog_rect.bottom - dialog_rect.top;
    x = owner_rect.left + ((owner_rect.right - owner_rect.left) - width) / 2;
    y = owner_rect.top + ((owner_rect.bottom - owner_rect.top) - height) / 2;

    screen_rect = AppVirtualScreenRect();
    if (x + width > screen_rect.right) {
        x = screen_rect.right - width;
    }
    if (y + height > screen_rect.bottom) {
        y = screen_rect.bottom - height;
    }
    if (x < screen_rect.left) {
        x = screen_rect.left;
    }
    if (y < screen_rect.top) {
        y = screen_rect.top;
    }

    SetWindowPos(dialog, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static LRESULT CALLBACK AppMessageBoxCbtProc(int code, WPARAM wparam, LPARAM lparam)
{
    HHOOK hook = g_messageBoxHook;

    if (code == HCBT_ACTIVATE) {
        HWND owner = g_messageBoxOwner;
        g_messageBoxHook = NULL;
        g_messageBoxOwner = NULL;
        if (hook) {
            UnhookWindowsHookEx(hook);
        }
        CenterDialogOnOwner((HWND)wparam, owner);
    }

    return CallNextHookEx(hook, code, wparam, lparam);
}

static void BeginMessageBoxCentering(HWND owner)
{
    if (!owner || !IsWindow(owner) || g_messageBoxHook) {
        return;
    }
    g_messageBoxOwner = owner;
    g_messageBoxHook = SetWindowsHookEx(WH_CBT, AppMessageBoxCbtProc, NULL, GetCurrentThreadId());
    if (!g_messageBoxHook) {
        g_messageBoxOwner = NULL;
    }
}

static void EndMessageBoxCentering(void)
{
    if (g_messageBoxHook) {
        UnhookWindowsHookEx(g_messageBoxHook);
        g_messageBoxHook = NULL;
    }
    g_messageBoxOwner = NULL;
}

#ifdef UNICODE
static int AppMessageBox(HWND hwnd, const WCHAR *text, const WCHAR *caption, UINT type)
{
    int result;

    BeginMessageBoxCentering(hwnd);
    result = MessageBoxW(hwnd, text, caption, type);
    EndMessageBoxCentering();
    return result;
}

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
typedef int (WINAPI *DrawTextWProc)(HDC, LPCWSTR, int, LPRECT, UINT);
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
    int result;

    BeginMessageBoxCentering(hwnd);
    if (g_useWideCommands) {
        MessageBoxWProc message_box =
            (MessageBoxWProc)AppGetProc("user32.dll", "MessageBoxW");
        if (message_box) {
            WCHAR wide_text[512];
            WCHAR wide_caption[128];
            AppToWide(text, wide_text, 512);
            AppToWide(caption, wide_caption, 128);
            result = message_box(hwnd, wide_text, wide_caption, type);
            EndMessageBoxCentering();
            return result;
        }
    }
    result = MessageBoxA(hwnd, text, caption, type);
    EndMessageBoxCentering();
    return result;
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
            if (text) {
                AppToWide(text, wide_text, 256);
            }
            return shell_about(hwnd, wide_title, text ? wide_text : NULL, icon);
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

static int AppDrawText(HDC hdc, const char *text, int len, LPRECT rc, UINT format)
{
    if (g_useWideCommands) {
        DrawTextWProc draw_text = (DrawTextWProc)AppGetProc("user32.dll", "DrawTextW");
        if (draw_text) {
            WCHAR wide[256];
            AppToWide(text, wide, 256);
            return draw_text(hdc, wide, AppWideLen(wide), rc, format);
        }
    }
    return DrawTextA(hdc, text, len, rc, format);
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
