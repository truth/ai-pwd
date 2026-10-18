#include <Windows.h>
#include "app/app_controller.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR pCmdLine, int cmdShow) {
    // Enable modern Per-Monitor V2 DPI Awareness for crisp multi-monitor rendering
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Ensure custom chrome and 1180x820 from process start to prevent resize animations
    SetEnvironmentVariableW(L"AI_WIN_UI_CHROME", L"custom");
    SetEnvironmentVariableW(L"AI_WIN_UI_SIZE", L"1180x820");

    pwd::AppController app(instance);
    if (!app.Initialize(pCmdLine ? pCmdLine : L"")) {
        MessageBoxW(nullptr, L"初始化应用程序失败！", L"错误", MB_ICONERROR);
        return -1;
    }

    return app.Run();
}
