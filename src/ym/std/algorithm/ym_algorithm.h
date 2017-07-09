#pragma once
#include <type_traits>
#include <iterator>
#include <memory>
#include <cstring>

namespace ym
{
    template<class InputIt,
             class OutputIt>
    InputIt
    copy(InputIt begin,
         InputIt end,
         OutputIt d_begin)
    {
        printf("Copy normal\n");
        while (begin != end)
            *d_begin++ = *begin++;

        return d_begin;
    }

    template<class InputIt,
             class OutputIt,
             typename std::enable_if_t
                <
                    std::is_pointer_v<OutputIt> &&
                    std::is_trivially_copyable_v<std::iterator_traits<OutputIt>::value_type> &&
                    std::is_same_v
                    <
                        std::remove_const_t<typename std::iterator_traits<InputIt>::value_type>,
                        std::iterator_traits<OutputIt>::value_type
                    >
                >
            >
    OutputIt
    copy(InputIt begin,
         InputIt end,
         OutputIt d_begin)
    {
        printf("Copy memmove\n");
        const auto distance = static_cast<ym::size_t>(std::distance(begin, end));
        if (distance > 0)
            std::memmove(d_begin, begin, distance * sizeof(std::iterator_traits<InputIt>::value_type));
        return d_begin;
    }


}
