#include "app_controller.h"
#include "clipboard_helper.h"
#include "../resource.h"
#include <shellapi.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <algorithm>

namespace pwd {

namespace {

constexpr wchar_t kAppClass[] = L"AiPwdAppClass";
constexpr UINT WM_APP_ACTION = WM_APP + 10;

LRESULT CALLBACK HostSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGED) {
        if (IsZoomed(hwnd)) {
            SetWindowRgn(hwnd, nullptr, TRUE);
        } else {
            RECT rc{};
            if (GetWindowRect(hwnd, &rc)) {
                int width = rc.right - rc.left;
                int height = rc.bottom - rc.top;
                if (width > 0 && height > 0) {
                    HRGN rgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, 16, 16);
                    SetWindowRgn(hwnd, rgn, TRUE);
                }
            }
        }
    } else if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, HostSubclassProc, uIdSubclass);
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

} // namespace

AppController::AppController(HINSTANCE instance) : m_instance(instance) {}

AppController::~AppController() {
    if (m_appWnd) {
        DestroyWindow(m_appWnd);
        m_appWnd = nullptr;
    }
}

void AppController::HostActionThunk(const char* actionId, void* user) {
    if (user && actionId) {
        static_cast<AppController*>(user)->EnqueueAction(actionId);
    }
}

void AppController::EnqueueAction(const std::string& actionId) {
    m_pendingActions.push_back(actionId);
    if (m_appWnd) {
        PostMessageW(m_appWnd, WM_APP_ACTION, 0, 0);
    }
}

LRESULT CALLBACK AppController::AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    auto* app = reinterpret_cast<AppController*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (msg == WM_APP_ACTION && app) {
        app->DrainActions();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool AppController::Initialize(const std::wstring& cmdLine) {
    // 1. Initialize SQLite Database
    if (!m_db.Init()) {
        MessageBoxW(nullptr, L"无法初始化 SQLite 数据库", L"错误", MB_ICONERROR);
        return false;
    }

    // 2. Register hidden app controller window
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &AppController::AppWndProc;
    wc.hInstance = m_instance;
    wc.lpszClassName = kAppClass;
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    m_appWnd = CreateWindowExW(0, kAppClass, L"AiPwdController", WS_OVERLAPPED,
                               0, 0, 0, 0, nullptr, nullptr, m_instance, this);
    if (!m_appWnd) {
        return false;
    }

    // 3. Restore persisted theme preference
    std::string savedTheme = m_db.GetSetting("theme", "blue");
    if (savedTheme == "purple") m_state.theme = ThemeColor::Purple;
    else if (savedTheme == "cyan") m_state.theme = ThemeColor::Cyan;
    else if (savedTheme == "emerald") m_state.theme = ThemeColor::Emerald;
    else if (savedTheme == "amber") m_state.theme = ThemeColor::Amber;
    else m_state.theme = ThemeColor::Blue;

    // 4. Initial generator evaluation
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

    // 5. Create main Host UI
    RecreateHost();
    return true;
}

int AppController::Run() {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (m_host && !m_host->IsClosed() && m_host->ProcessMessage(msg)) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (m_host && m_host->IsClosed()) {
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
    int x = 80, y = 80, w = 1180, h = 820;
    bool hasPos = false;
    if (m_host && !m_host->IsClosed()) {
        if (HWND hwnd = m_host->GetHwnd()) {
            RECT r{};
            if (GetWindowRect(hwnd, &r)) {
                x = r.left;
                y = r.top;
                w = (std::max)(1000L, r.right - r.left);
                h = (std::max)(640L, r.bottom - r.top);
                hasPos = true;
            }
        }
    }

    auto items = m_db.QueryItems(m_state.searchQuery, m_state.selectedCategory, (m_state.activeTab == ActiveTab::Favorites));
    m_state.stats = m_db.GetStats();

    std::string xml = LayoutBuilder::BuildMainLayout(m_state, items);

    const std::wstring sizeStr = std::to_wstring(w) + L"x" + std::to_wstring(h);
    SetEnvironmentVariableW(L"AI_WIN_UI_CHROME", L"custom");
    SetEnvironmentVariableW(L"AI_WIN_UI_SIZE", sizeStr.c_str());

    ai_win_ui::HostCreateInfo info{};
    info.inlineLayoutXml = xml.c_str();
    info.chrome = L"custom";
    info.ownMessagePump = false;
    info.onAction = &AppController::HostActionThunk;
    info.actionUser = this;
    info.showCommand = SW_HIDE;
    info.size = sizeStr.c_str();
    if (hasPos) {
        info.hasPosition = true;
        info.posX = x;
        info.posY = y;
    }

    auto newHost = ai_win_ui::Host::Create(m_instance, info);
    if (newHost) {
        HWND newHwnd = newHost->GetHwnd();
        if (newHwnd) {
            // 1. Force DWM to treat window as Dark Mode to remove any white border
            BOOL useDarkMode = TRUE;
            DwmSetWindowAttribute(newHwnd, 20, &useDarkMode, sizeof(useDarkMode));
            DwmSetWindowAttribute(newHwnd, 19, &useDarkMode, sizeof(useDarkMode));

            // 2. Windows 11 DWM rounded corners preference (DWMWA_WINDOW_CORNER_PREFERENCE = 33, DWMWCP_ROUND = 2)
            DWORD cornerPreference = 2; // DWMWCP_ROUND
            DwmSetWindowAttribute(newHwnd, 33, &cornerPreference, sizeof(cornerPreference));

            // 3. Theme-driven DWM native border color
            ThemePalette curPalette = GetThemePalette(m_state.theme);
            COLORREF themeDwmBorder = static_cast<COLORREF>(curPalette.dwmBorderColor);
            DwmSetWindowAttribute(newHwnd, 34 /* DWMWA_BORDER_COLOR */, &themeDwmBorder, sizeof(themeDwmBorder));

            // 4. Set window class background brush to match theme canvas so it never paints white or flickers
            static HBRUSH s_lastBrush = nullptr;
            HBRUSH newBrush = CreateSolidBrush(static_cast<COLORREF>(curPalette.classBgColor));
            SetClassLongPtrW(newHwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(newBrush));
            if (s_lastBrush) {
                DeleteObject(s_lastBrush);
            }
            s_lastBrush = newBrush;

            SetWindowPos(newHwnd, nullptr, x, y, w, h, (hasPos ? 0 : SWP_NOMOVE) | SWP_NOZORDER);

            // 5. Apply rounded corner window region (16px radius) and hook dynamic resize/maximize
            if (!IsZoomed(newHwnd)) {
                HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, 16, 16);
                SetWindowRgn(newHwnd, rgn, TRUE);
            }
            SetWindowSubclass(newHwnd, HostSubclassProc, 1001, 0);

            HICON hIcon = LoadIconW(m_instance, MAKEINTRESOURCEW(IDI_APP_ICON));
            if (hIcon) {
                SendMessageW(newHwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
                SendMessageW(newHwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
            }
        }
        m_host = std::move(newHost);
        if (newHwnd) {
            ShowWindow(newHwnd, SW_SHOW);
            UpdateWindow(newHwnd);
        }
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
    if (id == "windowMinimize") {
        if (m_host && m_host->GetHwnd()) {
            ShowWindow(m_host->GetHwnd(), SW_MINIMIZE);
        }
        return;
    }
    if (id == "windowMaximize") {
        if (m_host && m_host->GetHwnd()) {
            HWND hwnd = m_host->GetHwnd();
            ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
        }
        return;
    }
    if (id == "windowClose") {
        PostQuitMessage(0);
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
        HWND hwnd = m_host ? m_host->GetHwnd() : nullptr;
        CopyTextToClipboard(hwnd, m_state.currentAssessment.password);
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
            CopyTextToClipboard(m_host ? m_host->GetHwnd() : nullptr, it.username);
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
            CopyTextToClipboard(m_host ? m_host->GetHwnd() : nullptr, it.password);
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
