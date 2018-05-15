#include <meta.h>
#include <logger.h>
#include <string>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

// input:
//  directory_name
//  cache_path(?)
//  output_directory
// output:
//  .h files,
//  .cpp files,
//  CMakeLists.txt

// Parallelize this by giving each thread a top directory
int
main(int argc, char** argv)
{
    auto env = std::getenv("YM_SOURCE_DIR");

    for(auto& p: fs::recursive_directory_iterator(env))
        printf("%s\n", p.path().c_str());
        //std::cout << p << '\n';


    // if (env)
    //     LOG_DEBUG("Yay: %s", env);

    // std::string path = "../../src/ym/";
    // std::string command = "cd " + path;
    // system(path.c_str());

    return 0;
}
