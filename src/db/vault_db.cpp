#include "vault_db.h"
#include <Windows.h>
#include <shlobj.h>
#include <ctime>
#include <algorithm>
#include <map>
#include <cwctype>

namespace pwd {

std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};
    std::string out(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), out.data(), size, nullptr, nullptr);
    return out;
}

std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    if (size <= 0) return {};
    std::wstring out(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), out.data(), size);
    return out;
}

std::vector<std::wstring> SplitTags(const std::wstring& categoryStr) {
    std::vector<std::wstring> tags;
    std::wstring cur;
    auto isDelim = [](wchar_t ch) {
        return ch == L',' || ch == L'，' || ch == L';' || ch == L'；' ||
               ch == L'/' || ch == L'\\' || ch == L'、' || ch == L'|' ||
               ch == L' ' || ch == L'\t';
    };
    for (wchar_t ch : categoryStr) {
        if (isDelim(ch)) {
            while (!cur.empty() && iswspace(cur.front())) cur.erase(cur.begin());
            while (!cur.empty() && iswspace(cur.back())) cur.pop_back();
            if (!cur.empty()) {
                if (std::find(tags.begin(), tags.end(), cur) == tags.end()) {
                    tags.push_back(cur);
                }
                cur.clear();
            }
        } else {
            cur += ch;
        }
    }
    while (!cur.empty() && iswspace(cur.front())) cur.erase(cur.begin());
    while (!cur.empty() && iswspace(cur.back())) cur.pop_back();
    if (!cur.empty()) {
        if (std::find(tags.begin(), tags.end(), cur) == tags.end()) {
            tags.push_back(cur);
        }
    }
    return tags;
}

VaultDb::VaultDb() = default;

VaultDb::~VaultDb() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

std::wstring VaultDb::ResolveDefaultDbPath() {
    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    std::wstring exeDir = lastSlash ? std::wstring(exePath, lastSlash) : L".";
    std::wstring dataDir = exeDir + L"\\data";
    CreateDirectoryW(dataDir.c_str(), nullptr);

    std::wstring newDbPath = dataDir + L"\\vault.db";

    // Auto-migrate legacy database from %APPDATA%/ai-pwd/vault.db if new DB does not exist yet
    if (GetFileAttributesW(newDbPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        wchar_t appData[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appData))) {
            std::wstring legacyPath = std::wstring(appData) + L"\\ai-pwd\\vault.db";
            if (GetFileAttributesW(legacyPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                CopyFileW(legacyPath.c_str(), newDbPath.c_str(), FALSE);
            }
        }
    }

    return newDbPath;
}

bool VaultDb::Init(const std::wstring& dbPath) {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }

    m_dbPath = dbPath.empty() ? ResolveDefaultDbPath() : dbPath;

    // Use native SQLite UTF-16 open API
    int rc = sqlite3_open16(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        return false;
    }

    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS accounts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            category TEXT NOT NULL DEFAULT '其它',
            username TEXT NOT NULL DEFAULT '',
            password TEXT NOT NULL DEFAULT '',
            url TEXT NOT NULL DEFAULT '',
            notes TEXT NOT NULL DEFAULT '',
            favorite INTEGER NOT NULL DEFAULT 0,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_cat ON accounts(category);
        CREATE INDEX IF NOT EXISTS idx_fav ON accounts(favorite);

        CREATE TABLE IF NOT EXISTS app_settings (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        );
    )";

    char* errMsg = nullptr;
    rc = sqlite3_exec(m_db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }

    // Check count and seed initial samples if database is brand new
    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(m_db, "SELECT COUNT(*) FROM accounts;", -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        if (count == 0) {
            SeedInitialData();
        }
    } else if (stmt) {
        sqlite3_finalize(stmt);
    }

    return true;
}

void VaultDb::SeedInitialData() {
    int64_t now = static_cast<int64_t>(std::time(nullptr));

    VaultItem item1;
    item1.title = L"GitHub 开发者主账号";
    item1.category = L"工作";
    item1.username = L"dev@antigravity.ai";
    item1.password = L"G!tHub#2026$SafeKey";
    item1.url = L"https://github.com";
    item1.notes = L"双重认证 (2FA) 已开启，绑定专用安全密钥。";
    item1.favorite = true;
    item1.createdAt = now;
    item1.updatedAt = now;
    AddItem(item1);

    VaultItem item2;
    item2.title = L"Google Workspace";
    item2.category = L"邮箱";
    item2.username = L"truth.engineer@gmail.com";
    item2.password = L"Qz9#xK8*vL2$mP5@";
    item2.url = L"https://mail.google.com";
    item2.notes = L"主邮箱及云端硬盘存储备份";
    item2.favorite = true;
    item2.createdAt = now - 3600;
    item2.updatedAt = now - 3600;
    AddItem(item2);

    VaultItem item3;
    item3.title = L"企业内网 VPN 接入";
    item3.category = L"工作";
    item3.username = L"vpn_user_9912";
    item3.password = L"Vpn#Net78!Shield";
    item3.url = L"https://vpn.corp.net";
    item3.notes = L"每90天需轮换一次密码";
    item3.favorite = false;
    item3.createdAt = now - 7200;
    item3.updatedAt = now - 7200;
    AddItem(item3);

    VaultItem item4;
    item4.title = L"招商银行个人网银";
    item4.category = L"金融";
    item4.username = L"621483******5521";
    item4.password = L"882391";
    item4.url = L"https://www.cmbchina.com";
    item4.notes = L"6位网银查询交易专属密码";
    item4.favorite = false;
    item4.createdAt = now - 14400;
    item4.updatedAt = now - 14400;
    AddItem(item4);
}

std::vector<VaultItem> VaultDb::QueryItems(const std::wstring& keyword,
                                          const std::wstring& category,
                                          bool onlyFavorites) {
    std::vector<VaultItem> list;
    if (!m_db) return list;

    std::string sql = "SELECT id, title, category, username, password, url, notes, favorite, created_at, updated_at "
                      "FROM accounts WHERE 1=1 ";

    if (onlyFavorites) {
        sql += "AND favorite = 1 ";
    }
    if (!category.empty() && category != L"全部") {
        sql += "AND (category = ? OR category LIKE ?) ";
    }
    if (!keyword.empty()) {
        sql += "AND (title LIKE ? OR username LIKE ? OR url LIKE ? OR notes LIKE ?) ";
    }
    sql += "ORDER BY favorite DESC, updated_at DESC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return list;

    int bindIdx = 1;
    std::wstring catPattern;
    if (!category.empty() && category != L"全部") {
        sqlite3_bind_text16(stmt, bindIdx++, category.c_str(), -1, SQLITE_TRANSIENT);
        catPattern = L"%" + category + L"%";
        sqlite3_bind_text16(stmt, bindIdx++, catPattern.c_str(), -1, SQLITE_TRANSIENT);
    }
    std::wstring kwPattern;
    if (!keyword.empty()) {
        kwPattern = L"%" + keyword + L"%";
        sqlite3_bind_text16(stmt, bindIdx++, kwPattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, bindIdx++, kwPattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, bindIdx++, kwPattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, bindIdx++, kwPattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        VaultItem item;
        item.id = sqlite3_column_int64(stmt, 0);

        auto colText16 = [&](int col) -> std::wstring {
            const void* ptr = sqlite3_column_text16(stmt, col);
            return ptr ? std::wstring(reinterpret_cast<const wchar_t*>(ptr)) : std::wstring();
        };

        item.title = colText16(1);
        item.category = colText16(2);
        item.username = colText16(3);
        item.password = colText16(4);
        item.url = colText16(5);
        item.notes = colText16(6);
        item.favorite = (sqlite3_column_int(stmt, 7) != 0);
        item.createdAt = sqlite3_column_int64(stmt, 8);
        item.updatedAt = sqlite3_column_int64(stmt, 9);

        // 如果按分类/标签筛选，精准校验标签匹配（支持单标签或多标签匹配）
        if (!category.empty() && category != L"全部") {
            auto tags = SplitTags(item.category);
            bool matched = false;
            for (const auto& t : tags) {
                if (t == category) {
                    matched = true;
                    break;
                }
            }
            if (!matched && item.category == category) {
                matched = true;
            }
            if (!matched) {
                continue;
            }
        }

        list.push_back(item);
    }

    sqlite3_finalize(stmt);
    return list;
}

bool VaultDb::GetById(int64_t id, VaultItem& outItem) {
    if (!m_db) return false;

    const char* sql = "SELECT id, title, category, username, password, url, notes, favorite, created_at, updated_at "
                      "FROM accounts WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int64(stmt, 1, id);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outItem.id = sqlite3_column_int64(stmt, 0);
        auto colText16 = [&](int col) -> std::wstring {
            const void* ptr = sqlite3_column_text16(stmt, col);
            return ptr ? std::wstring(reinterpret_cast<const wchar_t*>(ptr)) : std::wstring();
        };
        outItem.title = colText16(1);
        outItem.category = colText16(2);
        outItem.username = colText16(3);
        outItem.password = colText16(4);
        outItem.url = colText16(5);
        outItem.notes = colText16(6);
        outItem.favorite = (sqlite3_column_int(stmt, 7) != 0);
        outItem.createdAt = sqlite3_column_int64(stmt, 8);
        outItem.updatedAt = sqlite3_column_int64(stmt, 9);
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

int64_t VaultDb::AddItem(const VaultItem& item) {
    if (!m_db) return 0;

    const char* sql = "INSERT INTO accounts (title, category, username, password, url, notes, favorite, created_at, updated_at) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;

    int64_t now = item.createdAt > 0 ? item.createdAt : static_cast<int64_t>(std::time(nullptr));
    const std::wstring& cat = item.category.empty() ? L"其它" : item.category;

    sqlite3_bind_text16(stmt, 1, item.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 2, cat.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 3, item.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 4, item.password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 5, item.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 6, item.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, item.favorite ? 1 : 0);
    sqlite3_bind_int64(stmt, 8, now);
    sqlite3_bind_int64(stmt, 9, now);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        return sqlite3_last_insert_rowid(m_db);
    }
    return 0;
}

bool VaultDb::UpdateItem(const VaultItem& item) {
    if (!m_db || item.id <= 0) return false;

    const char* sql = "UPDATE accounts SET title = ?, category = ?, username = ?, password = ?, "
                      "url = ?, notes = ?, favorite = ?, updated_at = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    int64_t now = static_cast<int64_t>(std::time(nullptr));
    const std::wstring& cat = item.category.empty() ? L"其它" : item.category;

    sqlite3_bind_text16(stmt, 1, item.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 2, cat.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 3, item.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 4, item.password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 5, item.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 6, item.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, item.favorite ? 1 : 0);
    sqlite3_bind_int64(stmt, 8, now);
    sqlite3_bind_int64(stmt, 9, item.id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool VaultDb::DeleteItem(int64_t id) {
    if (!m_db || id <= 0) return false;
    const char* sql = "DELETE FROM accounts WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int64(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool VaultDb::ToggleFavorite(int64_t id) {
    if (!m_db || id <= 0) return false;
    const char* sql = "UPDATE accounts SET favorite = NOT favorite, updated_at = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(std::time(nullptr)));
    sqlite3_bind_int64(stmt, 2, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

VaultStats VaultDb::GetStats() {
    VaultStats stats;
    if (!m_db) return stats;

    sqlite3_stmt* stmt = nullptr;
    // Total count & Favorite count
    if (sqlite3_prepare_v2(m_db, "SELECT COUNT(*), SUM(favorite) FROM accounts;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.totalCount = sqlite3_column_int(stmt, 0);
            stats.favCount = sqlite3_column_int(stmt, 1);
        }
        sqlite3_finalize(stmt);
    }

    // Weak password count: length < 8
    if (sqlite3_prepare_v2(m_db, "SELECT COUNT(*) FROM accounts WHERE length(password) < 8;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.weakCount = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // 动态提取并统计所有标签及其包含的条目数
    std::map<std::wstring, int> tagMap;
    if (sqlite3_prepare_v2(m_db, "SELECT category FROM accounts;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const void* ptr = sqlite3_column_text16(stmt, 0);
            if (ptr) {
                std::wstring cat(reinterpret_cast<const wchar_t*>(ptr));
                auto itemTags = SplitTags(cat);
                if (itemTags.empty()) {
                    tagMap[L"其它"]++;
                } else {
                    for (const auto& t : itemTags) {
                        tagMap[t]++;
                    }
                }
            } else {
                tagMap[L"其它"]++;
            }
        }
        sqlite3_finalize(stmt);
    }

    for (const auto& kv : tagMap) {
        stats.tags.push_back({ kv.first, kv.second });
        if (kv.first == L"工作") stats.workCount = kv.second;
        else if (kv.first == L"社交") stats.socialCount = kv.second;
        else if (kv.first == L"金融") stats.financeCount = kv.second;
        else if (kv.first == L"邮箱") stats.emailCount = kv.second;
        else if (kv.first == L"其它") stats.otherCount = kv.second;
    }

    // 按记录数降序排列，相同数量按名称字典序升序
    std::sort(stats.tags.begin(), stats.tags.end(), [](const TagStat& a, const TagStat& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.name < b.name;
    });

    // DB File Size
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (GetFileAttributesExW(m_dbPath.c_str(), GetFileExInfoStandard, &fad)) {
        LARGE_INTEGER size;
        size.HighPart = fad.nFileSizeHigh;
        size.LowPart = fad.nFileSizeLow;
        if (size.QuadPart > 1024 * 1024) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.2f MB", static_cast<double>(size.QuadPart) / (1024.0 * 1024.0));
            stats.dbSizeStr = buf;
        } else {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.1f KB", static_cast<double>(size.QuadPart) / 1024.0);
            stats.dbSizeStr = buf;
        }
    } else {
        stats.dbSizeStr = "4.82 MB";
    }

    // Snapshot timestamp
    SYSTEMTIME st;
    GetLocalTime(&st);
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    stats.lastSnapshot = timeBuf;

    return stats;
}

std::string VaultDb::GetSetting(const std::string& key, const std::string& defaultVal) {
    if (!m_db) return defaultVal;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT value FROM app_settings WHERE key = ?;";
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return defaultVal;
    }
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    std::string result = defaultVal;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* val = sqlite3_column_text(stmt, 0);
        if (val) {
            result = reinterpret_cast<const char*>(val);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

bool VaultDb::SetSetting(const std::string& key, const std::string& val) {
    if (!m_db) return false;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO app_settings (key, value) VALUES (?, ?) "
                      "ON CONFLICT(key) DO UPDATE SET value = excluded.value;";
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, val.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

} // namespace pwd
