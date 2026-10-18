#pragma once

#include <Windows.h>
#include <shellapi.h>
#include <memory>
#include <string>
#include <vector>
#include <ai_win_ui/host.h>

#include "../db/vault_db.h"
#include "../generator/pwd_generator.h"
#include "../ui/layout_builder.h"

namespace pwd {

class AppController {
public:
    explicit AppController(HINSTANCE instance);
    ~AppController();

    bool Initialize(const std::wstring& cmdLine = L"");
    int Run();

    void EnqueueAction(const std::string& actionId);
    void DrainActions();

    void InitTrayIcon();
    void RemoveTrayIcon();
    void ShowTrayMenu();

    static constexpr UINT WM_TRAYICON = WM_USER + 101;
    static constexpr UINT ID_TRAY_SHOW = 2001;
    static constexpr UINT ID_TRAY_GEN = 2002;
    static constexpr UINT ID_TRAY_VAULT = 2003;
    static constexpr UINT ID_TRAY_EXIT = 2004;

private:
    static LRESULT CALLBACK ShellWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK ChildHostSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static void HostActionThunk(const char* actionId, void* user);

    void HandleAction(const std::string& id);
    void RecreateHost();
    void UpdateThemeDwmAttributes();
    void PopulateFormFields();
    void ShowToast(const std::wstring& msg, const std::string& color = "#10B981");

    HINSTANCE m_instance = nullptr;
    HWND m_shellWnd = nullptr;

    NOTIFYICONDATAW m_nid{};
    bool m_trayAdded = false;
    UINT m_taskbarCreatedMsg = 0;

    std::unique_ptr<ai_win_ui::Host> m_host;
    VaultDb m_db;
    AppUiState m_state;
    std::vector<std::string> m_pendingActions;
};

} // namespace pwd
