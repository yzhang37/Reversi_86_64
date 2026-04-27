/* Included by reversi_win32.c. Registry-backed settings, dark theme probes, and saved window state. */

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
    case IDM_NOVICE:
    default:
        return 2;
    }
}

static int NormalizeSkillCommand(int cmd)
{
    switch (cmd) {
    case IDM_BEGINNER:
    case IDM_NOVICE:
    case IDM_EXPERT:
    case IDM_MASTER:
        return cmd;
    default:
        return IDM_NOVICE;
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
    case IDM_NOVICE:
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
        return IDM_NOVICE;
    case 2:
        return IDM_EXPERT;
    case 3:
        return IDM_MASTER;
    default:
        return IDM_NOVICE;
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

    g_configSkillCmd = IDM_NOVICE;
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
