#pragma once

#include <string>
#include <vector>

namespace pwd {

enum class GeneratorMode {
    Custom,
    HighSecurity, // 24-char mixed
    Balanced,     // 16-char mixed
    Pin,          // 6-digit PIN
    Memorable,    // 4-word passphrase
    NoAmbiguous   // 16-char without 1/l/I/0/O
};

struct GeneratorOptions {
    int length = 16;
    bool includeUpper = true;
    bool includeLower = true;
    bool includeDigits = true;
    bool includeSymbols = true;
    bool excludeAmbiguous = false;
    GeneratorMode mode = GeneratorMode::Balanced;
};

struct PasswordAssessment {
    std::wstring password;
    double entropyBits = 0.0;
    int score = 0;              // 0 - 100
    std::wstring label;         // 极弱 / 较弱 / 良好 / 强劲 / 极高安全
    std::string colorHex;       // Hex color for UI badge/bar
    std::string barPercent;     // e.g. "85%"
};

class PasswordGenerator {
public:
    static PasswordAssessment Generate(const GeneratorOptions& options);
    static PasswordAssessment Assess(const std::wstring& password);

    // Cryptographically secure random number in [0, maxExclusive)
    static uint32_t SecureRandom(uint32_t maxExclusive);

private:
    static std::wstring GenerateMemorablePassphrase();
};

} // namespace pwd
