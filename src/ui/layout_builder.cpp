#include "layout_builder.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace pwd {

std::string LayoutBuilder::XmlEscape(const std::string& str) {
    std::string out;
    out.reserve(str.size() * 11 / 10);
    for (char c : str) {
        switch (c) {
            case '&':  out += "&amp;"; break;
            case '<':  out += "&lt;"; break;
            case '>':  out += "&gt;"; break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default:   out += c; break;
        }
    }
    return out;
}

std::string LayoutBuilder::XmlEscape(const std::wstring& wstr) {
    return XmlEscape(WideToUtf8(wstr));
}

namespace {

std::string FormatTimestamp(int64_t ts) {
    if (ts <= 0) return "";
    std::time_t t = static_cast<std::time_t>(ts);
    std::tm tm_{};
    localtime_s(&tm_, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm_);
    return buf;
}

std::string GetCategoryColor(const std::wstring& cat) {
    if (cat == L"工作") return "#8B5CF6";     // Purple
    if (cat == L"社交") return "#3B82F6";     // Blue
    if (cat == L"金融") return "#10B981";     // Emerald
    if (cat == L"邮箱") return "#F59E0B";     // Amber
    if (cat == L"服务器") return "#EC4899";   // Pink
    return "#64748B";                         // Slate
}

} // namespace

std::string LayoutBuilder::BuildMainLayout(const AppUiState& state, const std::vector<VaultItem>& items) {
    std::string xml;
    xml.reserve(24576);

    // Root window with custom chrome
    xml += "<Panel chrome=\"custom\" direction=\"column\" background=\"#0B0F17\" padding=\"0,0,0,0\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // 1. Top Custom Title Bar (48px)
    // -------------------------------------------------------------
    xml += "<Panel role=\"titleBar\" hitTest=\"caption\" direction=\"row\" height=\"48\" background=\"#111827\" padding=\"14,0,10,0\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
    
    // Left: Logo & Title
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"10\" hitTest=\"caption\" flexGrow=\"1\">";
    xml += "<Panel width=\"26\" height=\"26\" background=\"#2563EB\" cornerRadius=\"6\" alignItems=\"center\" justifyContent=\"center\">";
    xml += "<Label text=\"AI\" fontSize=\"12\" color=\"#FFFFFF\" fontWeight=\"bold\" width=\"20\" />";
    xml += "</Panel>";
    xml += "<Label text=\"AI 密码生成器与保险库 (AI-Pwd)\" fontSize=\"14\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"260\" />";
    xml += "<Panel background=\"#1E293B\" cornerRadius=\"4\" padding=\"2,8,2,8\" margin=\"0,0,0,4\">";
    xml += "<Label name=\"app_status_badge\" text=\"SQLite 本地加密存储\" fontSize=\"11\" color=\"#38BDF8\" width=\"200\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // Left

    // Right: Minimize / Maximize / Close Buttons
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"2\" hitTest=\"client\" width=\"116\">";
    xml += "<Button variant=\"caption-minimize\" width=\"36\" height=\"32\" hitTest=\"client\" onClick=\"windowMinimize\" />";
    xml += "<Button variant=\"caption-maximize\" width=\"36\" height=\"32\" hitTest=\"client\" onClick=\"windowMaximize\" />";
    xml += "<Button variant=\"caption-close\" width=\"40\" height=\"32\" hitTest=\"client\" onClick=\"windowClose\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // Title Bar

    // -------------------------------------------------------------
    // 2. Main Body: Left Sidebar + Right Content Area
    // -------------------------------------------------------------
    xml += "<Panel direction=\"row\" flexGrow=\"1\" background=\"#0B0F17\" padding=\"0,0,0,0\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // Left Sidebar (230px)
    // -------------------------------------------------------------
    xml += "<Panel direction=\"column\" width=\"230\" background=\"#111827\" padding=\"16,14,16,14\" spacing=\"14\" alignItems=\"stretch\">";

    // Primary Nav Section
    xml += "<Panel direction=\"column\" spacing=\"6\" alignItems=\"stretch\">";
    xml += "<Label text=\"核心导航\" fontSize=\"11\" color=\"#64748B\" fontWeight=\"bold\" margin=\"0,0,4,4\" width=\"120\" />";

    // Tab 1: Generator
    bool isGen = (state.activeTab == ActiveTab::Generator && !state.isEditing);
    std::string genBg = isGen ? "#2563EB" : "transparent";
    std::string genFg = isGen ? "#FFFFFF" : "#94A3B8";
    xml += "<Button text=\"⚡  密码生成器\" width=\"202\" height=\"38\" fontSize=\"13\" background=\"" + genBg + "\" foreground=\"" + genFg + "\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:generator\" />";

    // Tab 2: Vault
    bool isVault = (state.activeTab == ActiveTab::Vault && !state.isEditing);
    std::string vaultBg = isVault ? "#2563EB" : "transparent";
    std::string vaultFg = isVault ? "#FFFFFF" : "#94A3B8";
    xml += "<Button text=\"❖  密码保险库\" width=\"202\" height=\"38\" fontSize=\"13\" background=\"" + vaultBg + "\" foreground=\"" + vaultFg + "\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:vault\" />";

    // Tab 3: Favorites
    bool isFav = (state.activeTab == ActiveTab::Favorites && !state.isEditing);
    std::string favBg = isFav ? "#2563EB" : "transparent";
    std::string favFg = isFav ? "#FFFFFF" : "#94A3B8";
    xml += "<Button text=\"★  收藏星标项\" width=\"202\" height=\"38\" fontSize=\"13\" background=\"" + favBg + "\" foreground=\"" + favFg + "\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:favorites\" />";
    xml += "</Panel>";

    // Categories Section
    xml += "<Panel direction=\"column\" spacing=\"4\" alignItems=\"stretch\" margin=\"8,0,0,0\">";
    xml += "<Label text=\"分类快速筛选\" fontSize=\"11\" color=\"#64748B\" fontWeight=\"bold\" margin=\"0,0,4,4\" width=\"120\" />";

    const wchar_t* categories[] = { L"全部", L"工作", L"社交", L"金融", L"邮箱", L"其它" };
    for (const wchar_t* cat : categories) {
        bool selected = (state.selectedCategory == cat);
        std::string catBg = selected ? "#1E293B" : "transparent";
        std::string catFg = selected ? "#38BDF8" : "#94A3B8";
        std::string label = "•  " + WideToUtf8(cat);
        xml += "<Button text=\"" + XmlEscape(label) + "\" width=\"202\" height=\"32\" fontSize=\"12\" background=\"" + catBg + "\" foreground=\"" + catFg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"cat:" + XmlEscape(cat) + "\" />";
    }
    xml += "</Panel>";

    // Spacer
    xml += "<Panel flexGrow=\"1\" />";

    // Sidebar Bottom Info Card
    xml += "<Panel direction=\"column\" background=\"#1E293B\" cornerRadius=\"10\" padding=\"12,12,12,12\" spacing=\"8\" alignItems=\"stretch\">";
    xml += "<Label text=\"SQLite 存储监控\" fontSize=\"12\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"140\" />";
    xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Label text=\"密码总记录\" fontSize=\"12\" color=\"#94A3B8\" width=\"80\" />";
    xml += "<Label text=\"" + std::to_string(state.stats.totalCount) + " 条\" fontSize=\"12\" color=\"#38BDF8\" fontWeight=\"bold\" width=\"60\" />";
    xml += "</Panel>";
    xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Label text=\"已收藏项\" fontSize=\"12\" color=\"#94A3B8\" width=\"80\" />";
    xml += "<Label text=\"" + std::to_string(state.stats.favCount) + " 项\" fontSize=\"12\" color=\"#FBBF24\" width=\"60\" />";
    xml += "</Panel>";
    if (state.stats.weakCount > 0) {
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"弱密码预警\" fontSize=\"12\" color=\"#94A3B8\" width=\"80\" />";
        xml += "<Label text=\"" + std::to_string(state.stats.weakCount) + " 个\" fontSize=\"12\" color=\"#EF4444\" width=\"60\" />";
        xml += "</Panel>";
    }
    xml += "<Button text=\"+ 新建密码记录\" width=\"178\" height=\"32\" fontSize=\"12\" background=\"#059669\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:open_add\" margin=\"4,0,0,0\" />";
    xml += "</Panel>";

    xml += "</Panel>"; // Left Sidebar

    // -------------------------------------------------------------
    // Right Content Area
    // -------------------------------------------------------------
    xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"#0B0F17\" padding=\"16,22,16,22\" spacing=\"14\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // Toast Feedback Banner (if message present)
    // -------------------------------------------------------------
    if (!state.toastMessage.empty()) {
        xml += "<Panel direction=\"row\" height=\"38\" background=\"#1E293B\" cornerRadius=\"8\" padding=\"0,16,0,16\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
        xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Panel width=\"8\" height=\"8\" background=\"" + state.toastColor + "\" cornerRadius=\"4\" />";
        xml += "<Label text=\"" + XmlEscape(state.toastMessage) + "\" fontSize=\"13\" color=\"#F8FAFC\" width=\"400\" />";
        xml += "</Panel>";
        xml += "<Button text=\"×\" width=\"24\" height=\"24\" fontSize=\"14\" background=\"transparent\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"toast:dismiss\" />";
        xml += "</Panel>";
    }

    // -------------------------------------------------------------
    // Content Views: Edit Modal OR Generator View OR Vault View
    // -------------------------------------------------------------
    if (state.isEditing) {
        // =========================================================
        // VIEW: Add / Edit Account Form
        // =========================================================
        std::string formTitle = state.isNewItem ? "新建密码记录" : "编辑密码记录";
        xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"#161B22\" cornerRadius=\"12\" padding=\"20,24,20,24\" spacing=\"14\" alignItems=\"stretch\">";
        
        xml += "<Panel direction=\"row\" alignItems=\"center\" justifyContent=\"spaceBetween\" height=\"36\">";
        xml += "<Label text=\"" + formTitle + "\" fontSize=\"16\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"200\" />";
        xml += "<Button text=\"返回列表\" width=\"80\" height=\"30\" fontSize=\"12\" background=\"#21262D\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:cancel\" />";
        xml += "</Panel>";

        xml += "<ScrollViewer scrollY=\"true\" flexGrow=\"1\" background=\"transparent\">";
        xml += "<Panel direction=\"column\" spacing=\"14\" padding=\"4,4,4,4\" alignItems=\"stretch\">";

        // Row 1: Title & Category
        xml += "<Panel direction=\"row\" spacing=\"16\" alignItems=\"center\">";
        xml += "<Panel direction=\"column\" flexGrow=\"1\" spacing=\"6\">";
        xml += "<Label text=\"账号名称 / 平台名称 (必填)\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_title\" fontSize=\"13\" height=\"38\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        xml += "<Panel direction=\"column\" width=\"200\" spacing=\"6\">";
        xml += "<Label text=\"分类标签 (如: 工作/社交/金融)\" fontSize=\"12\" color=\"#94A3B8\" width=\"190\" />";
        xml += "<TextInput name=\"edit_category\" fontSize=\"13\" height=\"38\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";
        xml += "</Panel>";

        // Row 2: Username
        xml += "<Panel direction=\"column\" spacing=\"6\">";
        xml += "<Label text=\"登录用户名 / 邮箱 / 手机号\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_username\" fontSize=\"13\" height=\"38\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 3: Password + Quick Generator button
        xml += "<Panel direction=\"column\" spacing=\"6\">";
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"登录密码\" fontSize=\"12\" color=\"#94A3B8\" width=\"100\" />";
        xml += "<Button text=\"⚡ 随机生成强密码填入\" width=\"160\" height=\"26\" fontSize=\"11\" background=\"#1E3A8A\" foreground=\"#60A5FA\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"edit:genpwd\" />";
        xml += "</Panel>";
        xml += "<TextInput name=\"edit_password\" fontSize=\"13\" height=\"38\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 4: URL
        xml += "<Panel direction=\"column\" spacing=\"6\">";
        xml += "<Label text=\"官方网站 / 登录页面 URL\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_url\" fontSize=\"13\" height=\"38\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 5: Notes
        xml += "<Panel direction=\"column\" spacing=\"6\">";
        xml += "<Label text=\"备注说明 / 恢复备用码 / 安全提示\" fontSize=\"12\" color=\"#94A3B8\" width=\"250\" />";
        xml += "<TextInput name=\"edit_notes\" fontSize=\"13\" height=\"60\" background=\"#0D1117\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Inner form
        xml += "</ScrollViewer>";

        // Form Footer Buttons
        xml += "<Panel direction=\"row\" height=\"46\" spacing=\"12\" justifyContent=\"flexEnd\" alignItems=\"center\" margin=\"8,0,0,0\">";
        xml += "<Button text=\"取消\" width=\"90\" height=\"34\" fontSize=\"13\" background=\"#21262D\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:cancel\" />";
        xml += "<Button text=\"✔ 保存记录至 SQLite\" width=\"150\" height=\"34\" fontSize=\"13\" background=\"#2563EB\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:save\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Edit Panel

    } else if (state.activeTab == ActiveTab::Generator) {
        // =========================================================
        // VIEW: Password Generator
        // =========================================================
        xml += "<Panel direction=\"column\" flexGrow=\"1\" spacing=\"14\" alignItems=\"stretch\">";

        // Card 1: Generated Password Display
        xml += "<Panel direction=\"column\" background=\"#161B22\" cornerRadius=\"12\" padding=\"20,24,20,24\" spacing=\"14\" alignItems=\"stretch\">";
        
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"生成的高强度随机密码\" fontSize=\"13\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"200\" />";
        xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Label text=\"安全强度: \" fontSize=\"12\" color=\"#64748B\" width=\"65\" />";
        xml += "<Panel background=\"#1E293B\" cornerRadius=\"4\" padding=\"3,8,3,8\">";
        xml += "<Label name=\"gen_strength_val\" text=\"" + XmlEscape(state.currentAssessment.label) + " (" + std::to_string(static_cast<int>(state.currentAssessment.entropyBits)) + " bits)\" fontSize=\"12\" color=\"" + state.currentAssessment.colorHex + "\" fontWeight=\"bold\" width=\"180\" />";
        xml += "</Panel>";
        xml += "</Panel>";
        xml += "</Panel>";

        // Big Password Text Box
        xml += "<Panel direction=\"row\" height=\"56\" background=\"#0D1117\" cornerRadius=\"8\" padding=\"0,16,0,16\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
        xml += "<Label name=\"gen_pwd_val\" text=\"" + XmlEscape(state.currentAssessment.password) + "\" fontSize=\"20\" color=\"#F8FAFC\" fontWeight=\"bold\" flexGrow=\"1\" />";
        xml += "</Panel>";

        // Strength Bar
        xml += "<Panel height=\"6\" background=\"#21262D\" cornerRadius=\"3\" padding=\"0,0,0,0\" alignItems=\"stretch\">";
        int barW = (std::min)(720, (std::max)(30, state.currentAssessment.score * 7));
        xml += "<Panel width=\"" + std::to_string(barW) + "\" height=\"6\" background=\"" + state.currentAssessment.colorHex + "\" cornerRadius=\"3\" />";
        xml += "</Panel>";

        // Primary Action Buttons
        xml += "<Panel direction=\"row\" spacing=\"10\" height=\"38\" alignItems=\"center\">";
        xml += "<Button text=\"重新生成\" width=\"110\" height=\"36\" fontSize=\"13\" background=\"#21262D\" foreground=\"#F8FAFC\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:refresh\" />";
        xml += "<Button text=\"复制密码到剪贴板\" width=\"150\" height=\"36\" fontSize=\"13\" background=\"#2563EB\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:copy\" />";
        xml += "<Button text=\"保存至密码保险库\" width=\"150\" height=\"36\" fontSize=\"13\" background=\"#059669\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:saveto\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Display Card

        // Card 2: Configuration & Presets
        xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"#161B22\" cornerRadius=\"12\" padding=\"20,24,20,24\" spacing=\"16\" alignItems=\"stretch\">";
        xml += "<Label text=\"生成参数定制\" fontSize=\"14\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"160\" />";

        // Row A: Quick Preset Chips
        xml += "<Panel direction=\"column\" spacing=\"8\">";
        xml += "<Label text=\"快速预设模式\" fontSize=\"12\" color=\"#94A3B8\" width=\"120\" />";
        xml += "<Panel direction=\"row\" spacing=\"8\" alignItems=\"center\">";
        auto presetBtn = [&](const char* name, const char* action, GeneratorMode m) {
            bool act = (state.genOptions.mode == m);
            std::string bg = act ? "#2563EB" : "#21262D";
            std::string fg = act ? "#FFFFFF" : "#E2E8F0";
            xml += "<Button text=\"" + std::string(name) + "\" height=\"32\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"" + action + "\" />";
        };
        presetBtn("强密码 16位", "gen:preset:balanced", GeneratorMode::Balanced);
        presetBtn("极高安全 24位", "gen:preset:high", GeneratorMode::HighSecurity);
        presetBtn("纯数字 PIN", "gen:preset:pin", GeneratorMode::Pin);
        presetBtn("记忆短语 Passphrase", "gen:preset:memorable", GeneratorMode::Memorable);
        presetBtn("易读无混淆", "gen:preset:noamb", GeneratorMode::NoAmbiguous);
        xml += "</Panel>";
        xml += "</Panel>";

        // Row B: Length Controls
        xml += "<Panel direction=\"column\" spacing=\"8\">";
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"密码长度\" fontSize=\"12\" color=\"#94A3B8\" width=\"100\" />";
        xml += "<Label text=\"" + std::to_string(state.genOptions.length) + " 字符\" fontSize=\"13\" color=\"#38BDF8\" fontWeight=\"bold\" width=\"80\" />";
        xml += "</Panel>";

        xml += "<Panel direction=\"row\" spacing=\"8\" alignItems=\"center\">";
        xml += "<Button text=\"- 减少\" width=\"70\" height=\"30\" fontSize=\"12\" background=\"#21262D\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:len:dec\" />";
        xml += "<Button text=\"+ 增加\" width=\"70\" height=\"30\" fontSize=\"12\" background=\"#21262D\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:len:inc\" />";

        int lenPresets[] = { 8, 12, 16, 20, 24, 32 };
        for (int lp : lenPresets) {
            bool isCur = (state.genOptions.length == lp);
            std::string bg = isCur ? "#1D4ED8" : "#21262D";
            std::string fg = isCur ? "#FFFFFF" : "#94A3B8";
            xml += "<Button text=\"" + std::to_string(lp) + "\" width=\"44\" height=\"30\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:len:" + std::to_string(lp) + "\" />";
        }
        xml += "</Panel>";
        xml += "</Panel>";

        // Row C: Character Set Toggles
        xml += "<Panel direction=\"column\" spacing=\"8\">";
        xml += "<Label text=\"包含字符规则\" fontSize=\"12\" color=\"#94A3B8\" width=\"120\" />";
        xml += "<Panel direction=\"row\" spacing=\"10\" alignItems=\"center\">";

        auto toggleBtn = [&](const char* text, bool val, const char* action) {
            std::string label = val ? (std::string("✔ ") + text) : (std::string("  ") + text);
            std::string bg = val ? "#1E293B" : "#161B22";
            std::string fg = val ? "#10B981" : "#64748B";
            xml += "<Button text=\"" + XmlEscape(label) + "\" height=\"32\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"" + action + "\" />";
        };

        toggleBtn("大写字母 (A-Z)", state.genOptions.includeUpper, "gen:opt:upper");
        toggleBtn("小写字母 (a-z)", state.genOptions.includeLower, "gen:opt:lower");
        toggleBtn("数字 (0-9)", state.genOptions.includeDigits, "gen:opt:digits");
        toggleBtn("特殊符号 (!@#$)", state.genOptions.includeSymbols, "gen:opt:symbols");
        toggleBtn("排除易混淆字符 (0/O/1/l/I)", state.genOptions.excludeAmbiguous, "gen:opt:ambiguous");

        xml += "</Panel>";
        xml += "</Panel>";

        xml += "</Panel>"; // Config Card
        xml += "</Panel>"; // Generator View

    } else {
        // =========================================================
        // VIEW: Vault Accounts List (Vault or Favorites)
        // =========================================================
        xml += "<Panel direction=\"column\" flexGrow=\"1\" spacing=\"12\" alignItems=\"stretch\">";

        // Top Search & Filter Bar
        xml += "<Panel direction=\"row\" height=\"40\" spacing=\"10\" alignItems=\"center\">";
        xml += "<TextInput name=\"vault_search\" fontSize=\"13\" height=\"36\" background=\"#161B22\" textColor=\"#F8FAFC\" cornerRadius=\"6\" flexGrow=\"1\" />";
        xml += "<Button text=\"搜索\" width=\"66\" height=\"36\" fontSize=\"12\" background=\"#21262D\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:search\" />";
        if (!state.searchQuery.empty()) {
            xml += "<Button text=\"清除\" width=\"60\" height=\"36\" fontSize=\"12\" background=\"#21262D\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:reset_search\" />";
        }
        xml += "<Button text=\"+ 新建账号\" width=\"100\" height=\"36\" fontSize=\"12\" background=\"#2563EB\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:open_add\" />";
        xml += "</Panel>";

        // Summary Bar: Showing count & filter
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\" height=\"28\" margin=\"0,4,4,0\">";
        std::string viewTitle = (state.activeTab == ActiveTab::Favorites) ? "★ 收藏星标密码项" : "❖ 全部密码保管记录";
        if (state.selectedCategory != L"全部") {
            viewTitle += " (" + WideToUtf8(state.selectedCategory) + ")";
        }
        xml += "<Label text=\"" + XmlEscape(viewTitle) + "\" fontSize=\"13\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"320\" />";
        xml += "<Label text=\"共 " + std::to_string(items.size()) + " 条匹配项\" fontSize=\"12\" color=\"#64748B\" width=\"120\" />";
        xml += "</Panel>";

        // Scrollable Accounts List
        xml += "<ScrollViewer scrollY=\"true\" flexGrow=\"1\" background=\"transparent\">";
        xml += "<Panel direction=\"column\" spacing=\"10\" padding=\"0,4,16,0\" alignItems=\"stretch\">";

        if (items.empty()) {
            // Empty State
            xml += "<Panel direction=\"column\" background=\"#161B22\" cornerRadius=\"12\" padding=\"48,24,48,24\" spacing=\"14\" alignItems=\"center\" justifyContent=\"center\" margin=\"24,0,0,0\">";
            xml += "<Label text=\"暂无匹配的密码记录\" fontSize=\"16\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"200\" />";
            xml += "<Label text=\"您可以点击下方按钮添加新账号，或调整左侧分类筛选条件。\" fontSize=\"12\" color=\"#64748B\" width=\"360\" />";
            xml += "<Button text=\"+ 立即新建第一个密码\" width=\"180\" height=\"36\" fontSize=\"13\" background=\"#2563EB\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:open_add\" margin=\"8,0,0,0\" />";
            xml += "</Panel>";
        } else {
            // List of Account Cards
            for (const auto& item : items) {
                std::string idStr = std::to_string(item.id);
                bool isRevealed = (state.revealedPasswords.count(item.id) > 0);
                std::string catColor = GetCategoryColor(item.category);

                xml += "<Panel direction=\"column\" background=\"#161B22\" cornerRadius=\"10\" padding=\"14,16,14,16\" spacing=\"10\" alignItems=\"stretch\">";

                // Card Top: Category Badge, Title, Date, Star Button
                xml += "<Panel direction=\"row\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
                xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\" width=\"400\">";
                xml += "<Panel width=\"44\" height=\"22\" background=\"" + catColor + "\" cornerRadius=\"4\" alignItems=\"center\" justifyContent=\"center\">";
                xml += "<Label text=\"" + XmlEscape(item.category) + "\" fontSize=\"11\" color=\"#FFFFFF\" fontWeight=\"bold\" width=\"32\" />";
                xml += "</Panel>";
                xml += "<Label text=\"" + XmlEscape(item.title) + "\" fontSize=\"14\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"330\" ellipsis=\"true\" />";
                xml += "</Panel>";

                xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\" width=\"180\" justifyContent=\"flexEnd\">";
                xml += "<Label text=\"" + FormatTimestamp(item.updatedAt) + "\" fontSize=\"11\" color=\"#64748B\" width=\"130\" />";
                std::string starText = item.favorite ? "★" : "☆";
                std::string starFg = item.favorite ? "#FBBF24" : "#64748B";
                std::string starBtnName = "vault_star_btn_" + idStr;
                xml += "<Button name=\"" + starBtnName + "\" text=\"" + starText + "\" width=\"30\" height=\"26\" fontSize=\"14\" background=\"#21262D\" foreground=\"" + starFg + "\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:star:" + idStr + "\" />";
                xml += "</Panel>";
                xml += "</Panel>";

                // Card Middle: Username & Password
                xml += "<Panel direction=\"row\" spacing=\"14\" alignItems=\"center\" background=\"#0D1117\" cornerRadius=\"6\" padding=\"8,12,8,12\">";
                // Username part
                xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"6\" width=\"280\">";
                xml += "<Label text=\"账号:\" fontSize=\"12\" color=\"#64748B\" width=\"36\" />";
                xml += "<Label text=\"" + XmlEscape(item.username) + "\" fontSize=\"13\" color=\"#E2E8F0\" width=\"170\" ellipsis=\"true\" />";
                xml += "<Button text=\"复制\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"#21262D\" foreground=\"#38BDF8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:copy_user:" + idStr + "\" />";
                xml += "</Panel>";

                // Password part
                std::string pwdLblName = "vault_pwd_lbl_" + idStr;
                std::string pwdBtnName = "vault_pwd_btn_" + idStr;
                xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"6\" width=\"360\">";
                xml += "<Label text=\"密码:\" fontSize=\"12\" color=\"#64748B\" width=\"36\" />";
                if (isRevealed) {
                    xml += "<Label name=\"" + pwdLblName + "\" text=\"" + XmlEscape(item.password) + "\" fontSize=\"13\" color=\"#10B981\" fontWeight=\"bold\" width=\"180\" ellipsis=\"true\" />";
                    xml += "<Button name=\"" + pwdBtnName + "\" text=\"隐藏\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"#21262D\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:toggle_pwd:" + idStr + "\" />";
                } else {
                    std::string masked(item.password.size() > 0 ? (std::min)(static_cast<size_t>(16), item.password.size()) : 8, '*');
                    xml += "<Label name=\"" + pwdLblName + "\" text=\"" + masked + "\" fontSize=\"13\" color=\"#64748B\" width=\"180\" />";
                    xml += "<Button name=\"" + pwdBtnName + "\" text=\"显示\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"#21262D\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:toggle_pwd:" + idStr + "\" />";
                }
                xml += "<Button text=\"复制\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"#2563EB\" foreground=\"#FFFFFF\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:copy_pwd:" + idStr + "\" />";
                xml += "</Panel>";

                xml += "</Panel>"; // Middle row

                // Card Bottom: URL & Notes & Action Buttons
                xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
                xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"10\" width=\"540\">";
                if (!item.url.empty()) {
                    xml += "<Label text=\"🔗 \" fontSize=\"11\" color=\"#38BDF8\" width=\"18\" />";
                    xml += "<Label text=\"" + XmlEscape(item.url) + "\" fontSize=\"11\" color=\"#38BDF8\" width=\"170\" ellipsis=\"true\" />";
                    xml += "<Button text=\"访问\" width=\"38\" height=\"20\" fontSize=\"10\" background=\"#21262D\" foreground=\"#38BDF8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:open_url:" + idStr + "\" />";
                }
                if (!item.notes.empty()) {
                    xml += "<Label text=\"备注:\" fontSize=\"11\" color=\"#64748B\" width=\"34\" />";
                    xml += "<Label text=\"" + XmlEscape(item.notes) + "\" fontSize=\"11\" color=\"#64748B\" width=\"240\" ellipsis=\"true\" />";
                }
                xml += "</Panel>";

                // Action buttons: Edit & Delete
                xml += "<Panel direction=\"row\" spacing=\"6\" alignItems=\"center\" width=\"120\" justifyContent=\"flexEnd\">";
                xml += "<Button text=\"编辑\" width=\"50\" height=\"26\" fontSize=\"11\" background=\"#21262D\" foreground=\"#E2E8F0\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:open_edit:" + idStr + "\" />";
                xml += "<Button text=\"删除\" width=\"50\" height=\"26\" fontSize=\"11\" background=\"#21262D\" foreground=\"#EF4444\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:del:" + idStr + "\" />";
                xml += "</Panel>";
                xml += "</Panel>"; // Bottom row

                xml += "</Panel>"; // Account Card
            }
        }

        xml += "</Panel>"; // List Panel
        xml += "</ScrollViewer>";

        xml += "</Panel>"; // Vault View
    }

    xml += "</Panel>"; // Right Content Area
    xml += "</Panel>"; // Main Body
    xml += "</Panel>"; // Root

    return xml;
}

} // namespace pwd
