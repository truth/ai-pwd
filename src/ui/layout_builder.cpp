#include "layout_builder.h"
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace pwd {

ThemePalette GetThemePalette(ThemeColor color) {
    ThemePalette p;
    p.id = color;
    switch (color) {
        case ThemeColor::Purple:
            p.name = "极客霓虹紫";
            p.primary = "#7C3AED";
            p.primaryHover = "#6D28D9";
            p.primaryGlow = "#5B21B6";
            p.accent = "#C084FC";
            p.dotColor = "#8B5CF6";
            p.borderAccent = "#7C3AED";
            p.badgeBg = "#2E1065";
            p.textAccent = "#C084FC";

            p.windowBg = "#0C0716";
            p.titleBarBg = "#120B20";
            p.sidebarBg = "#0F091B";
            p.footerBg = "#08040F";
            p.cardBg = "#180F2A";
            p.cardBorder = "#3B1B54";
            p.inputBg = "#0F081C";
            p.inputBorder = "#3B1B54";
            p.innerCardBg = "#0C0716";
            p.btnSecondaryBg = "#24133E";
            p.btnSecondaryHover = "#331B58";
            p.pillActiveBg = "#2E1065";
            p.pillInactiveBg = "#140A24";
            p.catSelectedBg = "#291547";
            p.statusCardBg = "#150C26";
            p.dwmBorderColor = RGB(76, 29, 149);
            p.classBgColor = RGB(12, 7, 22);
            break;

        case ThemeColor::Cyan:
            p.name = "赛博极光青";
            p.primary = "#0284C7";
            p.primaryHover = "#0369A1";
            p.primaryGlow = "#075985";
            p.accent = "#22D3EE";
            p.dotColor = "#06B6D4";
            p.borderAccent = "#0284C7";
            p.badgeBg = "#083344";
            p.textAccent = "#38BDF8";

            p.windowBg = "#040D14";
            p.titleBarBg = "#071520";
            p.sidebarBg = "#06121C";
            p.footerBg = "#03090F";
            p.cardBg = "#0A1F2D";
            p.cardBorder = "#0E3B54";
            p.inputBg = "#06131C";
            p.inputBorder = "#0E3B54";
            p.innerCardBg = "#040D14";
            p.btnSecondaryBg = "#102B3F";
            p.btnSecondaryHover = "#163B57";
            p.pillActiveBg = "#0C374D";
            p.pillInactiveBg = "#081B26";
            p.catSelectedBg = "#11344C";
            p.statusCardBg = "#091C29";
            p.dwmBorderColor = RGB(14, 59, 84);
            p.classBgColor = RGB(4, 13, 20);
            break;

        case ThemeColor::Emerald:
            p.name = "安全翡翠绿";
            p.primary = "#059669";
            p.primaryHover = "#047857";
            p.primaryGlow = "#064E3B";
            p.accent = "#34D399";
            p.dotColor = "#10B981";
            p.borderAccent = "#059669";
            p.badgeBg = "#022C22";
            p.textAccent = "#34D399";

            p.windowBg = "#030D08";
            p.titleBarBg = "#06160F";
            p.sidebarBg = "#05120C";
            p.footerBg = "#020905";
            p.cardBg = "#082116";
            p.cardBorder = "#0D422C";
            p.inputBg = "#04140D";
            p.inputBorder = "#0D422C";
            p.innerCardBg = "#030D08";
            p.btnSecondaryBg = "#0D2E20";
            p.btnSecondaryHover = "#13422E";
            p.pillActiveBg = "#0A3B28";
            p.pillInactiveBg = "#061C13";
            p.catSelectedBg = "#0F3827";
            p.statusCardBg = "#071E14";
            p.dwmBorderColor = RGB(13, 66, 44);
            p.classBgColor = RGB(3, 13, 8);
            break;

        case ThemeColor::Amber:
            p.name = "流金琥珀橙";
            p.primary = "#D97706";
            p.primaryHover = "#B45309";
            p.primaryGlow = "#78350F";
            p.accent = "#FBBF24";
            p.dotColor = "#F59E0B";
            p.borderAccent = "#D97706";
            p.badgeBg = "#451A03";
            p.textAccent = "#FBBF24";

            p.windowBg = "#0E0903";
            p.titleBarBg = "#171006";
            p.sidebarBg = "#130D05";
            p.footerBg = "#0A0602";
            p.cardBg = "#201509";
            p.cardBorder = "#462E12";
            p.inputBg = "#120B04";
            p.inputBorder = "#462E12";
            p.innerCardBg = "#0E0903";
            p.btnSecondaryBg = "#311F0B";
            p.btnSecondaryHover = "#462C0F";
            p.pillActiveBg = "#3E250A";
            p.pillInactiveBg = "#1B1005";
            p.catSelectedBg = "#36220B";
            p.statusCardBg = "#1B1106";
            p.dwmBorderColor = RGB(70, 46, 18);
            p.classBgColor = RGB(14, 9, 3);
            break;

        case ThemeColor::Blue:
        default:
            p.name = "经典科技蓝";
            p.primary = "#2563EB";
            p.primaryHover = "#1D4ED8";
            p.primaryGlow = "#1E40AF";
            p.accent = "#38BDF8";
            p.dotColor = "#3B82F6";
            p.borderAccent = "#2563EB";
            p.badgeBg = "#172554";
            p.textAccent = "#60A5FA";

            p.windowBg = "#070B14";
            p.titleBarBg = "#0B111F";
            p.sidebarBg = "#090E1A";
            p.footerBg = "#05080F";
            p.cardBg = "#0F172A";
            p.cardBorder = "#1E293B";
            p.inputBg = "#0A0F1D";
            p.inputBorder = "#1E293B";
            p.innerCardBg = "#070B14";
            p.btnSecondaryBg = "#162032";
            p.btnSecondaryHover = "#1E293B";
            p.pillActiveBg = "#172554";
            p.pillInactiveBg = "#0C1220";
            p.catSelectedBg = "#162540";
            p.statusCardBg = "#0D1424";
            p.dwmBorderColor = RGB(30, 41, 59);
            p.classBgColor = RGB(7, 11, 20);
            break;
    }
    return p;
}

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
    if (cat == L"工作") return "#3B82F6";     // Blue
    if (cat == L"社交") return "#A855F7";     // Purple
    if (cat == L"金融") return "#F59E0B";     // Amber
    if (cat == L"邮箱") return "#10B981";     // Emerald
    if (cat == L"服务器") return "#EC4899";   // Pink
    return "#64748B";                         // Slate
}

} // namespace

std::string LayoutBuilder::BuildMainLayout(const AppUiState& state, const std::vector<VaultItem>& items) {
    ThemePalette theme = GetThemePalette(state.theme);

    std::string xml;
    xml.reserve(32768);

    // Root window with custom chrome & theme-driven immersive canvas & smooth rounded corners
    xml += "<Panel chrome=\"custom\" direction=\"column\" background=\"" + theme.windowBg + "\" cornerRadius=\"16\" padding=\"0,0,0,0\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // 1. Top Custom Title Bar (46px)
    // -------------------------------------------------------------
    xml += "<Panel role=\"titleBar\" hitTest=\"caption\" direction=\"row\" height=\"46\" background=\"" + theme.titleBarBg + "\" padding=\"14,0,14,0\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
    
    // Left: Logo & Title & Security Encryption Badge
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\" hitTest=\"caption\" height=\"46\">";
    xml += "<Panel width=\"26\" height=\"26\" background=\"" + theme.primary + "\" cornerRadius=\"6\" alignItems=\"center\" justifyContent=\"center\">";
    xml += "<Label text=\"AI\" fontSize=\"12\" color=\"#FFFFFF\" fontWeight=\"bold\" width=\"20\" align=\"center\" />";
    xml += "</Panel>";
    xml += "<Label text=\"AI 密码生成器与保险库\" fontSize=\"13\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"280\" height=\"20\" ellipsis=\"true\" />";
    xml += "<Label text=\"(AI-Pwd)\" fontSize=\"11\" color=\"#64748B\" width=\"80\" height=\"20\" ellipsis=\"true\" />";
    
    // SQLite AES-256-GCM Badge
    xml += "<Panel height=\"26\" background=\"" + theme.badgeBg + "\" cornerRadius=\"13\" padding=\"12,0,12,0\" margin=\"0,0,0,4\" alignItems=\"center\" direction=\"row\" spacing=\"6\">";
    xml += "<Panel width=\"6\" height=\"6\" cornerRadius=\"3\" background=\"" + theme.accent + "\" />";
    xml += "<Label name=\"app_status_badge\" text=\"SQLite 本地加密存储 (AES-256-GCM)\" fontSize=\"11\" color=\"" + theme.textAccent + "\" width=\"250\" height=\"16\" ellipsis=\"true\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // Left

    // Right: Theme Color Dots Switcher + Minimize / Maximize / Close Buttons
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"14\" hitTest=\"client\">";
    
    // Theme Switcher Component
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"6\" hitTest=\"client\">";
    xml += "<Label text=\"🎨 主题色:\" fontSize=\"11\" color=\"#64748B\" fontWeight=\"bold\" width=\"58\" height=\"16\" ellipsis=\"true\" />";

    struct ThemeDotInfo {
        ThemeColor id;
        const char* colorHex;
        const char* action;
    };
    ThemeDotInfo dots[] = {
        { ThemeColor::Blue,    "#3B82F6", "theme:blue" },
        { ThemeColor::Purple,  "#8B5CF6", "theme:purple" },
        { ThemeColor::Cyan,    "#06B6D4", "theme:cyan" },
        { ThemeColor::Emerald, "#10B981", "theme:emerald" },
        { ThemeColor::Amber,   "#F59E0B", "theme:amber" }
    };

    for (const auto& d : dots) {
        bool isActive = (state.theme == d.id);
        if (isActive) {
            xml += "<Button text=\"✔\" fontSize=\"10\" width=\"18\" height=\"18\" cornerRadius=\"9\" background=\"" + std::string(d.colorHex) + "\" foreground=\"#FFFFFF\" lockColors=\"true\" hitTest=\"client\" onClick=\"" + std::string(d.action) + "\" />";
        } else {
            xml += "<Button text=\"\" width=\"14\" height=\"14\" cornerRadius=\"7\" margin=\"2,2,2,2\" background=\"" + std::string(d.colorHex) + "\" foreground=\"#FFFFFF\" lockColors=\"true\" hitTest=\"client\" onClick=\"" + std::string(d.action) + "\" />";
        }
    }
    xml += "</Panel>"; // Theme Switcher

    // Window Caption Controls
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"2\" hitTest=\"client\" width=\"108\">";
    xml += "<Button variant=\"caption-minimize\" width=\"34\" height=\"30\" hitTest=\"client\" onClick=\"windowMinimize\" />";
    xml += "<Button variant=\"caption-maximize\" width=\"34\" height=\"30\" hitTest=\"client\" onClick=\"windowMaximize\" />";
    xml += "<Button variant=\"caption-close\" width=\"38\" height=\"30\" hitTest=\"client\" onClick=\"windowClose\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // Right Title controls
    xml += "</Panel>"; // Title Bar

    // -------------------------------------------------------------
    // 2. Main Body: Left Sidebar + Right Content Area
    // -------------------------------------------------------------
    xml += "<Panel direction=\"row\" flexGrow=\"1\" background=\"" + theme.windowBg + "\" padding=\"0,0,0,0\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // Left Sidebar with ScrollViewer (236px)
    // -------------------------------------------------------------
    xml += "<ScrollViewer scrollY=\"true\" width=\"236\" background=\"" + theme.sidebarBg + "\">";
    xml += "<Panel direction=\"column\" padding=\"10,10,10,10\" spacing=\"6\" alignItems=\"stretch\">";

    // Primary Nav Section
    xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"4\" alignItems=\"stretch\">";
    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\" height=\"20\">";
    xml += "<Panel width=\"3\" height=\"12\" background=\"" + theme.primary + "\" cornerRadius=\"2\" />";
    xml += "<Label text=\"核心导航\" fontSize=\"12\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"120\" height=\"18\" ellipsis=\"true\" />";
    xml += "</Panel>";

    // Tab 1: Generator
    bool isGen = (state.activeTab == ActiveTab::Generator && !state.isEditing);
    if (isGen) {
        xml += "<Button text=\"⚡  密码生成器    ●\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:generator\" />";
    } else {
        xml += "<Button text=\"⚡  密码生成器\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"transparent\" foreground=\"#94A3B8\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:generator\" />";
    }

    // Tab 2: Vault
    bool isVault = (state.activeTab == ActiveTab::Vault && !state.isEditing);
    std::string vaultText = "🔒  密码保险库 (" + std::to_string(state.stats.totalCount) + ")";
    if (isVault) {
        xml += "<Button text=\"" + vaultText + "  ●\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:vault\" />";
    } else {
        xml += "<Button text=\"" + vaultText + "\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"transparent\" foreground=\"#94A3B8\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:vault\" />";
    }

    // Tab 3: Favorites
    bool isFav = (state.activeTab == ActiveTab::Favorites && !state.isEditing);
    std::string favText = "★  收藏星标项 (" + std::to_string(state.stats.favCount) + ")";
    if (isFav) {
        xml += "<Button text=\"" + favText + "  ●\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:favorites\" />";
    } else {
        xml += "<Button text=\"" + favText + "\" width=\"200\" height=\"36\" fontSize=\"13\" background=\"transparent\" foreground=\"#94A3B8\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"nav:favorites\" />";
    }
    xml += "</Panel>"; // Nav section

    // Categories Section with colored dots & count badges
    xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"3\" alignItems=\"stretch\" margin=\"0,4,0,0\">";
    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\" height=\"20\">";
    xml += "<Panel width=\"3\" height=\"12\" background=\"" + theme.accent + "\" cornerRadius=\"2\" />";
    xml += "<Label text=\"分类快速筛选\" fontSize=\"12\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"120\" height=\"18\" ellipsis=\"true\" />";
    xml += "</Panel>";

    struct CategoryMeta {
        const wchar_t* key;
        const char* label;
        const char* dotColor;
        int count;
    };
    CategoryMeta catList[] = {
        { L"全部", "全部项",   "#06B6D4", state.stats.totalCount },
        { L"工作", "工作系统", "#3B82F6", state.stats.workCount },
        { L"社交", "社交平台", "#A855F7", state.stats.socialCount },
        { L"金融", "金融资产", "#F59E0B", state.stats.financeCount },
        { L"邮箱", "常用邮箱", "#10B981", state.stats.emailCount },
        { L"其它", "其它记录", "#94A3B8", state.stats.otherCount }
    };

    for (const auto& c : catList) {
        bool selected = (state.selectedCategory == c.key);
        std::string bg = selected ? theme.catSelectedBg : "transparent";
        std::string fg = selected ? theme.accent : "#94A3B8";
        std::string act = "cat:" + XmlEscape(c.key);
        std::string catText = (selected ? "● " : "  ") + std::string(c.label) + " (" + std::to_string(c.count) + ")";

        xml += "<Button text=\"" + catText + "\" width=\"200\" height=\"28\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"" + act + "\" />";
    }
    xml += "</Panel>"; // Categories

    // Sidebar Bottom Info Card (SQLite Monitor)
    xml += "<Panel direction=\"column\" width=\"200\" background=\"" + theme.statusCardBg + "\" cornerRadius=\"10\" padding=\"10,8,10,8\" spacing=\"4\" alignItems=\"stretch\" margin=\"0,6,0,0\">";
    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\">";
    xml += "<Label text=\"🗄  SQLite 存储监控\" fontSize=\"12\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"130\" height=\"18\" ellipsis=\"true\" />";
    xml += "</Panel>";
    xml += "<Panel width=\"8\" height=\"8\" cornerRadius=\"4\" background=\"#10B981\" />";
    xml += "</Panel>";

    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Label text=\"状态 :\" fontSize=\"11\" color=\"#64748B\" width=\"50\" height=\"16\" ellipsis=\"true\" />";
    xml += "<Label text=\"已加密 · 同步正常\" fontSize=\"11\" color=\"#10B981\" fontWeight=\"bold\" width=\"110\" height=\"16\" align=\"right\" ellipsis=\"true\" />";
    xml += "</Panel>";

    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Label text=\"数据库体积 :\" fontSize=\"11\" color=\"#64748B\" width=\"75\" height=\"16\" ellipsis=\"true\" />";
    xml += "<Label text=\"" + state.stats.dbSizeStr + "\" fontSize=\"11\" color=\"" + theme.textAccent + "\" width=\"85\" height=\"16\" align=\"right\" ellipsis=\"true\" />";
    xml += "</Panel>";

    xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
    xml += "<Label text=\"最新快照 :\" fontSize=\"11\" color=\"#64748B\" width=\"75\" height=\"16\" ellipsis=\"true\" />";
    xml += "<Label text=\"" + state.stats.lastSnapshot + "\" fontSize=\"11\" color=\"#64748B\" width=\"85\" height=\"16\" align=\"right\" ellipsis=\"true\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // SQLite Monitor Card

    xml += "</Panel>"; // Inner Sidebar Panel
    xml += "</ScrollViewer>"; // Left Sidebar ScrollViewer

    // -------------------------------------------------------------
    // Right Content Area
    // -------------------------------------------------------------
    xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"" + theme.windowBg + "\" padding=\"8,18,8,18\" spacing=\"8\" alignItems=\"stretch\">";

    // -------------------------------------------------------------
    // Toast Feedback Banner (if message present)
    // -------------------------------------------------------------
    if (!state.toastMessage.empty()) {
        xml += "<Panel direction=\"row\" height=\"36\" background=\"" + theme.cardBg + "\" cornerRadius=\"8\" padding=\"0,16,0,16\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
        xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Panel width=\"8\" height=\"8\" background=\"" + state.toastColor + "\" cornerRadius=\"4\" />";
        xml += "<Label text=\"" + XmlEscape(state.toastMessage) + "\" fontSize=\"12\" color=\"#F8FAFC\" width=\"440\" />";
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
        xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"" + theme.cardBg + "\" cornerRadius=\"12\" padding=\"18,22,18,22\" spacing=\"12\" alignItems=\"stretch\">";
        
        xml += "<Panel direction=\"row\" alignItems=\"center\" justifyContent=\"spaceBetween\" height=\"36\">";
        xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Panel width=\"3\" height=\"14\" background=\"" + theme.primary + "\" cornerRadius=\"2\" />";
        xml += "<Label text=\"" + formTitle + "\" fontSize=\"15\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"200\" />";
        xml += "</Panel>";
        xml += "<Button text=\"返回列表\" width=\"80\" height=\"30\" fontSize=\"12\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:cancel\" />";
        xml += "</Panel>";

        xml += "<ScrollViewer scrollY=\"true\" flexGrow=\"1\" background=\"transparent\">";
        xml += "<Panel direction=\"column\" spacing=\"12\" padding=\"2,4,4,4\" alignItems=\"stretch\">";

        // Row 1: Title & Category
        xml += "<Panel direction=\"row\" spacing=\"14\" alignItems=\"center\">";
        xml += "<Panel direction=\"column\" flexGrow=\"1\" spacing=\"5\">";
        xml += "<Label text=\"账号名称 / 平台名称 (必填)\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_title\" fontSize=\"13\" height=\"36\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        xml += "<Panel direction=\"column\" width=\"200\" spacing=\"5\">";
        xml += "<Label text=\"分类标签 (工作/社交/金融/邮箱/其它)\" fontSize=\"12\" color=\"#94A3B8\" width=\"200\" />";
        xml += "<TextInput name=\"edit_category\" fontSize=\"13\" height=\"36\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";
        xml += "</Panel>";

        // Row 2: Username
        xml += "<Panel direction=\"column\" spacing=\"5\">";
        xml += "<Label text=\"登录用户名 / 邮箱 / 手机号\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_username\" fontSize=\"13\" height=\"36\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 3: Password + Quick Generator button
        xml += "<Panel direction=\"column\" spacing=\"5\">";
        xml += "<Panel direction=\"row\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"登录密码\" fontSize=\"12\" color=\"#94A3B8\" width=\"100\" />";
        xml += "<Button text=\"⚡ 随机生成强密码填入\" width=\"160\" height=\"24\" fontSize=\"11\" background=\"" + theme.badgeBg + "\" foreground=\"" + theme.accent + "\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"edit:genpwd\" />";
        xml += "</Panel>";
        xml += "<TextInput name=\"edit_password\" fontSize=\"13\" height=\"36\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 4: URL
        xml += "<Panel direction=\"column\" spacing=\"5\">";
        xml += "<Label text=\"官方网站 / 登录页面 URL\" fontSize=\"12\" color=\"#94A3B8\" width=\"220\" />";
        xml += "<TextInput name=\"edit_url\" fontSize=\"13\" height=\"36\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        // Row 5: Notes
        xml += "<Panel direction=\"column\" spacing=\"5\">";
        xml += "<Label text=\"备注说明 / 恢复备用码 / 安全提示\" fontSize=\"12\" color=\"#94A3B8\" width=\"250\" />";
        xml += "<TextInput name=\"edit_notes\" fontSize=\"13\" height=\"56\" background=\"" + theme.inputBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Inner form
        xml += "</ScrollViewer>";

        // Form Footer Buttons
        xml += "<Panel direction=\"row\" height=\"44\" spacing=\"12\" justifyContent=\"flexEnd\" alignItems=\"center\" margin=\"6,0,0,0\">";
        xml += "<Button text=\"取消\" width=\"90\" height=\"34\" fontSize=\"13\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:cancel\" />";
        xml += "<Button text=\"✔ 保存记录至 SQLite\" width=\"160\" height=\"34\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"edit:save\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Edit Panel

    } else if (state.activeTab == ActiveTab::Generator) {
        // =========================================================
        // VIEW: Password Generator (Cyberpunk Console Style)
        // =========================================================
        xml += "<Panel direction=\"column\" flexGrow=\"1\" padding=\"0,0,0,0\" spacing=\"8\" alignItems=\"stretch\">";

        // ---------------------------------------------------------
        // Card 1: Generated Password Display (Top Section)
        // ---------------------------------------------------------
        xml += "<Panel direction=\"column\" height=\"225\" background=\"" + theme.cardBg + "\" cornerRadius=\"12\" padding=\"10,16,10,16\" spacing=\"8\" alignItems=\"stretch\">";
        
        // Header Row: Title & Strength Pill Badge
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Panel width=\"3\" height=\"14\" background=\"" + theme.primary + "\" cornerRadius=\"2\" />";
        xml += "<Label text=\"生成的高强度随机密码\" fontSize=\"13\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"180\" ellipsis=\"true\" />";
        xml += "</Panel>";
        
        // Dynamic Entropy Strength Badge
        std::string badgeBg = "#052E2B";
        std::string badgeFg = "#34D399";
        std::string badgeText = "🛡 极高安全 (防撞破) · " + std::to_string(static_cast<int>(state.currentAssessment.entropyBits)) + " bits 熵值";
        if (state.currentAssessment.entropyBits < 40) {
            badgeBg = "#4C0519";
            badgeFg = "#FDA4AF";
            badgeText = "⚠ 弱密码 · " + std::to_string(static_cast<int>(state.currentAssessment.entropyBits)) + " bits 熵值";
        } else if (state.currentAssessment.entropyBits < 64) {
            badgeBg = "#451A03";
            badgeFg = "#FBBF24";
            badgeText = "中等强度 · " + std::to_string(static_cast<int>(state.currentAssessment.entropyBits)) + " bits 熵值";
        } else if (state.currentAssessment.entropyBits < 85) {
            badgeBg = "#064E3B";
            badgeFg = "#34D399";
            badgeText = "🛡 良好防御 · " + std::to_string(static_cast<int>(state.currentAssessment.entropyBits)) + " bits 熵值";
        }

        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Label text=\"安全强度: \" fontSize=\"12\" color=\"#64748B\" width=\"75\" ellipsis=\"true\" />";
        xml += "<Panel background=\"" + badgeBg + "\" cornerRadius=\"12\" padding=\"2,10,2,10\">";
        xml += "<Label name=\"gen_strength_val\" text=\"" + XmlEscape(badgeText) + "\" fontSize=\"11\" color=\"" + badgeFg + "\" fontWeight=\"bold\" width=\"210\" ellipsis=\"true\" />";
        xml += "</Panel>";
        xml += "</Panel>";
        xml += "</Panel>";

        // Big Password Text Box with Syntax Highlighting & Eye/Refresh buttons
        xml += "<Panel direction=\"row\" height=\"52\" background=\"" + theme.inputBg + "\" cornerRadius=\"8\" padding=\"14,0,14,0\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
        
        if (!state.generatorRevealed) {
            std::string masked(state.currentAssessment.password.empty() ? 16 : state.currentAssessment.password.size(), '*');
            xml += "<Label text=\"" + masked + "\" fontSize=\"22\" color=\"#64748B\" fontWeight=\"bold\" width=\"460\" ellipsis=\"true\" />";
        } else {
            // Multi-color character syntax highlighting
            xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"1\" flexWrap=\"wrap\" width=\"500\">";
            for (wchar_t ch : state.currentAssessment.password) {
                std::string col = "#FFFFFF"; // lowercase
                if (ch >= L'0' && ch <= L'9') {
                    col = "#FBBF24"; // Numbers: Amber
                } else if (ch >= L'A' && ch <= L'Z') {
                    col = "#38BDF8"; // Uppercase: Cyan
                } else if (ch >= L'a' && ch <= L'z') {
                    col = "#FFFFFF"; // Lowercase: Crisp White
                } else {
                    col = "#F43F5E"; // Symbols: Pink/Magenta
                }
                std::string s;
                if (ch == L'<') s = "&lt;";
                else if (ch == L'>') s = "&gt;";
                else if (ch == L'&') s = "&amp;";
                else if (ch == L'\"') s = "&quot;";
                else if (ch == L'\'') s = "&apos;";
                else s = WideToUtf8(std::wstring(1, ch));

                xml += "<Label text=\"" + s + "\" fontSize=\"22\" color=\"" + col + "\" fontWeight=\"bold\" ellipsis=\"true\" />";
            }
            xml += "</Panel>";
        }

        // Action toggles inside display box: Eye & Refresh
        xml += "<Panel direction=\"row\" spacing=\"6\" alignItems=\"center\">";
        std::string eyeText = state.generatorRevealed ? "👁" : "👁‍🗨";
        xml += "<Button text=\"" + eyeText + "\" width=\"32\" height=\"30\" fontSize=\"13\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:toggle_eye\" />";
        xml += "<Button text=\"⟳\" width=\"32\" height=\"30\" fontSize=\"15\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:refresh\" />";
        xml += "</Panel>";
        xml += "</Panel>"; // Big Password Box

        // Multi-Segmented Strength Bar & Labels
        int scoreLevel = 1;
        if (state.currentAssessment.entropyBits >= 85) scoreLevel = 4;
        else if (state.currentAssessment.entropyBits >= 64) scoreLevel = 3;
        else if (state.currentAssessment.entropyBits >= 40) scoreLevel = 2;

        std::string seg1Color = (scoreLevel >= 1) ? "#F43F5E" : theme.btnSecondaryBg;
        std::string seg2Color = (scoreLevel >= 2) ? "#F59E0B" : theme.btnSecondaryBg;
        std::string seg3Color = (scoreLevel >= 3) ? "#10B981" : theme.btnSecondaryBg;
        std::string seg4Color = (scoreLevel >= 4) ? "#06B6D4" : theme.btnSecondaryBg;

        xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"3\" alignItems=\"stretch\">";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" height=\"4\" spacing=\"4\" alignItems=\"stretch\">";
        xml += "<Panel flexGrow=\"1\" height=\"4\" background=\"" + seg1Color + "\" cornerRadius=\"2\" />";
        xml += "<Panel flexGrow=\"1\" height=\"4\" background=\"" + seg2Color + "\" cornerRadius=\"2\" />";
        xml += "<Panel flexGrow=\"1\" height=\"4\" background=\"" + seg3Color + "\" cornerRadius=\"2\" />";
        xml += "<Panel flexGrow=\"1\" height=\"4\" background=\"" + seg4Color + "\" cornerRadius=\"2\" />";
        xml += "</Panel>";

        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"弱\" fontSize=\"11\" color=\"" + (scoreLevel >= 1 ? std::string("#F43F5E") : std::string("#475569")) + "\" width=\"40\" ellipsis=\"true\" />";
        xml += "<Label text=\"中等\" fontSize=\"11\" color=\"" + (scoreLevel >= 2 ? std::string("#F59E0B") : std::string("#475569")) + "\" width=\"50\" align=\"center\" ellipsis=\"true\" />";
        xml += "<Label text=\"良好\" fontSize=\"11\" color=\"" + (scoreLevel >= 3 ? std::string("#10B981") : std::string("#475569")) + "\" width=\"50\" align=\"center\" ellipsis=\"true\" />";
        xml += "<Label text=\"企业级防破\" fontSize=\"11\" color=\"" + (scoreLevel >= 4 ? std::string("#06B6D4") : std::string("#475569")) + "\" width=\"75\" align=\"right\" ellipsis=\"true\" />";
        xml += "</Panel>";
        xml += "</Panel>"; // Strength bar & labels

        // Primary Action Buttons Row
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" spacing=\"10\" height=\"36\" alignItems=\"center\">";
        xml += "<Button text=\"⟳  重新生成\" width=\"120\" height=\"36\" fontSize=\"13\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#E2E8F0\" cornerRadius=\"8\" lockColors=\"true\" onClick=\"gen:refresh\" />";
        xml += "<Button text=\"📋  复制密码到剪贴板\" flexGrow=\"1\" height=\"36\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"8\" fontWeight=\"bold\" lockColors=\"true\" onClick=\"gen:copy\" />";
        xml += "<Button text=\"🛡  保存至密码保险库\" width=\"190\" height=\"36\" fontSize=\"13\" background=\"#059669\" foreground=\"#FFFFFF\" cornerRadius=\"8\" fontWeight=\"bold\" lockColors=\"true\" onClick=\"gen:saveto\" />";
        xml += "</Panel>";

        xml += "</Panel>"; // Display Card 1

        // ---------------------------------------------------------
        // Card 2: Configuration & Parameters ("生成参数定制")
        // ---------------------------------------------------------
        xml += "<Panel direction=\"column\" flexGrow=\"1\" background=\"" + theme.cardBg + "\" cornerRadius=\"12\" padding=\"10,16,10,16\" spacing=\"8\" alignItems=\"stretch\">";
        
        // Header with AI Breach Verification badge
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\" height=\"24\">";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"8\">";
        xml += "<Panel width=\"3\" height=\"14\" background=\"" + theme.primary + "\" cornerRadius=\"2\" />";
        xml += "<Label text=\"生成参数定制\" fontSize=\"13\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"120\" ellipsis=\"true\" />";
        xml += "</Panel>";

        xml += "<Panel background=\"" + theme.badgeBg + "\" cornerRadius=\"12\" padding=\"2,10,2,10\">";
        xml += "<Label text=\"🛡 AI 实时防撞库校验 : 未在已知泄露库中出现\" fontSize=\"11\" color=\"" + theme.textAccent + "\" width=\"270\" ellipsis=\"true\" />";
        xml += "</Panel>";
        xml += "</Panel>";

        // Inner Parameters Container
        xml += "<Panel direction=\"column\" background=\"" + theme.innerCardBg + "\" cornerRadius=\"10\" padding=\"8,12,8,12\" spacing=\"6\" alignItems=\"stretch\">";

        // Row A: Quick Presets
        xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"3\">";
        xml += "<Label text=\"快速预设模式\" fontSize=\"11\" color=\"#64748B\" fontWeight=\"bold\" width=\"120\" ellipsis=\"true\" />";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" spacing=\"8\" alignItems=\"center\">";
        auto presetBtn = [&](const char* name, const char* action, GeneratorMode m) {
            bool act = (state.genOptions.mode == m);
            std::string bg = act ? theme.primary : theme.btnSecondaryBg;
            std::string fg = act ? "#FFFFFF" : "#94A3B8";
            xml += "<Button text=\"" + std::string(name) + "\" height=\"28\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"" + action + "\" />";
        };
        presetBtn("强密码 16位", "gen:preset:balanced", GeneratorMode::Balanced);
        presetBtn("极高安全 24位", "gen:preset:high", GeneratorMode::HighSecurity);
        presetBtn("纯数字 PIN", "gen:preset:pin", GeneratorMode::Pin);
        presetBtn("⚡ 记忆短语", "gen:preset:memorable", GeneratorMode::Memorable);
        presetBtn("易读无混淆", "gen:preset:noamb", GeneratorMode::NoAmbiguous);
        xml += "</Panel>";
        xml += "</Panel>";

        // Row B: Password Length Slider & Counter
        xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"3\">";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Label text=\"密码长度\" fontSize=\"12\" color=\"#94A3B8\" width=\"80\" ellipsis=\"true\" />";
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\">";
        xml += "<Button text=\"-\" width=\"24\" height=\"20\" fontSize=\"12\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"gen:len:dec\" />";
        xml += "<Label text=\"" + std::to_string(state.genOptions.length) + " 字符\" fontSize=\"12\" color=\"" + theme.accent + "\" fontWeight=\"bold\" width=\"56\" align=\"center\" ellipsis=\"true\" />";
        xml += "<Button text=\"+\" width=\"24\" height=\"20\" fontSize=\"12\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"gen:len:inc\" />";
        xml += "</Panel>";
        xml += "</Panel>";

        // Visual Slider Bar
        double ratio = (std::max)(0.04, (std::min)(0.96, (state.genOptions.length - 6) / 58.0));
        int totalTrackWidth = 600;
        int fillW = static_cast<int>(ratio * totalTrackWidth);
        int remW = (std::max)(10, totalTrackWidth - fillW);
        xml += "<Panel height=\"12\" direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" width=\"620\">";
        xml += "<Panel width=\"" + std::to_string(fillW) + "\" height=\"6\" background=\"" + theme.accent + "\" cornerRadius=\"3\" />";
        xml += "<Panel width=\"12\" height=\"12\" cornerRadius=\"6\" background=\"#FFFFFF\" padding=\"2,2,2,2\" alignItems=\"center\" justifyContent=\"center\">";
        xml += "<Panel width=\"8\" height=\"8\" cornerRadius=\"4\" background=\"" + theme.primary + "\" />";
        xml += "</Panel>";
        xml += "<Panel width=\"" + std::to_string(remW) + "\" height=\"6\" background=\"" + theme.btnSecondaryBg + "\" cornerRadius=\"3\" />";
        xml += "</Panel>";

        // Slider Quick Tick Marks
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
        xml += "<Button text=\"6位 (简短)\" height=\"18\" fontSize=\"10\" background=\"transparent\" foreground=\"" + (state.genOptions.length == 6 ? theme.accent : std::string("#64748B")) + "\" lockColors=\"true\" onClick=\"gen:len:6\" />";
        xml += "<Button text=\"16位 (推荐)\" height=\"18\" fontSize=\"10\" background=\"transparent\" foreground=\"" + (state.genOptions.length == 16 ? theme.accent : std::string("#64748B")) + "\" lockColors=\"true\" onClick=\"gen:len:16\" />";
        xml += "<Button text=\"32位 (超强)\" height=\"18\" fontSize=\"10\" background=\"transparent\" foreground=\"" + (state.genOptions.length == 32 ? theme.accent : std::string("#64748B")) + "\" lockColors=\"true\" onClick=\"gen:len:32\" />";
        xml += "<Button text=\"64位 (最大)\" height=\"18\" fontSize=\"10\" background=\"transparent\" foreground=\"" + (state.genOptions.length == 64 ? theme.accent : std::string("#64748B")) + "\" lockColors=\"true\" onClick=\"gen:len:64\" />";
        xml += "</Panel>";
        xml += "</Panel>"; // Row B Length

        // Row C: Character Set Toggles (2x2 Grid)
        xml += "<Panel direction=\"column\" padding=\"0,0,0,0\" spacing=\"4\">";
        xml += "<Label text=\"字符构成规则\" fontSize=\"11\" color=\"#64748B\" fontWeight=\"bold\" width=\"120\" ellipsis=\"true\" />";

        auto renderToggleCard = [&](const char* badge, const char* title, bool isChecked, const char* action) {
            std::string bg = isChecked ? theme.pillActiveBg : theme.pillInactiveBg;
            std::string fg = isChecked ? theme.accent : "#94A3B8";
            std::string mark = isChecked ? "✔ " : "   ";
            std::string btnText = mark + "[" + std::string(badge) + "]  " + std::string(title);

            xml += "<Button text=\"" + btnText + "\" flexGrow=\"1\" height=\"34\" fontSize=\"12\" background=\"" + bg + "\" foreground=\"" + fg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"" + std::string(action) + "\" />";
        };

        // Grid Row 1
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" spacing=\"10\" alignItems=\"center\">";
        renderToggleCard("ABC", "大写字母 (A-Z)", state.genOptions.includeUpper, "gen:opt:upper");
        renderToggleCard("abc", "小写字母 (a-z)", state.genOptions.includeLower, "gen:opt:lower");
        xml += "</Panel>";

        // Grid Row 2
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" spacing=\"10\" alignItems=\"center\">";
        renderToggleCard("123", "包含数字 (0-9)", state.genOptions.includeDigits, "gen:opt:digits");
        renderToggleCard("#$&", "特殊符号 (!@#$...)", state.genOptions.includeSymbols, "gen:opt:symbols");
        xml += "</Panel>";

        // Full Width Button: Exclude Ambiguous
        {
            bool isChecked = state.genOptions.excludeAmbiguous;
            std::string ambBg = isChecked ? theme.pillActiveBg : theme.pillInactiveBg;
            std::string ambFg = isChecked ? theme.accent : "#94A3B8";
            std::string ambMark = isChecked ? "✔ " : "   ";
            std::string ambText = ambMark + "排除易混淆与模糊字符 (自动过滤 1/l/I/0/O/` 等)";

            xml += "<Button text=\"" + ambText + "\" height=\"36\" fontSize=\"12\" background=\"" + ambBg + "\" foreground=\"" + ambFg + "\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"gen:opt:ambiguous\" />";
        }

        xml += "</Panel>"; // Row C Character sets

        xml += "</Panel>"; // Inner Parameters
        xml += "</Panel>"; // Config Card 2

        xml += "</Panel>"; // Generator View

    } else {
        // =========================================================
        // VIEW: Vault Accounts List (Vault or Favorites)
        // =========================================================
        xml += "<Panel direction=\"column\" flexGrow=\"1\" spacing=\"10\" alignItems=\"stretch\">";

        // Top Search & Filter Bar
        xml += "<Panel direction=\"row\" height=\"40\" spacing=\"10\" alignItems=\"center\">";
        xml += "<TextInput name=\"vault_search\" fontSize=\"13\" height=\"36\" background=\"" + theme.cardBg + "\" textColor=\"#F8FAFC\" cornerRadius=\"6\" flexGrow=\"1\" />";
        xml += "<Button text=\"搜索\" width=\"66\" height=\"36\" fontSize=\"12\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#E2E8F0\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:search\" />";
        if (!state.searchQuery.empty()) {
            xml += "<Button text=\"清除\" width=\"60\" height=\"36\" fontSize=\"12\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:reset_search\" />";
        }
        xml += "<Button text=\"+ 新建账号\" width=\"100\" height=\"36\" fontSize=\"12\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:open_add\" />";
        xml += "</Panel>";

        // Summary Bar: Showing count & filter
        xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\" height=\"26\" margin=\"0,4,0,4\">";
        std::string viewTitle = (state.activeTab == ActiveTab::Favorites) ? "★ 收藏星标密码项" : "❖ 全部密码保管记录";
        if (state.selectedCategory != L"全部") {
            viewTitle += " (" + WideToUtf8(state.selectedCategory) + ")";
        }
        xml += "<Label text=\"" + XmlEscape(viewTitle) + "\" fontSize=\"13\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"320\" height=\"20\" ellipsis=\"true\" />";
        xml += "<Label text=\"共 " + std::to_string(items.size()) + " 条匹配项\" fontSize=\"12\" color=\"#64748B\" width=\"120\" height=\"20\" align=\"right\" ellipsis=\"true\" />";
        xml += "</Panel>";

        // Scrollable Accounts List
        xml += "<ScrollViewer scrollY=\"true\" flexGrow=\"1\" background=\"transparent\">";
        xml += "<Panel direction=\"column\" spacing=\"8\" padding=\"0,2,14,0\" alignItems=\"stretch\">";

        if (items.empty()) {
            // Empty State
            xml += "<Panel direction=\"column\" background=\"" + theme.cardBg + "\" cornerRadius=\"12\" padding=\"48,24,48,24\" spacing=\"14\" alignItems=\"center\" justifyContent=\"center\" margin=\"24,0,0,0\">";
            xml += "<Label text=\"暂无匹配的密码记录\" fontSize=\"16\" color=\"#94A3B8\" fontWeight=\"bold\" width=\"200\" />";
            xml += "<Label text=\"您可以点击下方按钮添加新账号，或调整左侧分类筛选条件。\" fontSize=\"12\" color=\"#64748B\" width=\"360\" />";
            xml += "<Button text=\"+ 立即新建第一个密码\" width=\"180\" height=\"36\" fontSize=\"13\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"6\" lockColors=\"true\" onClick=\"vault:open_add\" margin=\"8,0,0,0\" />";
            xml += "</Panel>";
        } else {
            // List of Account Cards
            for (const auto& item : items) {
                std::string idStr = std::to_string(item.id);
                bool isRevealed = (state.revealedPasswords.count(item.id) > 0);
                std::string catColor = GetCategoryColor(item.category);

                xml += "<Panel direction=\"column\" background=\"" + theme.cardBg + "\" cornerRadius=\"10\" padding=\"14,12,14,12\" spacing=\"8\" alignItems=\"stretch\">";

                // Card Top: Category Badge, Title, Date, Star Button
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"8\" width=\"400\">";
                xml += "<Panel width=\"44\" height=\"22\" background=\"" + catColor + "\" cornerRadius=\"4\" padding=\"0,0,0,0\" alignItems=\"center\" justifyContent=\"center\">";
                xml += "<Label text=\"" + XmlEscape(item.category) + "\" fontSize=\"11\" color=\"#FFFFFF\" fontWeight=\"bold\" width=\"36\" height=\"16\" align=\"center\" ellipsis=\"true\" />";
                xml += "</Panel>";
                xml += "<Label text=\"" + XmlEscape(item.title) + "\" fontSize=\"14\" color=\"#F8FAFC\" fontWeight=\"bold\" width=\"330\" height=\"20\" ellipsis=\"true\" />";
                xml += "</Panel>";

                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"8\" width=\"180\" justifyContent=\"flexEnd\">";
                xml += "<Label text=\"" + FormatTimestamp(item.updatedAt) + "\" fontSize=\"11\" color=\"#64748B\" width=\"130\" height=\"16\" ellipsis=\"true\" />";
                std::string starText = item.favorite ? "★" : "☆";
                std::string starFg = item.favorite ? "#FBBF24" : "#64748B";
                std::string starBtnName = "vault_star_btn_" + idStr;
                xml += "<Button name=\"" + starBtnName + "\" text=\"" + starText + "\" width=\"30\" height=\"26\" fontSize=\"14\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"" + starFg + "\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:star:" + idStr + "\" />";
                xml += "</Panel>";
                xml += "</Panel>";

                // Card Middle: Username & Password
                xml += "<Panel direction=\"row\" spacing=\"14\" alignItems=\"center\" background=\"" + theme.inputBg + "\" cornerRadius=\"6\" padding=\"12,6,12,6\">";
                // Username part
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\" width=\"280\">";
                xml += "<Label text=\"账号:\" fontSize=\"12\" color=\"#64748B\" width=\"36\" height=\"18\" ellipsis=\"true\" />";
                xml += "<Label text=\"" + XmlEscape(item.username) + "\" fontSize=\"13\" color=\"#E2E8F0\" width=\"170\" height=\"18\" ellipsis=\"true\" />";
                xml += "<Button text=\"复制\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#38BDF8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:copy_user:" + idStr + "\" />";
                xml += "</Panel>";

                // Password part
                std::string pwdLblName = "vault_pwd_lbl_" + idStr;
                std::string pwdBtnName = "vault_pwd_btn_" + idStr;
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"6\" width=\"360\">";
                xml += "<Label text=\"密码:\" fontSize=\"12\" color=\"#64748B\" width=\"36\" height=\"18\" ellipsis=\"true\" />";
                if (isRevealed) {
                    xml += "<Label name=\"" + pwdLblName + "\" text=\"" + XmlEscape(item.password) + "\" fontSize=\"13\" color=\"#10B981\" fontWeight=\"bold\" width=\"180\" height=\"18\" ellipsis=\"true\" />";
                    xml += "<Button name=\"" + pwdBtnName + "\" text=\"隐藏\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:toggle_pwd:" + idStr + "\" />";
                } else {
                    std::string masked(item.password.size() > 0 ? (std::min)(static_cast<size_t>(16), item.password.size()) : 8, '*');
                    xml += "<Label name=\"" + pwdLblName + "\" text=\"" + masked + "\" fontSize=\"13\" color=\"#64748B\" width=\"180\" height=\"18\" ellipsis=\"true\" />";
                    xml += "<Button name=\"" + pwdBtnName + "\" text=\"显示\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#94A3B8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:toggle_pwd:" + idStr + "\" />";
                }
                xml += "<Button text=\"复制\" width=\"44\" height=\"24\" fontSize=\"11\" background=\"" + theme.primary + "\" foreground=\"#FFFFFF\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:copy_pwd:" + idStr + "\" />";
                xml += "</Panel>";

                xml += "</Panel>"; // Middle row

                // Card Bottom: URL & Notes & Action Buttons
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" justifyContent=\"spaceBetween\" alignItems=\"center\">";
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" alignItems=\"center\" spacing=\"10\" width=\"540\">";
                if (!item.url.empty()) {
                    xml += "<Label text=\"🔗 \" fontSize=\"11\" color=\"#38BDF8\" width=\"18\" height=\"16\" ellipsis=\"true\" />";
                    xml += "<Label text=\"" + XmlEscape(item.url) + "\" fontSize=\"11\" color=\"#38BDF8\" width=\"170\" height=\"16\" ellipsis=\"true\" />";
                    xml += "<Button text=\"访问\" width=\"38\" height=\"20\" fontSize=\"10\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#38BDF8\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:open_url:" + idStr + "\" />";
                }
                if (!item.notes.empty()) {
                    xml += "<Label text=\"备注:\" fontSize=\"11\" color=\"#64748B\" width=\"34\" height=\"16\" ellipsis=\"true\" />";
                    xml += "<Label text=\"" + XmlEscape(item.notes) + "\" fontSize=\"11\" color=\"#64748B\" width=\"240\" height=\"16\" ellipsis=\"true\" />";
                }
                xml += "</Panel>";

                // Action buttons: Edit & Delete
                xml += "<Panel direction=\"row\" padding=\"0,0,0,0\" spacing=\"6\" alignItems=\"center\" width=\"120\" justifyContent=\"flexEnd\">";
                xml += "<Button text=\"编辑\" width=\"50\" height=\"24\" fontSize=\"11\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#E2E8F0\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:open_edit:" + idStr + "\" />";
                xml += "<Button text=\"删除\" width=\"50\" height=\"24\" fontSize=\"11\" background=\"" + theme.btnSecondaryBg + "\" foreground=\"#EF4444\" cornerRadius=\"4\" lockColors=\"true\" onClick=\"vault:del:" + idStr + "\" />";
                xml += "</Panel>";
                xml += "</Panel>"; // Bottom row

                xml += "</Panel>"; // Account Card
            }
        }

        xml += "</Panel>"; // List Panel
        xml += "</ScrollViewer>";

        xml += "</Panel>"; // Vault View
    }

    xml += "</Panel>"; // Right Content Panel
    xml += "</Panel>"; // Main Body

    // -------------------------------------------------------------
    // 3. Footer Status Bar (26px)
    // -------------------------------------------------------------
    xml += "<Panel direction=\"row\" height=\"26\" background=\"" + theme.footerBg + "\" padding=\"0,16,0,16\" alignItems=\"center\" justifyContent=\"spaceBetween\">";
    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
    xml += "<Panel width=\"6\" height=\"6\" cornerRadius=\"3\" background=\"#10B981\" />";
    xml += "<Label text=\"安全沙箱环境运行中   |   无剪贴板外泄窃取监听\" fontSize=\"11\" color=\"#64748B\" width=\"320\" />";
    xml += "</Panel>";

    xml += "<Panel direction=\"row\" alignItems=\"center\" spacing=\"8\">";
    xml += "<Label text=\"算法 : CSPRNG + Argon2id   |   v2.4.0-desktop\" fontSize=\"11\" color=\"#64748B\" width=\"280\" align=\"right\" />";
    xml += "</Panel>";
    xml += "</Panel>"; // Footer Status Bar

    xml += "</Panel>"; // Root

    return xml;
}

} // namespace pwd

