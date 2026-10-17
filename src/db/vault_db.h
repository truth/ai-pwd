#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <sqlite3.h>

namespace pwd {

struct VaultItem {
    int64_t id = 0;
    std::wstring title;
    std::wstring category; // e.g. "社交", "工作", "金融", "邮箱", "其它"
    std::wstring username;
    std::wstring password;
    std::wstring url;
    std::wstring notes;
    bool favorite = false;
    int64_t createdAt = 0; // Unix timestamp
    int64_t updatedAt = 0;
};

struct VaultStats {
    int totalCount = 0;
    int favCount = 0;
    int weakCount = 0;
};

class VaultDb {
public:
    VaultDb();
    ~VaultDb();

    // Initialize database. If dbPath is empty, automatically use %APPDATA%/ai-pwd/vault.db
    bool Init(const std::wstring& dbPath = L"");

    std::vector<VaultItem> QueryItems(const std::wstring& keyword = L"",
                                      const std::wstring& category = L"",
                                      bool onlyFavorites = false);

    bool GetById(int64_t id, VaultItem& outItem);
    int64_t AddItem(const VaultItem& item);
    bool UpdateItem(const VaultItem& item);
    bool DeleteItem(int64_t id);
    bool ToggleFavorite(int64_t id);
    VaultStats GetStats();

    const std::wstring& GetDbPath() const { return m_dbPath; }

private:
    void SeedInitialData();
    std::wstring ResolveDefaultDbPath();

    sqlite3* m_db = nullptr;
    std::wstring m_dbPath;
};

// UTF conversion utilities
std::string WideToUtf8(const std::wstring& w);
std::wstring Utf8ToWide(const std::string& s);

} // namespace pwd
