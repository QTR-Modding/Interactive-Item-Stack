#pragma once

namespace InputConfig {
    void Install();
    std::vector<std::pair<RE::INPUT_DEVICE, uint32_t>> Get(const std::string& key);
}