#pragma once

#include <vector>
#include <algorithm>

template<typename T>
class UComponentSet {
    std::vector<T> mComponents;

public:
    UComponentSet() = default;
    ~UComponentSet() = default;

    void Clear() {
        mComponents.clear();
    }

    uint32_t Add(const T& comp) {
        auto it = std::find(mComponents.begin(), mComponents.end(), comp);

        uint32_t index = std::distance(mComponents.begin(), it);

        if (it == mComponents.end()) {
            mComponents.push_back(comp);
        }

        return index;
    }

    auto begin() const { return mComponents.begin(); }
    auto cbegin() const { return mComponents.cbegin(); }

    auto end() const { return mComponents.end(); }
    auto cend() const { return mComponents.cend(); }
};
