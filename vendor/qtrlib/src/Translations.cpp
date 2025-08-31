#include "Translations.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Translations {
    constexpr const char* translationsFolder = "Data/SKSE/Plugins/" PRODUCT_NAME "Strings.json";
    const char* defaultTranslation = "missing translation";
    static inline std::map<std::string, const char*> translations;
}


void Translations::Install() {
    std::ifstream file(translationsFolder);
    nlohmann::json j;
    file >> j;
    logger::trace("reading translation");
    if (!j.is_object()) {
        logger::trace("translation json: {} must be an object", translationsFolder);
    }
    for (auto& [key, value] : j.items()) {
        logger::trace("{} -> {}", key, value);
        std::string v = value;
        translations[key] = strdup(v.c_str());
    }
}

const const char* Translations::Get(std::string key) {
    IF_FIND(translations, key, it) {
        return it->second;
    }
    return defaultTranslation;
}
