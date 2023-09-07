//
// Created by swift on 2021/2/24.
//

#pragma once


#include "assert.h"

#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <atomic>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <bitset>
#include <queue>
#include <stack>
#include <type_traits>
#include <unistd.h>
#include <iosfwd>
#include <fstream>

namespace SandHook {

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using u128 = std::array<u64, 2>;

    using s8 = std::int8_t;
    using s16 = std::int16_t;
    using s32 = std::int32_t;
    using s64 = std::int64_t;

    using f32 = float;
    using f64 = double;
    using f128 = u128;

    const auto page_size = getpagesize();

    using VAddr = size_t;

    using LockGuard = std::lock_guard<std::mutex>;

    using RecursiveGuard = std::lock_guard<std::recursive_mutex>;

    template<typename T>
    constexpr T AlignUp(T value, size_t size) {
        auto mod{static_cast<T>(value % size)};
        value -= mod;
        return static_cast<T>(mod == T{0} ? value : value + size);
    }

    template<typename T>
    constexpr T AlignDown(T value, size_t size) {
        return static_cast<T>(value - value % size);
    }

    inline std::ostream& NullStream() {
        static std::ostream bitBucket(0);
        return bitBucket;
    }
}
