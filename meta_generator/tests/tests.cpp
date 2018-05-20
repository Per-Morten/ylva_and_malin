#include <string>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

//D1FA72

#include <meta.h>
#include <logger.h>

bool compare(const meta::type_info& lhs, const meta::type_info& rhs)
{
    if (lhs.type != rhs.type)
        return false;

    if (lhs.name != rhs.name)
        return false;

    if (lhs.values.size() != rhs.values.size())
        return false;

    for (std::size_t i = 0; i < lhs.values.size(); i++)
    {
        if (lhs.values[i].key != rhs.values[i].key ||
            lhs.values[i].value != rhs.values[i].value)
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////
/// ENUMS
///////////////////////////////////////////////////////////
TEST_CASE("Enums with values are tokenized", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success         = (0 << 0),
       ym_errc_invalid_input   = (1 << 0),
       ym_errc_bad_alloc       = (1 << 1),
       ym_errc_mem_leak        = (1 << 2),
       ym_errc_uninitialized   = (1 << 3),
       ym_errc_system_error    = (1 << 4),
       ym_errc_gl_error        = (1 << 5),
       ym_errc_out_of_memory   = (1 << 6),
       ym_errc_container_full  = (1 << 7),
    } ym_errc;
    )delimiter";

    meta::type_info desired;
    desired.type = meta::type_info::data_type::enumeration;
    desired.name = "ym_errc";
    desired.values =
    {
        {"ym_errc_success", "(0 << 0)" },
        {"ym_errc_invalid_input", "(1 << 0)" },
        {"ym_errc_bad_alloc", "(1 << 1)" },
        {"ym_errc_mem_leak", "(1 << 2)" },
        {"ym_errc_uninitialized", "(1 << 3)" },
        {"ym_errc_system_error", "(1 << 4)" },
        {"ym_errc_gl_error", "(1 << 5)" },
        {"ym_errc_out_of_memory", "(1 << 6)" },
        {"ym_errc_container_full", "(1 << 7)" },
    };

    meta::type_info result;
    meta::tokenize_enum(enum_declaration.c_str(), result);
    REQUIRE(compare(result, desired));
}

TEST_CASE("Enums without values are tokenized", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success,
       ym_errc_invalid_input,
       ym_errc_bad_alloc,
       ym_errc_mem_leak,
       ym_errc_uninitialized,
       ym_errc_system_error,
       ym_errc_gl_error,
       ym_errc_out_of_memory,
       ym_errc_container_full,
    } ym_errc;
    )delimiter";

    meta::type_info desired;
    desired.type = meta::type_info::data_type::enumeration;
    desired.name = "ym_errc";
    desired.values =
    {
        {"ym_errc_success", "0" },
        {"ym_errc_invalid_input", "1" },
        {"ym_errc_bad_alloc", "2" },
        {"ym_errc_mem_leak", "3" },
        {"ym_errc_uninitialized", "4" },
        {"ym_errc_system_error", "5" },
        {"ym_errc_gl_error", "6" },
        {"ym_errc_out_of_memory", "7" },
        {"ym_errc_container_full", "8" },
    };

    meta::type_info result;
    meta::tokenize_enum(enum_declaration.c_str(), result);
    REQUIRE(compare(result, desired));
}

TEST_CASE("Enums with comments are tokenized", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success, // success
       ym_errc_invalid_input,
       ym_errc_bad_alloc,
       ym_errc_mem_leak,
       ym_errc_uninitialized,
       ym_errc_system_error,
       ym_errc_gl_error,
       ym_errc_out_of_memory,
       ym_errc_container_full,
    } ym_errc;
    )delimiter";

    meta::type_info desired;
    desired.type = meta::type_info::data_type::enumeration;
    desired.name = "ym_errc";
    desired.values =
    {
        {"ym_errc_success", "0" },
        {"ym_errc_invalid_input", "1" },
        {"ym_errc_bad_alloc", "2" },
        {"ym_errc_mem_leak", "3" },
        {"ym_errc_uninitialized", "4" },
        {"ym_errc_system_error", "5" },
        {"ym_errc_gl_error", "6" },
        {"ym_errc_out_of_memory", "7" },
        {"ym_errc_container_full", "8" },
    };

    meta::type_info result;
    meta::tokenize_enum(enum_declaration.c_str(), result);
    REQUIRE(compare(result, desired));
}

TEST_CASE("Enums with values & comments are tokenized", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success         = (0 << 0),
       ym_errc_invalid_input   = (1 << 0),
       ym_errc_bad_alloc       = (1 << 1),
       ym_errc_mem_leak        = (1 << 2), // leak
       ym_errc_uninitialized   = (1 << 3),
       ym_errc_system_error    = (1 << 4),
       ym_errc_gl_error        = (1 << 5),
       ym_errc_out_of_memory   = (1 << 6),
       ym_errc_container_full  = (1 << 7),
    } ym_errc;
    )delimiter";

    meta::type_info desired;
    desired.type = meta::type_info::data_type::enumeration;
    desired.name = "ym_errc";
    desired.values =
    {
        {"ym_errc_success", "(0 << 0)" },
        {"ym_errc_invalid_input", "(1 << 0)" },
        {"ym_errc_bad_alloc", "(1 << 1)" },
        {"ym_errc_mem_leak", "(1 << 2)" },
        {"ym_errc_uninitialized", "(1 << 3)" },
        {"ym_errc_system_error", "(1 << 4)" },
        {"ym_errc_gl_error", "(1 << 5)" },
        {"ym_errc_out_of_memory", "(1 << 6)" },
        {"ym_errc_container_full", "(1 << 7)" },
    };

    meta::type_info result;
    meta::tokenize_enum(enum_declaration.c_str(), result);
    REQUIRE(compare(result, desired));
}

TEST_CASE("Enums with empty lines are tokenized", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success         = (0 << 0),
       ym_errc_invalid_input   = (1 << 0),
       ym_errc_bad_alloc       = (1 << 1),
       ym_errc_mem_leak        = (1 << 2), // leak

       // A comment
       ym_errc_system_error    = (1 << 4),
       ym_errc_gl_error        = (1 << 5),
       ym_errc_out_of_memory   = (1 << 6),
       ym_errc_container_full  = (1 << 7),
    } ym_errc;
    )delimiter";

    meta::type_info desired;
    desired.type = meta::type_info::data_type::enumeration;
    desired.name = "ym_errc";
    desired.values =
    {
        {"ym_errc_success", "(0 << 0)" },
        {"ym_errc_invalid_input", "(1 << 0)" },
        {"ym_errc_bad_alloc", "(1 << 1)" },
        {"ym_errc_mem_leak", "(1 << 2)" },
        {"ym_errc_system_error", "(1 << 4)" },
        {"ym_errc_gl_error", "(1 << 5)" },
        {"ym_errc_out_of_memory", "(1 << 6)" },
        {"ym_errc_container_full", "(1 << 7)" },
    };

    meta::type_info result;
    meta::tokenize_enum(enum_declaration.c_str(), result);
    REQUIRE(compare(result, desired));
}

TEST_CASE("Enums are given to_string", "[enum]")
{
    const std::string enum_declaration =
    R"delimiter(
    typedef
    enum
    {
       ym_errc_success         = (0 << 0),
       ym_errc_invalid_input   = (1 << 0),
       ym_errc_bad_alloc       = (1 << 1),
       ym_errc_mem_leak        = (1 << 2),
       ym_errc_uninitialized   = (1 << 3),
       ym_errc_system_error    = (1 << 4),
       ym_errc_gl_error        = (1 << 5),
       ym_errc_out_of_memory   = (1 << 6),
       ym_errc_container_full  = (1 << 7),
    } ym_errc;
    )delimiter";

    const std::string def_desired =
    R"delimiter(const char*
ym_errc_to_string(ym_errc e)
{
    switch (e)
    {
        case ym_errc_success:
            return "ym_errc_success";
        case ym_errc_invalid_input:
            return "ym_errc_invalid_input";
        case ym_errc_bad_alloc:
            return "ym_errc_bad_alloc";
        case ym_errc_mem_leak:
            return "ym_errc_mem_leak";
        case ym_errc_uninitialized:
            return "ym_errc_uninitialized";
        case ym_errc_system_error:
            return "ym_errc_system_error";
        case ym_errc_gl_error:
            return "ym_errc_gl_error";
        case ym_errc_out_of_memory:
            return "ym_errc_out_of_memory";
        case ym_errc_container_full:
            return "ym_errc_container_full";
    }

    return "ym_errc: Unknown Value!";
}

)delimiter";

    const std::string dec_desired =
    "const char*\nym_errc_to_string(ym_errc e);\n\n";

    meta::type_info e;
    meta::tokenize_enum(enum_declaration.c_str(), e);
    const auto result = meta::generate_enum_to_string(e);

    REQUIRE(result.second == def_desired);
    REQUIRE(result.first == dec_desired);
}

// // Think if I should actually support this.
// TEST_CASE("Enums are given is_valid with given expression", "[enum]")
// {
//     const std::string enum_declaration =
//     R"delimiter(
//     typedef
//     enum
//     {
//        ym_errc_success         = (0 << 0),
//        ym_errc_invalid_input   = (1 << 0),
//        ym_errc_bad_alloc       = (1 << 1),
//        ym_errc_mem_leak        = (1 << 2),
//        ym_errc_uninitialized   = (1 << 3),
//        ym_errc_system_error    = (1 << 4),
//        ym_errc_gl_error        = (1 << 5),
//        ym_errc_out_of_memory   = (1 << 6),
//        ym_errc_container_full  = (1 << 7),
//     } ym_errc;
//     )delimiter";

//     const std::string desired =
//     R"delimiter(
//     bool
//     ym_errc_to_string(ym_errc errc)
//     {
//         return (errc == (0 << 0)) ||
//                (errc == (1 << 0)) ||
//                (errc == (1 << 1)) ||
//                (errc == (1 << 2)) ||
//                (errc == (1 << 3)) ||
//                (errc == (1 << 4)) ||
//                (errc == (1 << 5)) ||
//                (errc == (1 << 6)) ||
//                (errc == (1 << 7));
//     }
//     )delimiter";

//     //const auto result = meta::parse_enum(enum_declaration);

//     //REQUIRE(result == desired);
// }

// TEST_CASE("Enums are given is_valid without given expression", "[enum]")
// {

// }

// ///////////////////////////////////////////////////////////
// /// STRUCTS Currently waiting with this,
// /// until I have more structs, so I can better see what
// /// Needs to be done.
// ///////////////////////////////////////////////////////////
// TEST_CASE("Struct is tokenized", "[struct]")
// {

// }



// ///////////////////////////////////////////////////////////
// /// PRIMITIVE STRUCTS
// ///////////////////////////////////////////////////////////


// TEST_CASE("Struct containing only primitives to_string", "[primitive struct]")
// {

// }

// TEST_CASE("Struct containing only primitives print", "[primitive struct]")
// {

// }

// ///////////////////////////////////////////////////////////
// /// COMPLEX STRUCTS
// ///////////////////////////////////////////////////////////
// TEST_CASE("Struct containing ym definition to_string", "[complex struct]")
// {

// }

// TEST_CASE("Struct containing ym definition print")
// {

// }

///////////////////////////////////////////////////////////
/// COMPLETE
///////////////////////////////////////////////////////////
TEST_CASE("Combine complete H file", "[total]")
{
    const std::string h_file =
    R"delimiter(#pragma once
typedef
enum
{
   ym_errc_success         = (0 << 0),
   ym_errc_invalid_input   = (1 << 0),
   ym_errc_bad_alloc       = (1 << 1),
   ym_errc_mem_leak        = (1 << 2),
   ym_errc_uninitialized   = (1 << 3),
   ym_errc_system_error    = (1 << 4),
   ym_errc_gl_error        = (1 << 5),
   ym_errc_out_of_memory   = (1 << 6),
   ym_errc_container_full  = (1 << 7),
} ym_errc;

typedef uint u8;

typedef
enum
{
    ym_alloc_strategy_stack,
    ym_alloc_strategy_pool,
    ym_alloc_strategy_linear,
    ym_alloc_strategy_region, // Make this buddy allocator in future, currently its fixed list

    // Add strategies for aligned allocations?
} ym_alloc_strategy;

)delimiter";

    const std::string desired =
    R"delimiter(#pragma once
const char*
ym_errc_to_string(ym_errc e);

const char*
ym_alloc_strategy_to_string(ym_alloc_strategy e);

)delimiter";

    const auto result = meta::parse("ym_errc", h_file.c_str());
    REQUIRE(result.first.contents == desired);
    REQUIRE(result.first.filename == "ym_errc_meta.h");
}

TEST_CASE("Combine complete cpp file", "[total]")
{
    const std::string h_file =
R"delimiter(#pragma once
typedef
enum
{
   ym_errc_success         = (0 << 0),
   ym_errc_invalid_input   = (1 << 0),
   ym_errc_bad_alloc       = (1 << 1),
   ym_errc_mem_leak        = (1 << 2),
   ym_errc_uninitialized   = (1 << 3),
   ym_errc_system_error    = (1 << 4),
   ym_errc_gl_error        = (1 << 5),
   ym_errc_out_of_memory   = (1 << 6),
   ym_errc_container_full  = (1 << 7),
} ym_errc;

typedef uint u8;

typedef
enum
{
    ym_alloc_strategy_stack,
    ym_alloc_strategy_pool,
    ym_alloc_strategy_linear,
    ym_alloc_strategy_region, // Make this buddy allocator in future, currently its fixed list

    // Add strategies for aligned allocations?
} ym_alloc_strategy;

)delimiter";


    const std::string def_desired =
    R"delimiter(#include <ym_errc_meta.h>
const char*
ym_errc_to_string(ym_errc e)
{
    switch (e)
    {
        case ym_errc_success:
            return "ym_errc_success";
        case ym_errc_invalid_input:
            return "ym_errc_invalid_input";
        case ym_errc_bad_alloc:
            return "ym_errc_bad_alloc";
        case ym_errc_mem_leak:
            return "ym_errc_mem_leak";
        case ym_errc_uninitialized:
            return "ym_errc_uninitialized";
        case ym_errc_system_error:
            return "ym_errc_system_error";
        case ym_errc_gl_error:
            return "ym_errc_gl_error";
        case ym_errc_out_of_memory:
            return "ym_errc_out_of_memory";
        case ym_errc_container_full:
            return "ym_errc_container_full";
    }

    return "ym_errc: Unknown Value!";
}

const char*
ym_alloc_strategy_to_string(ym_alloc_strategy e)
{
    switch (e)
    {
        case ym_alloc_strategy_stack:
            return "ym_alloc_strategy_stack";
        case ym_alloc_strategy_pool:
            return "ym_alloc_strategy_pool";
        case ym_alloc_strategy_linear:
            return "ym_alloc_strategy_linear";
        case ym_alloc_strategy_region:
            return "ym_alloc_strategy_region";
    }

    return "ym_alloc_strategy: Unknown Value!";
}

)delimiter";

    const auto result = meta::parse("ym_errc", h_file.c_str());
    REQUIRE(result.second.contents == def_desired);
    REQUIRE(result.second.filename == "ym_errc_meta.cpp");
}

