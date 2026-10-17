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

struct AppUiState {
    ActiveTab activeTab = ActiveTab::Generator;
    std::wstring selectedCategory = L"全部";
    std::wstring searchQuery = L"";

    // Generator state
    GeneratorOptions genOptions;
    PasswordAssessment currentAssessment;

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
