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

enum class VaultViewMode {
    Card, // 卡片视图
    List  // 列表视图
};

enum class ThemeColor {
    Blue,      // 经典科技蓝
    Purple,    // 极客霓虹紫
    Cyan,      // 赛博极光青
    Emerald,   // 安全翡翠绿
    Amber,     // 流金琥珀橙
    Light      // 优雅明亮白
};

struct ThemePalette {
    ThemeColor id = ThemeColor::Blue;
    std::string name;

    // Brand accents
    std::string primary;         // #2563EB
    std::string primaryHover;    // #1D4ED8
    std::string primaryGlow;     // #1E40AF
    std::string accent;          // #38BDF8
    std::string dotColor;        // #3B82F6
    std::string borderAccent;    // #2563EB
    std::string badgeBg;         // #172554
    std::string textAccent;      // #60A5FA

    // Surface / Background tokens
    std::string windowBg;        // Root canvas background
    std::string titleBarBg;      // Top custom caption bar background
    std::string sidebarBg;       // Left navigation panel background
    std::string footerBg;        // Bottom status bar background
    std::string cardBg;          // Main workspace cards (generator card, vault card, edit card)
    std::string cardBorder;      // Card border stroke
    std::string inputBg;         // Inputs, display box, item sub-boxes
    std::string inputBorder;     // Border for inputs
    std::string innerCardBg;     // Inner container
    std::string btnSecondaryBg;  // Secondary/action buttons
    std::string btnSecondaryHover;
    std::string pillActiveBg;    // Active toggle/pill
    std::string pillInactiveBg;  // Inactive toggle/pill
    std::string catSelectedBg;   // Selected category item background
    std::string statusCardBg;    // SQLite monitor card background

    // Semantic Text tokens (Light vs Dark mode)
    std::string textPrimary = "#F8FAFC";
    std::string textSecondary = "#94A3B8";
    std::string textMuted = "#64748B";
    std::string textInput = "#F8FAFC";
    std::string btnSecondaryFg = "#E2E8F0";

    // Windows Native DWM & GDI attributes
    unsigned long dwmBorderColor = 0; // COLORREF
    unsigned long classBgColor = 0;   // COLORREF
};

ThemePalette GetThemePalette(ThemeColor color);

struct AppUiState {
    ActiveTab activeTab = ActiveTab::Generator;
    std::wstring selectedCategory = L"全部";
    std::wstring searchQuery = L"";

    // View mode: Card or List
    VaultViewMode viewMode = VaultViewMode::Card;

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
