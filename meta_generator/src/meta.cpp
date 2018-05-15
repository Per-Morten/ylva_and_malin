#include <meta.h>

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <vector>

#include <logger.h>

namespace
{
    namespace local
    {
        std::string
        fmt(const char* format, ...) noexcept
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

std::pair<meta::file, meta::file>
meta::parse(const char* filename,
            const char* string)
{
    // Go through all declarations and tokenize them.
    char type_buffer[256];

    std::vector<meta::type_info> types;

    for (auto itr = std::strstr(string, "typedef"); itr;)
    {
        auto errc = std::sscanf(itr, " typedef %s", type_buffer);
        if (errc != 1)
        {
            LOG_WARN("Could note decipher typedef");
            return {};
        }

        // To ensure that we skip to the next typedef the next time we search.
        // Won't impact tokenizing, as it need to get to the opening { anyway
        itr += sizeof("typedef");

        if (!std::strcmp(type_buffer, "enum"))
        {
            types.push_back({});
            itr = tokenize_enum(itr, types.back());
        }

        itr = std::strstr(itr, "typedef");
    }

    std::string declarations = "#pragma once\n";
    std::string definitions = local::fmt("#include <%s_meta.h>\n",
                                         filename);
    // Generate to_string function for all items.
    for (const auto& item : types)
    {
        if (item.type == meta::type_info::enumeration)
        {
            auto[decl, def] = generate_enum_to_string(item);
            declarations += decl;
            definitions += def;
        }
    }

    // Create filenames
    file header = {local::fmt("%s_meta.h", filename), declarations};
    file cpp = {local::fmt("%s_meta.cpp", filename), definitions};

    return {header, cpp};
}

// Returns pointer to the place where parsing should continue.
char*
meta::tokenize_enum(const char* string,
                    meta::type_info& info)
{
    info.type = meta::type_info::enumeration;

    // Buffers for sscanf
    char name_buf[256];
    char value_buf[256];

    // Used as replacement if no value is found within
    int value = 0;

    auto enum_start = std::strstr(string, "{\n") + sizeof("{\n");
    auto enum_stop = std::strchr(enum_start, '}');

    while (enum_start < enum_stop)
    {
        auto errc = std::sscanf(enum_start, " %[^, /}]s", name_buf);
        if (errc == 1)
        {
            errc = std::sscanf(enum_start, " %*s = %[^,/]s", value_buf);
            if (errc != 1)
                std::sprintf(value_buf, "%d", value++);

            info.values.push_back({name_buf, value_buf});
        }

        enum_start = std::strchr(enum_start, '\n') + sizeof('\n');
    }

    auto errc = std::sscanf(enum_stop, " } %[^;]s;", name_buf);
    if (errc != 1)
    {
        LOG_WARN("Could not parse enumeration name, line: %s", enum_stop);
        return {};
    }

    info.name = name_buf;

    return enum_stop;
}

std::pair<std::string, std::string>
meta::generate_enum_to_string(const meta::type_info& e)
{
    constexpr auto decl_fmt =
    "const char*\n"
    "%s_to_string(%s e);\n\n";

    std::string decl_str =
        local::fmt(decl_fmt, e.name.c_str(), e.name.c_str());


    std::string case_stmt;
    for (const auto& item : e.values)
    {
        case_stmt +=
            local::fmt("        case %s:\n"
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
    "}\n\n";

    std::string def_str =
        local::fmt(def_fmt,
                   e.name.c_str(),
                   e.name.c_str(),
                   case_stmt.c_str(),
                   e.name.c_str());

    return std::make_pair(decl_str, def_str);
}
