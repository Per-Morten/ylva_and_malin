#include <meta.h>

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <iterator>
#include <vector>

#include <logger.h>

namespace
{
    namespace local
    {
        std::string
        fmt_string(const char* format, ...) noexcept
        {
            std::va_list args1;
            va_start(args1, format);
            std::va_list args2;
            va_copy(args2, args1);
            const int size = std::vsnprintf(nullptr, 0, format, args1) + 1;
            va_end(args1);
            std::vector<char> tmp(size);
            std::vsnprintf(tmp.data(), tmp.size(), format, args2);
            va_end(args2);
            return{ tmp.data() };
        }
    }
}


std::pair<std::string, std::string>
meta::parse(const std::string& string)
{
    // Go through all declarations and tokenize them.

    // Generate to_string function for all items.

    // Can just do += on declarations and definitions.
    return {};
}

// Might be easier to do this with char* actually XD
std::size_t
meta::tokenize_enum(const std::string& string,
                    meta::type_info& info)
{
    info.type = meta::type_info::data_type::enumeration;

    // Buffers for sscanf
    char name_buf[256];
    char value_buf[256];

    // Used as replacement if no value is found within
    int value = 0;

    auto line_start = string.find("{\n") + sizeof("{\n");
    auto line_end = string.find("\n", line_start) + sizeof("\n");
    auto line = string.substr(line_start, line_end - line_start);

    while (line.find("}") == std::string::npos)
    {
        auto errc = std::sscanf(line.c_str(), " %[^, /]s", name_buf);
        if (errc == 1)
        {
            errc = std::sscanf(line.c_str(), " %*s = %[^,/]s", value_buf);
            if (errc != 1)
                std::sprintf(value_buf, "%d", value++);

            info.values.push_back({name_buf, value_buf});
        }

        line_start = line_end;
        line_end = string.find("\n", line_start) + sizeof("\n");
        line = string.substr(line_start, line_end - line_start);
    }

    auto errc = std::sscanf(line.c_str(), " } %[^;]s;", name_buf);
    if (errc != 1)
    {
        LOG_WARN("Could not parse enumeration name, line: %s", line.c_str());
        return {};
    }

    info.name = name_buf;

    return line_end;
}

std::pair<std::string, std::string>
meta::generate_enum_to_string(const meta::type_info& e)
{
    constexpr auto decl_fmt =
    "const char*\n"
    "%s_to_string(%s e);\n";

    std::string decl_str =
        local::fmt_string(decl_fmt, e.name.c_str(), e.name.c_str());


    std::string case_stmt;
    for (const auto& item : e.values)
    {
        case_stmt +=
            local::fmt_string("        case %s:\n"
                              "            return \"%s\";\n",
                              item.key.c_str(), item.key.c_str());
    }

    constexpr auto def_fmt =
    "const char*\n"
    "%s_to_string(%s e)\n{\n"
    "    switch (e)\n"
    "    {\n"
    "%s"
    "    }\n\n"
    "    return \"%s: Unknown Value!\";\n"
    "}\n";

    std::string def_str =
        local::fmt_string(def_fmt,
                          e.name.c_str(),
                          e.name.c_str(),
                          case_stmt.c_str(),
                          e.name.c_str());

    return std::make_pair(decl_str, def_str);
}
