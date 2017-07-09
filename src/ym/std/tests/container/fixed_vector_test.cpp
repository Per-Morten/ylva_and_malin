#include <catch.h>
#include <string>
#include <algorithm>
#include <ym_fixed_vector.h>

SCENARIO("Fixed vectors can be created and destroyed",
         "[fixed vector]")
{
    GIVEN("An initial vector")
    {
        WHEN("A user sends in a nullptr allocator")
        {
            // Depends on build setting, it either asserts, or undefined behavior.
            THEN("The fixed vector might throw an error")
            {
                REQUIRE_THROWS(ym::fixed_vector<int>(25, nullptr));
            }
        }
    }

    GIVEN("Fixed size vector")
    {
        ym::fixed_vector<int> vector(25);
        REQUIRE(vector.size() == 0);
        REQUIRE(vector.capacity() == 25);
        REQUIRE(vector.empty());
    }
}

SCENARIO("Users can add",
         "[fixed vector]")
{
    GIVEN("Empty fixed size vector")
    {
        ym::fixed_vector<int> vector(25);
        REQUIRE(vector.size() == 0);
        REQUIRE(vector.capacity() == 25);

        WHEN("A push_back's are requested")
        {
            vector.push_back(1);
            vector.push_back(2);
            vector.push_back(3);

            THEN("the size is increased")
            {
                REQUIRE(vector.size() == 3);
                REQUIRE([&]
                        {
                            return vector[0] == 1 &&
                                   vector[1] == 2 &&
                                   vector[2] == 3;
                        }());
                REQUIRE(!vector.empty());
            }
        }
    }

    GIVEN("A full fixed size vector")
    {
        ym::fixed_vector<int> vector(3);
        vector.push_back(1);
        vector.push_back(2);
        vector.push_back(3);

        REQUIRE(vector.capacity() == 3);
        REQUIRE(vector.size() == 3);

        WHEN("A push_back is requested")
        {
            REQUIRE_THROWS(vector.push_back(4));

            THEN("The size remains fixed")
            {
                REQUIRE(vector.size() == 3);
                REQUIRE([&]
                        {
                            return vector[0] == 1 &&
                                   vector[1] == 2 &&
                                   vector[2] == 3;
                        }());
                REQUIRE(!vector.empty());
            }
        }
    }
}

SCENARIO("Users can remove data in fixed vectors",
         "[fixed vector]")
{
    GIVEN("An empty vector")
    {
        ym::fixed_vector<std::string> vector(5);

        WHEN("User want to pop back")
        {
            THEN("An error is possibly produced")
            {
                REQUIRE_THROWS(vector.pop_back());
            }
        }
    }

    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vector(5);
        for (ym::size_t i = 0; i < vector.capacity(); ++i)
        {
            vector.push_back(std::to_string(i));
        }

        WHEN("User want to pop back")
        {
            THEN("The size is reduced")
            {
                vector.pop_back();
                REQUIRE(vector.size() == 4);
            }
        }
    }

    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vector(5);
        for (ym::size_t i = 0; i < vector.capacity(); ++i)
        {
            vector.push_back(std::to_string(i));
        }

        WHEN("User want to clear the vector")
        {
            vector.clear();
            THEN("The vector is empty")
            {
                REQUIRE(vector.empty());
                REQUIRE(vector.size() == 0);
            }
        }
    }
}

SCENARIO("Vectors can be copied and moved",
         "[fixed_vector]")
{
    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vec(5);
        for (ym::size_t i = 0; i < vec.capacity(); ++i)
        {
            vec.push_back(std::to_string(1));
        }

        WHEN("A user wants to copy the vector")
        {
            ym::fixed_vector<std::string> vec2(vec);
            THEN("The new vector will contain the same as the first vector")
            {
                REQUIRE(vec2.size() == vec.size());
                REQUIRE(vec2.capacity() == vec.capacity());
                REQUIRE(std::all_of(std::cbegin(vec2),
                                    std::cend(vec2),
                                    [](const auto& item)
                                    {
                                        return item == "1";
                                    }));
            }
        }
    }

    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vec(5, 5, std::to_string(1));

        WHEN("A user wants to copy with custom allocator")
        {
            ym::fixed_vector<std::string> vec2(vec, &ym::memory_globals::default_allocator);
            THEN("The new vector will contain the same as the first vector")
            {
                REQUIRE(vec2.size() == vec.size());
                REQUIRE(vec2.capacity() == vec.capacity());
                REQUIRE(std::all_of(std::cbegin(vec2),
                                    std::cend(vec2),
                                    [](const auto& item)
                                    {
                                        return item == "1";
                                    }));
            }
        }
    }

    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vec(5);
        for (ym::size_t i = 0; i < vec.capacity(); ++i)
        {
            vec.push_back(std::to_string(1));
        }

        WHEN("A user wants to move the vector")
        {
            ym::fixed_vector<std::string> vec2(std::move(vec));
            THEN("The new vector will contain the same as the first vector")
            {
                REQUIRE(vec2.size() == 5);
                REQUIRE(vec2.capacity() == 5);
                REQUIRE(std::all_of(std::cbegin(vec2),
                                    std::cend(vec2),
                                    [](const auto& item)
                                    {
                                        return item == "1";
                                    }));
            }
        }
    }

    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<std::string> vec(5, 5, std::to_string(1));

        WHEN("A user wants to move with custom allocator")
        {
            ym::fixed_vector<std::string> vec2(std::move(vec), &ym::memory_globals::default_allocator);
            THEN("The new vector will contain the same as the first vector")
            {
                REQUIRE(vec2.size() == 5);
                REQUIRE(vec2.capacity() == 5);
                REQUIRE(std::all_of(std::cbegin(vec2),
                                    std::cend(vec2),
                                    [](const auto& item)
                                    {
                                        return item == "1";
                                    }));
            }
        }
    }
}

SCENARIO("Users can access data in fixed vectors",
         "[fixed vector]")
{
    GIVEN("A vector of 5 elements")
    {
        ym::fixed_vector<int> vector(5, 5, 2);
        REQUIRE(vector.size() == 5);
        REQUIRE(vector.capacity() == 5);
        REQUIRE(std::all_of(vector.begin(),
                            vector.end(),
                            [](const auto& item)
                            {
                                return item == 2;
                            }));

        WHEN("User wants to access the 6th element")
        {
            THEN("An error is possibly produced")
            {
                REQUIRE_THROWS(vector[6]);
            }
        }
    }
}
