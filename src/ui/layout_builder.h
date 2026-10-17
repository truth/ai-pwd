#pragma once

#include <string>
#include <vector>
#include <set>
#include "../db/vault_db.h"
#include "../generator/pwd_generator.h"

namespace pwd {

enum class ActiveTab {
    Generator,
    Vault,
    Favorites
};

enum class ThemeColor {
    Blue,      // 经典科技蓝
    Purple,    // 极客霓虹紫
    Cyan,      // 赛博极光青
    Emerald,   // 安全翡翠绿
    Amber      // 流金琥珀橙
};

struct ThemePalette {
    ThemeColor id = ThemeColor::Blue;
    std::string name;
    std::string primary;         // #2563EB
    std::string primaryHover;    // #1D4ED8
    std::string primaryGlow;     // #1E40AF
    std::string accent;          // #38BDF8
    std::string dotColor;        // #3B82F6
    std::string borderAccent;    // #2563EB
    std::string badgeBg;         // #1E3A8A44
    std::string textAccent;      // #60A5FA
};

ThemePalette GetThemePalette(ThemeColor color);

struct AppUiState {
    ActiveTab activeTab = ActiveTab::Generator;
    std::wstring selectedCategory = L"全部";
    std::wstring searchQuery = L"";

    // Theme state
    ThemeColor theme = ThemeColor::Blue;

    // Generator state
    GeneratorOptions genOptions;
    PasswordAssessment currentAssessment;
    bool generatorRevealed = true;

    // Vault view state
    std::set<int64_t> revealedPasswords;

    // Edit modal state
    bool isEditing = false;
    bool isNewItem = false;
    VaultItem editingItem;

    // Toast banner
    std::wstring toastMessage;
    std::string toastColor = "#10B981";

    // Overall stats
    VaultStats stats;
};

class LayoutBuilder {
public:
    static std::string BuildMainLayout(const AppUiState& state, const std::vector<VaultItem>& items);

    static std::string XmlEscape(const std::string& str);
    static std::string XmlEscape(const std::wstring& wstr);
};

} // namespace pwd
