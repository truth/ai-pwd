#pragma once

#include <Windows.h>
#include <string>

namespace pwd {

inline bool CopyTextToClipboard(HWND hwnd, const std::wstring& text) {
    if (text.empty()) return false;

    if (!OpenClipboard(hwnd)) {
        return false;
    }
    EmptyClipboard();

    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    void* pMem = GlobalLock(hMem);
    if (pMem) {
        memcpy(pMem, text.data(), bytes);
        GlobalUnlock(hMem);
        SetClipboardData(CF_UNICODETEXT, hMem);
    }

    CloseClipboard();
    return true;
}

} // namespace pwd
