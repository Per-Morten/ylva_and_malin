#pragma once
#include <string>
#include <utility>
#include <vector>

namespace meta
{
    struct type_info
    {
        enum data_type
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

    struct file
    {
        std::string filename;
        std::string contents;
    };

    // Return a pair of .h and .cpp file?
    std::pair<file, file>
    parse(const char* filename,
          const char* string);

    const char*
    tokenize_enum(const char* string,
                  type_info& info);

    std::pair<std::string, std::string>
    generate_enum_to_string(const type_info& e);
}
