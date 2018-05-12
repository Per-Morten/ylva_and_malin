#pragma once
#include <string>
#include <utility>
#include <vector>

namespace meta
{
    struct type_info
    {
        enum struct data_type
        {
            enumeration,
            structure,
        };

        struct key_value
        {
            std::string key;
            std::string value;
        };

        data_type type;
        std::string name;
        std::vector<key_value> values;
    };

    // Return a pair of .h and .cpp file?
    std::pair<std::string, std::string>
    parse(const std::string& string);

    std::size_t
    tokenize_enum(const std::string& string,
                  type_info& info);

    std::pair<std::string, std::string>
    generate_enum_to_string(const type_info& e);
}
