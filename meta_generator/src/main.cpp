#include <meta.h>
#include <logger.h>
#include <string>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
//namespace fs = std::filesystem;

// input:
//  directory_name
//  cache_path(?)
//  output_directory
// output:
//  .h files,
//  .cpp files,
//  CMakeLists.txt

// TODO:
// - Parallelize this by giving each thread a top directory
// - Remove all files that does not exist in cache or in directory
int
main(int argc, char** argv)
{
    const auto in_dir = "c:/Users/PMS/Documents/other_programming/ylva_and_malin/src/ym/";
    const auto cache_path = "";
    const std::string out_dir = "c:/Users/PMS/Documents/other_programming/ylva_and_malin/src/ym/generated/";

    std::vector<fs::path> files;
    for (auto& p : fs::recursive_directory_iterator(in_dir))
    {
        if (p.path().extension() == ".h" &&
            p.path().filename() != "glext.h" &&
            p.path().filename() != "wglext.h")
        {
            files.push_back(p);
        }
    }

    for (const auto& file : files)
    {
        auto fp = std::fopen(file.generic_string().c_str(), "r");
        if (fp)
        {
            std::string contents;
            std::fseek(fp, 0, SEEK_END);
            contents.resize(std::ftell(fp));
            std::rewind(fp);
            std::fread(&contents[0], 1, contents.size(), fp);
            std::fclose(fp);

            auto filename = file.filename().generic_string();
            filename.pop_back();
            filename.pop_back();

            auto[header, source] = meta::parse(filename.c_str(), contents.c_str());

            const auto write_to_file = [&out_dir](const std::string& filename,
                                                  const std::string& contents)
            {
                const auto path = out_dir + filename;
                auto outfile = std::fopen(path.c_str(), "w+");
                std::fwrite(contents.data(), sizeof(char), contents.size(), outfile);
                std::fclose(outfile);
            };

            if (!header.contents.empty())
            {
                write_to_file(header.filename, header.contents);
                write_to_file(source.filename, source.contents);
            }
        }
    }

    for (const auto& item : files)
        printf("%s\n", item.c_str());

    system("pause");

    return 0;
}
