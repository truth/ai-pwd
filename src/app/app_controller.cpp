#include "app_controller.h"
#include "clipboard_helper.h"
#include "../resource.h"
#include <shellapi.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <windowsx.h>
#include <algorithm>

namespace pwd {

namespace {

constexpr wchar_t kShellClass[] = L"AiPwdShellClass";
constexpr UINT WM_APP_ACTION = WM_APP + 10;

LRESULT HitTestResizeBorder(HWND hwnd, POINT screenPt) {
    if (IsZoomed(hwnd)) {
        return HTCLIENT;
    }
    RECT bounds{};
    GetClientRect(hwnd, &bounds);
    POINT pt = screenPt;
    ScreenToClient(hwnd, &pt);

    constexpr int border = 8;
    const bool left = pt.x < bounds.left + border;
    const bool right = pt.x >= bounds.right - border;
    const bool top = pt.y < bounds.top + border;
    const bool bottom = pt.y >= bounds.bottom - border;

    if (top && left) return HTTOPLEFT;
    if (top && right) return HTTOPRIGHT;
    if (bottom && left) return HTBOTTOMLEFT;
    if (bottom && right) return HTBOTTOMRIGHT;
    if (left) return HTLEFT;
    if (right) return HTRIGHT;
    if (top) return HTTOP;
    if (bottom) return HTBOTTOM;

    return HTCLIENT;
}

} // namespace

AppController::AppController(HINSTANCE instance) : m_instance(instance) {}

AppController::~AppController() {
    RemoveTrayIcon();
    m_host.reset();
    if (m_shellWnd) {
        DestroyWindow(m_shellWnd);
        m_shellWnd = nullptr;
    }
}

void AppController::InitTrayIcon() {
    if (!m_shellWnd || m_trayAdded) return;

    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = m_shellWnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;

    HICON hIcon = LoadIconW(m_instance, MAKEINTRESOURCEW(IDI_APP_ICON));
    if (!hIcon) {
        hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    }
    m_nid.hIcon = hIcon;
    wcscpy_s(m_nid.szTip, L"AI 密码生成器与保险库 (AI-Pwd)");

    if (Shell_NotifyIconW(NIM_ADD, &m_nid)) {
        m_trayAdded = true;
    }

    if (!m_taskbarCreatedMsg) {
        m_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    }
}

void AppController::RemoveTrayIcon() {
    if (m_trayAdded) {
        Shell_NotifyIconW(NIM_DELETE, &m_nid);
        m_trayAdded = false;
    }
}

void AppController::ShowTrayMenu() {
    if (!m_shellWnd) return;

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(m_shellWnd);

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    AppendMenuW(hMenu, MF_STRING, ID_TRAY_SHOW, L"⚡ 显示主界面");
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_GEN, L"⚡ 密码生成器");
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_VAULT, L"🛡 密码保险库");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"❌ 退出程序");

    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_RIGHTALIGN, pt.x, pt.y, 0, m_shellWnd, nullptr);
    PostMessageW(m_shellWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);
}

void AppController::HostActionThunk(const char* actionId, void* user) {
    if (user && actionId) {
        static_cast<AppController*>(user)->EnqueueAction(actionId);
    }
}

void AppController::EnqueueAction(const std::string& actionId) {
    m_pendingActions.push_back(actionId);
    if (m_shellWnd) {
        PostMessageW(m_shellWnd, WM_APP_ACTION, 0, 0);
    }
}

LRESULT CALLBACK AppController::ShellWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    auto* app = reinterpret_cast<AppController*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_NCCALCSIZE: {
            if (wParam == TRUE) {
                auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                if (params && IsZoomed(hwnd)) {
                    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO info{};
                    info.cbSize = sizeof(info);
                    if (GetMonitorInfoW(monitor, &info)) {
                        params->rgrc[0] = info.rcWork;
                    }
                }
                return 0; // Entire window is client area, zero system non-client border
            }
            return 0;
        }

        case WM_NCPAINT:
            return 0; // Suppress system frame painting

        case WM_NCHITTEST: {
            POINT screenPt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            LRESULT hit = HitTestResizeBorder(hwnd, screenPt);
            if (hit != HTCLIENT) {
                return hit;
            }
            break;
        }

        case WM_GETMINMAXINFO: {
            auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
            if (info) {
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{};
                mi.cbSize = sizeof(mi);
                if (GetMonitorInfoW(monitor, &mi)) {
                    info->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
                    info->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
                    info->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
                    info->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
                    info->ptMaxTrackSize.x = info->ptMaxSize.x;
                    info->ptMaxTrackSize.y = info->ptMaxSize.y;
                }
                info->ptMinTrackSize.x = 900;
                info->ptMinTrackSize.y = 600;
            }
            return 0;
        }

        case WM_DPICHANGED: {
            auto* prc = reinterpret_cast<RECT*>(lParam);
            if (prc) {
                SetWindowPos(hwnd, nullptr, prc->left, prc->top,
                             prc->right - prc->left, prc->bottom - prc->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) != WA_INACTIVE && app && app->m_host && !app->m_host->IsClosed()) {
                HWND childHwnd = app->m_host->GetHwnd();
                if (childHwnd) {
                    if (IsIconic(childHwnd)) {
                        ShowWindow(childHwnd, SW_RESTORE);
                    }
                    if (!IsWindowVisible(childHwnd)) {
                        ShowWindow(childHwnd, SW_SHOW);
                    }
                    app->m_host->FitToParent();
                    InvalidateRect(childHwnd, nullptr, TRUE);
                }
            }
            break;
        }

        case WM_SETFOCUS: {
            if (app && app->m_host && !app->m_host->IsClosed()) {
                HWND childHwnd = app->m_host->GetHwnd();
                if (childHwnd) {
                    SetFocus(childHwnd);
                }
            }
            return 0;
        }

        case WM_SIZE: {
            if (wParam == SIZE_MINIMIZED) {
                return 0; // Do not resize child to 0x0 while shell is minimized
            }
            RECT rc{};
            if (GetClientRect(hwnd, &rc)) {
                int width = rc.right - rc.left;
                int height = rc.bottom - rc.top;
                if (IsZoomed(hwnd)) {
                    SetWindowRgn(hwnd, nullptr, TRUE);
                } else if (width > 0 && height > 0) {
                    HRGN rgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, 16, 16);
                    SetWindowRgn(hwnd, rgn, TRUE);
                }
            }
            if (app && app->m_host && !app->m_host->IsClosed()) {
                HWND childHwnd = app->m_host->GetHwnd();
                if (childHwnd) {
                    if (IsIconic(childHwnd)) {
                        ShowWindow(childHwnd, SW_RESTORE);
                    }
                    if (!IsWindowVisible(childHwnd)) {
                        ShowWindow(childHwnd, SW_SHOW);
                    }
                    // Keep child WS_MAXIMIZE style in sync with parent shell so IsZoomed(childHwnd)
                    // accurately reflects maximized state for caption glyph rendering (restore vs maximize).
                    LONG_PTR style = GetWindowLongPtrW(childHwnd, GWL_STYLE);
                    if (IsZoomed(hwnd)) {
                        SetWindowLongPtrW(childHwnd, GWL_STYLE, style | WS_MAXIMIZE);
                    } else {
                        SetWindowLongPtrW(childHwnd, GWL_STYLE, style & ~WS_MAXIMIZE);
                    }
                }
                app->m_host->FitToParent();
                if (childHwnd) {
                    InvalidateRect(childHwnd, nullptr, TRUE);
                }
            }
            return 0;
        }

        case WM_TRAYICON: {
            if (lParam == WM_LBUTTONUP || lParam == WM_LBUTTONDBLCLK) {
                if (IsIconic(hwnd) || !IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                } else if (GetForegroundWindow() == hwnd && lParam == WM_LBUTTONUP) {
                    ShowWindow(hwnd, SW_MINIMIZE);
                } else {
                    SetForegroundWindow(hwnd);
                }
            } else if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU) {
                if (app) {
                    app->ShowTrayMenu();
                }
            }
            return 0;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_TRAY_SHOW:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    return 0;
                case ID_TRAY_GEN:
                    if (app) {
                        app->m_state.activeTab = ActiveTab::Generator;
                        app->m_state.isEditing = false;
                        app->RecreateHost();
                        ShowWindow(hwnd, SW_RESTORE);
                        SetForegroundWindow(hwnd);
                    }
                    return 0;
                case ID_TRAY_VAULT:
                    if (app) {
                        app->m_state.activeTab = ActiveTab::Vault;
                        app->m_state.isEditing = false;
                        app->RecreateHost();
                        ShowWindow(hwnd, SW_RESTORE);
                        SetForegroundWindow(hwnd);
                    }
                    return 0;
                case ID_TRAY_EXIT:
                    if (app) {
                        app->RemoveTrayIcon();
                    }
                    DestroyWindow(hwnd);
                    return 0;
                default:
                    break;
            }
            break;
        }

        case WM_APP_ACTION: {
            if (app) {
                app->DrainActions();
            }
            return 0;
        }

        case WM_DESTROY: {
            if (app) {
                app->RemoveTrayIcon();
                app->m_host.reset();
            }
            PostQuitMessage(0);
            return 0;
        }

        default:
            if (app && app->m_taskbarCreatedMsg != 0 && msg == app->m_taskbarCreatedMsg) {
                app->m_trayAdded = false;
                app->InitTrayIcon();
                return 0;
            }
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK AppController::ChildHostSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* app = reinterpret_cast<AppController*>(dwRefData);
    HWND shellWnd = app ? app->m_shellWnd : GetParent(hwnd);

    switch (uMsg) {
        case WM_SYSCOMMAND: {
            WPARAM cmd = wParam & 0xFFF0;
            if (cmd == SC_MINIMIZE || cmd == SC_MAXIMIZE || cmd == SC_RESTORE || cmd == SC_CLOSE) {
                if (shellWnd) {
                    SendMessageW(shellWnd, WM_SYSCOMMAND, wParam, lParam);
                    return 0;
                }
            }
            break;
        }

        case WM_NCHITTEST: {
            if (shellWnd && !IsZoomed(shellWnd)) {
                POINT screenPt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                POINT clientPt = screenPt;
                ScreenToClient(hwnd, &clientPt);
                RECT rc{};
                GetClientRect(hwnd, &rc);
                constexpr int kGrip = 7;
                // If near the boundaries, let parent Shell window handle resize
                if (clientPt.x < kGrip || clientPt.x >= rc.right - kGrip ||
                    clientPt.y < kGrip || clientPt.y >= rc.bottom - kGrip) {
                    return HTTRANSPARENT;
                }
            }
            break;
        }

        case WM_MOUSEACTIVATE: {
            SetFocus(hwnd);
            return MA_ACTIVATE;
        }

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN: {
            SetFocus(hwnd);
            break;
        }

        case WM_LBUTTONDOWN: {
            SetFocus(hwnd);
            int y = GET_Y_LPARAM(lParam);
            int x = GET_X_LPARAM(lParam);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            // Title bar height is 46px.
            // Right controls take ~340px (theme switcher dots + caption buttons).
            // Everything to the left of (rc.right - 340) initiates smooth native caption dragging on shellWnd!
            if (y >= 0 && y < 46 && x < (rc.right - 340)) {
                ReleaseCapture();
                SendMessageW(shellWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                return 0;
            }
            break;
        }

        case WM_LBUTTONDBLCLK: {
            SetFocus(hwnd);
            int y = GET_Y_LPARAM(lParam);
            int x = GET_X_LPARAM(lParam);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            if (y >= 0 && y < 46 && x < (rc.right - 340)) {
                if (shellWnd) {
                    ShowWindow(shellWnd, IsZoomed(shellWnd) ? SW_RESTORE : SW_MAXIMIZE);
                }
                return 0;
            }
            break;
        }

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hwnd, ChildHostSubclassProc, uIdSubclass);
            break;
        }

        default:
            break;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AppController::UpdateThemeDwmAttributes() {
    if (!m_shellWnd) return;
    ThemePalette curPalette = GetThemePalette(m_state.theme);

    // 1. DWM Dark mode
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(m_shellWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &useDarkMode, sizeof(useDarkMode));
    DwmSetWindowAttribute(m_shellWnd, 19, &useDarkMode, sizeof(useDarkMode));

    // 2. Windows 11 DWM rounded corners preference (DWMWCP_ROUND = 2)
    DWORD cornerPreference = 2;
    DwmSetWindowAttribute(m_shellWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &cornerPreference, sizeof(cornerPreference));

    // 3. Theme-driven DWM border color (instant, 100% flicker-free)
    COLORREF themeDwmBorder = static_cast<COLORREF>(curPalette.dwmBorderColor);
    DwmSetWindowAttribute(m_shellWnd, 34 /* DWMWA_BORDER_COLOR */, &themeDwmBorder, sizeof(themeDwmBorder));

    // 4. Update Shell window background brush
    static HBRUSH s_shellBrush = nullptr;
    HBRUSH newBrush = CreateSolidBrush(static_cast<COLORREF>(curPalette.classBgColor));
    SetClassLongPtrW(m_shellWnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(newBrush));
    if (s_shellBrush) {
        DeleteObject(s_shellBrush);
    }
    s_shellBrush = newBrush;
}

bool AppController::Initialize(const std::wstring& cmdLine) {
    // 0. Ensure assets.zip is present next to executable (extract from embedded RC if needed)
    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    std::wstring exeDir = lastSlash ? std::wstring(exePath, lastSlash) : L".";
    std::wstring zipPath = exeDir + L"\\assets.zip";
    std::wstring resourceDir = exeDir + L"\\resource";

    if (GetFileAttributesW(zipPath.c_str()) == INVALID_FILE_ATTRIBUTES &&
        GetFileAttributesW(resourceDir.c_str()) == INVALID_FILE_ATTRIBUTES) {
        HRSRC hRes = FindResourceW(m_instance, MAKEINTRESOURCEW(IDR_EMBEDDED_ASSETS), RT_RCDATA);
        if (hRes) {
            HGLOBAL hData = LoadResource(m_instance, hRes);
            if (hData) {
                DWORD size = SizeofResource(m_instance, hRes);
                const void* ptr = LockResource(hData);
                if (ptr && size > 0) {
                    HANDLE hFile = CreateFileW(zipPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                    if (hFile != INVALID_HANDLE_VALUE) {
                        DWORD written = 0;
                        WriteFile(hFile, ptr, size, &written, nullptr);
                        CloseHandle(hFile);
                    }
                }
            }
        }
    }

    // 1. Initialize SQLite Database
    if (!m_db.Init()) {
        MessageBoxW(nullptr, L"无法初始化 SQLite 数据库", L"错误", MB_ICONERROR);
        return false;
    }

    // 2. Restore persisted theme preference
    std::string savedTheme = m_db.GetSetting("theme", "blue");
    if (savedTheme == "purple") m_state.theme = ThemeColor::Purple;
    else if (savedTheme == "cyan") m_state.theme = ThemeColor::Cyan;
    else if (savedTheme == "emerald") m_state.theme = ThemeColor::Emerald;
    else if (savedTheme == "amber") m_state.theme = ThemeColor::Amber;
    else m_state.theme = ThemeColor::Blue;

    // 3. Register Shell Window Class
    ThemePalette initialPalette = GetThemePalette(m_state.theme);
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &AppController::ShellWndProc;
    wc.hInstance = m_instance;
    wc.lpszClassName = kShellClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(static_cast<COLORREF>(initialPalette.classBgColor));
    wc.style = CS_DBLCLKS;
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    // 4. Center on screen: default 1180x820
    const int initialW = 1180;
    const int initialH = 820;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int posX = (std::max)(40, (screenW - initialW) / 2);
    int posY = (std::max)(40, (screenH - initialH) / 2);

    m_shellWnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        kShellClass,
        L"AI 密码生成器与保险库 (AI-Pwd)",
        WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN,
        posX, posY, initialW, initialH,
        nullptr, nullptr, m_instance, this
    );
    if (!m_shellWnd) {
        return false;
    }

    // 5. Initialize DWM shadow & attributes
    MARGINS margins{0, 0, 1, 0};
    DwmExtendFrameIntoClientArea(m_shellWnd, &margins);
    UpdateThemeDwmAttributes();

    // 6. Set 16px rounded window region
    HRGN rgn = CreateRoundRectRgn(0, 0, initialW + 1, initialH + 1, 16, 16);
    SetWindowRgn(m_shellWnd, rgn, TRUE);

    // 7. Set App Icon
    HICON hIcon = LoadIconW(m_instance, MAKEINTRESOURCEW(IDI_APP_ICON));
    if (hIcon) {
        SendMessageW(m_shellWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
        SendMessageW(m_shellWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
    }

    // 8. Initialize System Tray Icon
    InitTrayIcon();

    // 8. Initial generator evaluation
    m_state.genOptions.length = 16;
    m_state.genOptions.mode = GeneratorMode::Balanced;
    m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
    m_state.stats = m_db.GetStats();

    if (cmdLine.find(L"vault") != std::wstring::npos) {
        m_state.activeTab = ActiveTab::Vault;
    } else if (cmdLine.find(L"edit") != std::wstring::npos || cmdLine.find(L"new") != std::wstring::npos) {
        m_state.isEditing = true;
        m_state.editingItem = VaultItem{};
        m_state.editingItem.category = L"工作";
        m_state.editingItem.password = m_state.currentAssessment.password;
    }

    // 9. Show shell window and create child Host UI
    ShowWindow(m_shellWnd, SW_SHOW);
    UpdateWindow(m_shellWnd);

    RecreateHost();
    return true;
}

int AppController::Run() {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (m_host && !m_host->IsClosed()) {
            HWND childHwnd = m_host->GetHwnd();
            if (childHwnd) {
                // If keyboard input arrives targeting the outer shell, forward to child host
                if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) {
                    if (msg.hwnd == m_shellWnd) {
                        msg.hwnd = childHwnd;
                    }
                    // Enter key triggers search on Vault/Favorites tabs
                    if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
                        if ((m_state.activeTab == ActiveTab::Vault || m_state.activeTab == ActiveTab::Favorites) && !m_state.isEditing) {
                            EnqueueAction("vault:search");
                            continue;
                        }
                    }
                }
            }
            if (m_host->ProcessMessage(msg)) {
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (!IsWindow(m_shellWnd)) {
            break;
        }
    }
    return static_cast<int>(msg.wParam);
}

void AppController::DrainActions() {
    std::vector<std::string> actions;
    actions.swap(m_pendingActions);
    for (const auto& act : actions) {
        HandleAction(act);
    }
}

void AppController::ShowToast(const std::wstring& msg, const std::string& color) {
    m_state.toastMessage = msg;
    m_state.toastColor = color;
}

void AppController::RecreateHost() {
    if (!m_shellWnd) return;

    // 1. Immediately update DWM border & Shell background brush without destroying m_shellWnd
    UpdateThemeDwmAttributes();

    // 2. Query items and build layout
    auto items = m_db.QueryItems(m_state.searchQuery, m_state.selectedCategory, (m_state.activeTab == ActiveTab::Favorites));
    m_state.stats = m_db.GetStats();

    std::string xml = LayoutBuilder::BuildMainLayout(m_state, items);

    ai_win_ui::HostCreateInfo info{};
    info.parent = m_shellWnd; // Embed as WS_CHILD in persistent shell window!
    info.inlineLayoutXml = xml.c_str();
    info.ownMessagePump = false;
    info.onAction = &AppController::HostActionThunk;
    info.actionUser = this;
    info.showCommand = SW_SHOW;

    auto newHost = ai_win_ui::Host::Create(m_instance, info);
    if (newHost) {
        HWND childHwnd = newHost->GetHwnd();
        if (childHwnd) {
            SetWindowSubclass(childHwnd, ChildHostSubclassProc, 1002, reinterpret_cast<DWORD_PTR>(this));
        }
        newHost->FitToParent();
        m_host = std::move(newHost);
        PopulateFormFields();
    }
}

void AppController::PopulateFormFields() {
    if (!m_host) return;

    if (m_state.isEditing) {
        m_host->SetElementText("edit_title", m_state.editingItem.title.c_str());
        m_host->SetElementText("edit_category", m_state.editingItem.category.c_str());
        m_host->SetElementText("edit_username", m_state.editingItem.username.c_str());
        m_host->SetElementText("edit_password", m_state.editingItem.password.c_str());
        m_host->SetElementText("edit_url", m_state.editingItem.url.c_str());
        m_host->SetElementText("edit_notes", m_state.editingItem.notes.c_str());
    } else {
        if (!m_state.searchQuery.empty()) {
            m_host->SetElementText("vault_search", m_state.searchQuery.c_str());
        }
    }
}

void AppController::HandleAction(const std::string& id) {
    auto startsWith = [&](const char* prefix) {
        return id.rfind(prefix, 0) == 0;
    };

    // ---------------------------------------------------------
    // Window Chrome Actions
    // ---------------------------------------------------------
    if (id == "app:windowMinimize" || id == "windowMinimize") {
        if (m_shellWnd) {
            ShowWindow(m_shellWnd, SW_MINIMIZE);
        }
        return;
    }
    if (id == "app:windowMaximize" || id == "windowMaximize") {
        if (m_shellWnd) {
            ShowWindow(m_shellWnd, IsZoomed(m_shellWnd) ? SW_RESTORE : SW_MAXIMIZE);
        }
        return;
    }
    if (id == "app:windowClose" || id == "windowClose") {
        if (m_shellWnd) {
            PostMessageW(m_shellWnd, WM_CLOSE, 0, 0);
        } else {
            PostQuitMessage(0);
        }
        return;
    }

    // ---------------------------------------------------------
    // Theme Switcher Actions
    // ---------------------------------------------------------
    if (id == "theme:blue") {
        m_state.theme = ThemeColor::Blue;
        m_db.SetSetting("theme", "blue");
        RecreateHost();
        return;
    }
    if (id == "theme:purple") {
        m_state.theme = ThemeColor::Purple;
        m_db.SetSetting("theme", "purple");
        RecreateHost();
        return;
    }
    if (id == "theme:cyan") {
        m_state.theme = ThemeColor::Cyan;
        m_db.SetSetting("theme", "cyan");
        RecreateHost();
        return;
    }
    if (id == "theme:emerald") {
        m_state.theme = ThemeColor::Emerald;
        m_db.SetSetting("theme", "emerald");
        RecreateHost();
        return;
    }
    if (id == "theme:amber") {
        m_state.theme = ThemeColor::Amber;
        m_db.SetSetting("theme", "amber");
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Toast Dismiss
    // ---------------------------------------------------------
    if (id == "toast:dismiss") {
        m_state.toastMessage = L"";
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Navigation Tabs
    // ---------------------------------------------------------
    if (id == "nav:generator") {
        m_state.activeTab = ActiveTab::Generator;
        m_state.isEditing = false;
        RecreateHost();
        return;
    }
    if (id == "nav:vault") {
        m_state.activeTab = ActiveTab::Vault;
        m_state.isEditing = false;
        RecreateHost();
        return;
    }
    if (id == "nav:favorites") {
        m_state.activeTab = ActiveTab::Favorites;
        m_state.isEditing = false;
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Category Filter
    // ---------------------------------------------------------
    if (startsWith("cat:")) {
        std::string catStr = id.substr(4);
        m_state.selectedCategory = Utf8ToWide(catStr);
        if (m_state.activeTab == ActiveTab::Generator) {
            m_state.activeTab = ActiveTab::Vault;
        }
        m_state.isEditing = false;
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Generator: Presets
    // ---------------------------------------------------------
    if (id == "gen:preset:balanced") {
        m_state.genOptions.mode = GeneratorMode::Balanced;
        m_state.genOptions.length = 16;
        m_state.genOptions.includeUpper = true;
        m_state.genOptions.includeLower = true;
        m_state.genOptions.includeDigits = true;
        m_state.genOptions.includeSymbols = true;
        m_state.genOptions.excludeAmbiguous = false;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:preset:high") {
        m_state.genOptions.mode = GeneratorMode::HighSecurity;
        m_state.genOptions.length = 24;
        m_state.genOptions.includeUpper = true;
        m_state.genOptions.includeLower = true;
        m_state.genOptions.includeDigits = true;
        m_state.genOptions.includeSymbols = true;
        m_state.genOptions.excludeAmbiguous = false;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:preset:pin") {
        m_state.genOptions.mode = GeneratorMode::Pin;
        m_state.genOptions.length = 6;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:preset:memorable") {
        m_state.genOptions.mode = GeneratorMode::Memorable;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:preset:noamb") {
        m_state.genOptions.mode = GeneratorMode::NoAmbiguous;
        m_state.genOptions.length = 16;
        m_state.genOptions.includeUpper = true;
        m_state.genOptions.includeLower = true;
        m_state.genOptions.includeDigits = true;
        m_state.genOptions.includeSymbols = true;
        m_state.genOptions.excludeAmbiguous = true;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Generator: Length controls
    // ---------------------------------------------------------
    if (id == "gen:len:dec") {
        if (m_state.genOptions.length > 6) {
            m_state.genOptions.length--;
            if (m_state.genOptions.mode == GeneratorMode::Memorable) {
                m_state.genOptions.mode = GeneratorMode::Custom;
            }
            m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
            RecreateHost();
        }
        return;
    }
    if (id == "gen:len:inc") {
        if (m_state.genOptions.length < 64) {
            m_state.genOptions.length++;
            if (m_state.genOptions.mode == GeneratorMode::Memorable) {
                m_state.genOptions.mode = GeneratorMode::Custom;
            }
            m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
            RecreateHost();
        }
        return;
    }
    if (startsWith("gen:len:")) {
        int l = std::stoi(id.substr(8));
        m_state.genOptions.length = l;
        if (m_state.genOptions.mode == GeneratorMode::Memorable) {
            m_state.genOptions.mode = GeneratorMode::Custom;
        }
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Generator: Options Toggles
    // ---------------------------------------------------------
    if (id == "gen:opt:upper") {
        m_state.genOptions.includeUpper = !m_state.genOptions.includeUpper;
        m_state.genOptions.mode = GeneratorMode::Custom;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:opt:lower") {
        m_state.genOptions.includeLower = !m_state.genOptions.includeLower;
        m_state.genOptions.mode = GeneratorMode::Custom;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:opt:digits") {
        m_state.genOptions.includeDigits = !m_state.genOptions.includeDigits;
        m_state.genOptions.mode = GeneratorMode::Custom;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:opt:symbols") {
        m_state.genOptions.includeSymbols = !m_state.genOptions.includeSymbols;
        m_state.genOptions.mode = GeneratorMode::Custom;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:opt:ambiguous") {
        m_state.genOptions.excludeAmbiguous = !m_state.genOptions.excludeAmbiguous;
        m_state.genOptions.mode = GeneratorMode::Custom;
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Generator: Refresh / Copy / SaveTo
    // ---------------------------------------------------------
    if (id == "gen:refresh") {
        m_state.currentAssessment = PasswordGenerator::Generate(m_state.genOptions);
        RecreateHost();
        return;
    }
    if (id == "gen:toggle_eye") {
        m_state.generatorRevealed = !m_state.generatorRevealed;
        RecreateHost();
        return;
    }
    if (id == "gen:copy") {
        CopyTextToClipboard(m_shellWnd, m_state.currentAssessment.password);
        if (m_host) {
            m_host->SetElementText("app_status_badge", L"✔ 生成密码已复制到剪贴板！");
            m_host->SetElementForeground("app_status_badge", 0xFF10B981);
        }
        return;
    }
    if (id == "gen:saveto") {
        // Open quick add dialog prefilled with the generated password
        m_state.isEditing = true;
        m_state.isNewItem = true;
        m_state.editingItem = VaultItem{};
        m_state.editingItem.password = m_state.currentAssessment.password;
        m_state.editingItem.category = (m_state.selectedCategory == L"全部") ? L"其它" : m_state.selectedCategory;
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Vault: Search & Filter
    // ---------------------------------------------------------
    if (id == "vault:search") {
        if (m_host) {
            m_state.searchQuery = m_host->GetElementText("vault_search");
        }
        RecreateHost();
        return;
    }
    if (id == "vault:reset_search") {
        m_state.searchQuery = L"";
        RecreateHost();
        return;
    }

    // ---------------------------------------------------------
    // Vault: Open Add / Edit
    // ---------------------------------------------------------
    if (id == "vault:open_add") {
        m_state.isEditing = true;
        m_state.isNewItem = true;
        m_state.editingItem = VaultItem{};
        m_state.editingItem.category = (m_state.selectedCategory == L"全部") ? L"其它" : m_state.selectedCategory;
        RecreateHost();
        return;
    }
    if (startsWith("vault:open_edit:")) {
        int64_t itemId = std::stoll(id.substr(16));
        VaultItem it;
        if (m_db.GetById(itemId, it)) {
            m_state.isEditing = true;
            m_state.isNewItem = false;
            m_state.editingItem = it;
            RecreateHost();
        }
        return;
    }

    // ---------------------------------------------------------
    // Vault: Delete / Star / Copy / Toggle Pwd / Open URL
    // ---------------------------------------------------------
    if (startsWith("vault:del:")) {
        int64_t itemId = std::stoll(id.substr(10));
        m_db.DeleteItem(itemId);
        m_state.revealedPasswords.erase(itemId);
        ShowToast(L"✔ 密码记录已从 SQLite 数据库安全删除", "#EF4444");
        RecreateHost();
        return;
    }
    if (startsWith("vault:star:")) {
        int64_t itemId = std::stoll(id.substr(11));
        m_db.ToggleFavorite(itemId);
        if (m_state.activeTab != ActiveTab::Favorites && m_host) {
            VaultItem it;
            if (m_db.GetById(itemId, it)) {
                std::string starBtnName = "vault_star_btn_" + std::to_string(itemId);
                m_host->SetElementText(starBtnName.c_str(), it.favorite ? L"★" : L"☆");
                m_host->SetElementForeground(starBtnName.c_str(), it.favorite ? 0xFFFBBF24 : 0xFF64748B);
                return;
            }
        }
        RecreateHost();
        return;
    }
    if (startsWith("vault:copy_user:")) {
        int64_t itemId = std::stoll(id.substr(16));
        VaultItem it;
        if (m_db.GetById(itemId, it)) {
            CopyTextToClipboard(m_shellWnd, it.username);
            if (m_host) {
                std::wstring feedback = L"✔ 已复制账号: " + it.username;
                m_host->SetElementText("app_status_badge", feedback.c_str());
                m_host->SetElementForeground("app_status_badge", 0xFF38BDF8);
            }
        }
        return;
    }
    if (startsWith("vault:copy_pwd:")) {
        int64_t itemId = std::stoll(id.substr(15));
        VaultItem it;
        if (m_db.GetById(itemId, it)) {
            CopyTextToClipboard(m_shellWnd, it.password);
            if (m_host) {
                m_host->SetElementText("app_status_badge", L"✔ 密码已安全复制到剪贴板！");
                m_host->SetElementForeground("app_status_badge", 0xFF10B981);
            }
        }
        return;
    }
    if (startsWith("vault:toggle_pwd:")) {
        int64_t itemId = std::stoll(id.substr(17));
        bool isRevealed = false;
        if (m_state.revealedPasswords.count(itemId)) {
            m_state.revealedPasswords.erase(itemId);
            isRevealed = false;
        } else {
            m_state.revealedPasswords.insert(itemId);
            isRevealed = true;
        }

        if (m_host) {
            std::string idStr = std::to_string(itemId);
            std::string lblName = "vault_pwd_lbl_" + idStr;
            std::string btnName = "vault_pwd_btn_" + idStr;
            VaultItem it;
            if (m_db.GetById(itemId, it)) {
                if (isRevealed) {
                    m_host->SetElementText(lblName.c_str(), it.password.c_str());
                    m_host->SetElementText(btnName.c_str(), L"隐藏");
                    m_host->SetElementForeground(lblName.c_str(), 0xFF10B981);
                } else {
                    std::wstring masked(it.password.size() > 0 ? (std::min)(static_cast<size_t>(16), it.password.size()) : 8, L'*');
                    m_host->SetElementText(lblName.c_str(), masked.c_str());
                    m_host->SetElementText(btnName.c_str(), L"显示");
                    m_host->SetElementForeground(lblName.c_str(), 0xFF64748B);
                }
                return;
            }
        }
        RecreateHost();
        return;
    }
    if (startsWith("vault:open_url:")) {
        int64_t itemId = std::stoll(id.substr(15));
        VaultItem it;
        if (m_db.GetById(itemId, it) && !it.url.empty()) {
            ShellExecuteW(nullptr, L"open", it.url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
        return;
    }

    // ---------------------------------------------------------
    // Form: Edit Modal Handlers
    // ---------------------------------------------------------
    if (id == "edit:genpwd") {
        // Generate a high security password and set directly into the field
        GeneratorOptions opts;
        opts.length = 18;
        opts.mode = GeneratorMode::Balanced;
        auto ass = PasswordGenerator::Generate(opts);
        m_state.editingItem.password = ass.password;
        if (m_host) {
            m_host->SetElementText("edit_password", ass.password.c_str());
        }
        ShowToast(L"✔ 已为您自动生成 18 位高强度密码！", "#10B981");
        RecreateHost();
        return;
    }
    if (id == "edit:save") {
        if (m_host) {
            m_state.editingItem.title = m_host->GetElementText("edit_title");
            m_state.editingItem.category = m_host->GetElementText("edit_category");
            m_state.editingItem.username = m_host->GetElementText("edit_username");
            m_state.editingItem.password = m_host->GetElementText("edit_password");
            m_state.editingItem.url = m_host->GetElementText("edit_url");
            m_state.editingItem.notes = m_host->GetElementText("edit_notes");
        }

        if (m_state.editingItem.title.empty()) {
            ShowToast(L"⚠ 保存失败：账号名称/平台不能为空！", "#EF4444");
            RecreateHost();
            return;
        }

        if (m_state.editingItem.category.empty()) {
            m_state.editingItem.category = L"其它";
        }

        if (m_state.isNewItem) {
            m_db.AddItem(m_state.editingItem);
            ShowToast(L"✔ 密码记录已成功保存到 SQLite 数据库！", "#10B981");
        } else {
            m_db.UpdateItem(m_state.editingItem);
            ShowToast(L"✔ 账号资料已更新！", "#10B981");
        }

        m_state.isEditing = false;
        m_state.activeTab = ActiveTab::Vault;
        RecreateHost();
        return;
    }
    if (id == "edit:cancel") {
        m_state.isEditing = false;
        RecreateHost();
        return;
    }
}

} // namespace pwd
