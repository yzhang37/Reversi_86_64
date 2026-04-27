/* Included by reversi_win32.c. Groups string/path helpers and legacy WinHelp/CHM dispatch. */

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

static int AppStringEquals(const APP_CHAR *left, const APP_CHAR *right)
{
    if (!left || !right) {
        return 0;
    }
    while (*left && *right && *left == *right) {
        ++left;
        ++right;
    }
    return *left == 0 && *right == 0;
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

static int AppBuildModuleParentFilePath(APP_CHAR *out, int cch, const APP_CHAR *file_name)
{
    if (!APP_GET_MODULE_FILENAME(g_hinst, out, (DWORD)cch)) {
        return 0;
    }

    int slash = -1;
    int previous = -1;
    for (int i = 0; out[i]; ++i) {
        if (out[i] == APP_TEXT('\\') || out[i] == APP_TEXT('/')) {
            previous = slash;
            slash = i;
        }
    }
    if (previous >= 0) {
        out[previous + 1] = 0;
    } else if (slash >= 0) {
        out[slash + 1] = 0;
    } else {
        out[0] = 0;
    }
    AppStrAppend(out, cch, file_name);
    return 1;
}

static int AppBuildMuiFilePath(APP_CHAR *out, int cch, const APP_CHAR *file_name)
{
    if (!g_muiDirectory[0]) {
        return 0;
    }
    AppStrCopy(out, cch, g_muiDirectory);
    AppStrAppend(out, cch, file_name);
    return 1;
}

#define HELP_PATH_LOCALE_NAME 0
#define HELP_PATH_MUI_HEX 1
#define HELP_PATH_HELP_MUI_HEX 2
#define HELP_PATH_MUI_LOCALE_NAME 3
#define HELP_PATH_HEX 4
#define HELP_PATH_DECIMAL 5
#define HELP_PATH_COUNT 6

static int AppAppendPathPart(APP_CHAR *path, int cch, const APP_CHAR *part)
{
    if (!part || !part[0]) {
        return 0;
    }
    AppEnsureTrailingSlash(path, cch);
    AppStrAppend(path, cch, part);
    AppEnsureTrailingSlash(path, cch);
    return 1;
}

static int AppBuildLocaleHelpPath(
    APP_CHAR *out, int cch, const APP_CHAR *base, const APP_CHAR *file_name, int layout)
{
    AppStrCopy(out, cch, base);
    AppEnsureTrailingSlash(out, cch);

    switch (layout) {
    case HELP_PATH_LOCALE_NAME:
        if (!AppAppendPathPart(out, cch, g_uiLocaleName)) {
            return 0;
        }
        break;
    case HELP_PATH_MUI_HEX:
        if (!AppAppendPathPart(out, cch, APP_TEXT("MUI")) ||
            !AppAppendPathPart(out, cch, g_uiLocaleHex)) {
            return 0;
        }
        break;
    case HELP_PATH_HELP_MUI_HEX:
        if (!AppAppendPathPart(out, cch, APP_TEXT("Help")) ||
            !AppAppendPathPart(out, cch, APP_TEXT("MUI")) ||
            !AppAppendPathPart(out, cch, g_uiLocaleHex)) {
            return 0;
        }
        break;
    case HELP_PATH_MUI_LOCALE_NAME:
        if (!AppAppendPathPart(out, cch, APP_TEXT("MUI")) ||
            !AppAppendPathPart(out, cch, g_uiLocaleName)) {
            return 0;
        }
        break;
    case HELP_PATH_HEX:
        if (!AppAppendPathPart(out, cch, g_uiLocaleHex)) {
            return 0;
        }
        break;
    case HELP_PATH_DECIMAL:
        if (!AppAppendPathPart(out, cch, g_uiLocaleDecimal)) {
            return 0;
        }
        break;
    default:
        return 0;
    }

    AppStrAppend(out, cch, file_name);
    return 1;
}

static int AppTryLocaleHelpBase(const APP_CHAR *base, const APP_CHAR *file_name, APP_CHAR *out, int cch)
{
    for (int layout = 0; layout < HELP_PATH_COUNT; ++layout) {
        if (AppBuildLocaleHelpPath(out, cch, base, file_name, layout) && AppFileExists(out)) {
            return 1;
        }
    }
    return 0;
}

static int AppResolveMuiHelpFile(const APP_CHAR *file_name, APP_CHAR *out, int cch)
{
    APP_CHAR base[MAX_PATH];
    if (AppBuildMuiFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    if (!g_uiLocaleName[0]) {
        return 0;
    }

    /* Match app-local Vista folders, XP MUI hex folders, and Windows Help\MUI hex folders. */
    if (AppBuildModuleFilePath(base, MAX_PATH, APP_TEXT("")) &&
        AppTryLocaleHelpBase(base, file_name, out, cch)) {
        return 1;
    }
    if (AppBuildModuleParentFilePath(base, MAX_PATH, APP_TEXT("")) &&
        AppTryLocaleHelpBase(base, file_name, out, cch)) {
        return 1;
    }
    if (APP_GET_WINDOWS_DIRECTORY(base, MAX_PATH)) {
        return AppTryLocaleHelpBase(base, file_name, out, cch);
    }
    return 0;
}

static int AppResolveLocalHelpFile(UINT string_id, APP_CHAR *out, int cch)
{
    APP_CHAR file_name[MAX_PATH];
    LoadText(string_id, file_name, MAX_PATH);
    if (!file_name[0]) {
        return 0;
    }
    if (AppResolveMuiHelpFile(file_name, out, cch)) {
        return 1;
    }
    if (AppBuildModuleFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    if (AppBuildModuleParentFilePath(out, cch, file_name) && AppFileExists(out)) {
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
    if (AppResolveMuiHelpFile(file_name, out, cch)) {
        return 1;
    }
    if (AppBuildModuleFilePath(out, cch, file_name) && AppFileExists(out)) {
        return 1;
    }
    if (AppBuildModuleParentFilePath(out, cch, file_name) && AppFileExists(out)) {
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
