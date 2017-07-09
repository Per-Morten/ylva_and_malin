#pragma once
#include <cstddef>
#include <cstdint>

namespace ym
{
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using ufast8 = std::uint_fast8_t;
    using ufast16 = std::uint_fast16_t;
    using ufast32 = std::uint_fast32_t;
    using ufast64 = std::uint_fast64_t;

    using ifast8 = std::int_fast8_t;
    using ifast16 = std::int_fast16_t;
    using ifast32 = std::int_fast32_t;
    using ifast64 = std::int_fast64_t;

    using size_t = std::size_t;
    using ptrdiff_t = std::ptrdiff_t;
    using iptr_t = std::intptr_t;
    using uptr_t = std::uintptr_t;

    using byte = ym::u8;

    using f32 = float;
    using f64 = double;
}
