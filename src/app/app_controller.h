#pragma once

#include <Windows.h>
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

    std::unique_ptr<ai_win_ui::Host> m_host;
    VaultDb m_db;
    AppUiState m_state;
    std::vector<std::string> m_pendingActions;
};

} // namespace pwd
