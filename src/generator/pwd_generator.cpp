#include "pwd_generator.h"
#include <Windows.h>
#include <bcrypt.h>
#include <cmath>
#include <algorithm>
#include <set>

namespace pwd {

namespace {

const char* kUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* kLower = "abcdefghijklmnopqrstuvwxyz";
const char* kDigits = "0123456789";
const char* kSymbols = "!@#$%^&*()_+-=[]{}|;:,.<>?";

const char* kAmbiguous = "0Oo1lI|";

// Word pool for memorable passphrases
const char* kWordList[] = {
    "Cosmic", "Falcon", "Velvet", "Orbit", "Silver", "Beacon", "Tiger", "Solar",
    "Aurora", "Thunder", "Canyon", "Summit", "Shield", "Rocket", "Anchor", "Breeze",
    "Crystal", "Dragon", "Echo", "Forest", "Galaxy", "Harbor", "Island", "Jaguar",
    "Kestrel", "Legend", "Meteor", "Nexus", "Oasis", "Phoenix", "Quartz", "River",
    "Shadow", "Titan", "Vortex", "Willow", "Zenith", "Brave", "Clever", "Swift",
    "Daring", "Flash", "Golden", "Iron", "Knight", "Laser", "Mystic", "Nova",
    "Pioneer", "Quantum"
};

} // namespace

uint32_t PasswordGenerator::SecureRandom(uint32_t maxExclusive) {
    if (maxExclusive <= 1) return 0;

    uint32_t randVal = 0;
    // Use BCryptGenRandom from CNG for cryptographically secure pseudo-random numbers
    NTSTATUS status = BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&randVal),
                                      sizeof(randVal), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status)) {
        // Fallback to rand() with salt
        randVal = static_cast<uint32_t>(GetTickCount64() ^ (rand() << 16) ^ rand());
    }
    return randVal % maxExclusive;
}

std::wstring PasswordGenerator::GenerateMemorablePassphrase() {
    constexpr size_t numWords = sizeof(kWordList) / sizeof(kWordList[0]);
    std::wstring result;
    for (int i = 0; i < 4; ++i) {
        if (i > 0) result += L"-";
        uint32_t idx = SecureRandom(static_cast<uint32_t>(numWords));
        const char* w = kWordList[idx];
        while (*w) {
            result += static_cast<wchar_t>(*w++);
        }
    }
    // Append 2 random digits at the end
    result += L"-";
    result += std::to_wstring(SecureRandom(90) + 10);
    return result;
}

PasswordAssessment PasswordGenerator::Generate(const GeneratorOptions& options) {
    if (options.mode == GeneratorMode::Memorable) {
        std::wstring pwd = GenerateMemorablePassphrase();
        return Assess(pwd);
    }

    if (options.mode == GeneratorMode::Pin) {
        int len = (options.length < 4) ? 6 : options.length;
        std::wstring pwd;
        pwd.reserve(len);
        for (int i = 0; i < len; ++i) {
            pwd += static_cast<wchar_t>(L'0' + SecureRandom(10));
        }
        return Assess(pwd);
    }

    // Build allowed character pools
    std::string pool;
    std::vector<std::string> mandatoryCategories;

    auto filterPool = [&](const char* src) -> std::string {
        std::string filtered;
        for (const char* p = src; *p; ++p) {
            if (options.excludeAmbiguous && strchr(kAmbiguous, *p)) {
                continue;
            }
            filtered += *p;
        }
        return filtered;
    };

    if (options.includeUpper) {
        std::string s = filterPool(kUpper);
        if (!s.empty()) {
            pool += s;
            mandatoryCategories.push_back(s);
        }
    }
    if (options.includeLower) {
        std::string s = filterPool(kLower);
        if (!s.empty()) {
            pool += s;
            mandatoryCategories.push_back(s);
        }
    }
    if (options.includeDigits) {
        std::string s = filterPool(kDigits);
        if (!s.empty()) {
            pool += s;
            mandatoryCategories.push_back(s);
        }
    }
    if (options.includeSymbols) {
        std::string s = filterPool(kSymbols);
        if (!s.empty()) {
            pool += s;
            mandatoryCategories.push_back(s);
        }
    }

    if (pool.empty()) {
        pool = filterPool(kLower) + filterPool(kDigits);
        mandatoryCategories.push_back(filterPool(kLower));
        mandatoryCategories.push_back(filterPool(kDigits));
    }

    int targetLen = (std::max)(6, options.length);
    std::string generated(targetLen, '\0');

    // Ensure at least one character from each selected category
    int pos = 0;
    for (const auto& cat : mandatoryCategories) {
        if (pos < targetLen && !cat.empty()) {
            generated[pos++] = cat[SecureRandom(static_cast<uint32_t>(cat.size()))];
        }
    }

    // Fill the remainder from the general pool
    for (; pos < targetLen; ++pos) {
        generated[pos] = pool[SecureRandom(static_cast<uint32_t>(pool.size()))];
    }

    // Fisher-Yates shuffle
    for (int i = targetLen - 1; i > 0; --i) {
        int j = SecureRandom(i + 1);
        std::swap(generated[i], generated[j]);
    }

    std::wstring widePwd;
    widePwd.reserve(targetLen);
    for (char c : generated) {
        widePwd += static_cast<wchar_t>(c);
    }

    return Assess(widePwd);
}

PasswordAssessment PasswordGenerator::Assess(const std::wstring& password) {
    PasswordAssessment a;
    a.password = password;

    if (password.empty()) {
        a.entropyBits = 0.0;
        a.score = 0;
        a.label = L"空密码";
        a.colorHex = "#64748B";
        a.barPercent = "0%";
        return a;
    }

    bool hasLower = false, hasUpper = false, hasDigits = false, hasSymbols = false;
    for (wchar_t wc : password) {
        if (wc >= L'a' && wc <= L'z') hasLower = true;
        else if (wc >= L'A' && wc <= L'Z') hasUpper = true;
        else if (wc >= L'0' && wc <= L'9') hasDigits = true;
        else hasSymbols = true;
    }

    int poolSize = 0;
    if (hasLower) poolSize += 26;
    if (hasUpper) poolSize += 26;
    if (hasDigits) poolSize += 10;
    if (hasSymbols) poolSize += 33;
    if (poolSize == 0) poolSize = 10;

    double entropy = static_cast<double>(password.size()) * (std::log2(static_cast<double>(poolSize)));
    a.entropyBits = std::round(entropy * 10.0) / 10.0;

    // Score calculation (0 - 100)
    int score = static_cast<int>(entropy);
    if (password.size() < 6) score = (std::min)(score, 20);
    else if (password.size() < 10) score = (std::min)(score, 50);

    score = (std::clamp)(score, 5, 100);
    a.score = score;

    if (score < 35) {
        a.label = L"极弱 (易破解)";
        a.colorHex = "#EF4444"; // Red
        a.barPercent = "25%";
    } else if (score < 55) {
        a.label = L"较弱 (基础防护)";
        a.colorHex = "#F59E0B"; // Amber
        a.barPercent = "45%";
    } else if (score < 75) {
        a.label = L"中等 (常规安全)";
        a.colorHex = "#EAB308"; // Yellow
        a.barPercent = "65%";
    } else if (score < 95) {
        a.label = L"强劲 (高安全)";
        a.colorHex = "#10B981"; // Emerald
        a.barPercent = "85%";
    } else {
        a.label = L"极高安全 (防撞破)";
        a.colorHex = "#06B6D4"; // Cyan
        a.barPercent = "100%";
    }

    return a;
}

} // namespace pwd
