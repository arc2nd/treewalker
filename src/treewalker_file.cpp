#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
// #include "boost/json.hpp"
// #include "boost/json/src.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
namespace fs = boost::filesystem;
// using namespace boost::json;

void read_file(std::string & file_path, std::string & contents)
{
    std::ifstream this_file (file_path);
    if (this_file.is_open())
    {
        this_file.seekg(0, std::ios::end);
        contents.reserve(this_file.tellg());
        this_file.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(this_file)), 
                         std::istreambuf_iterator<char>());
    }
}

int main()
{
    fs::path current_dir(".");  // start hunting at our current path
    boost::regex pattern("(.*\\.json)");  // regex pattern for *.json files
    for (fs::recursive_directory_iterator iter(current_dir), end; iter !=end; ++iter)
    {
        std::string name = iter->path().filename().string();
        std::string file_path = iter->path().string();
        if (regex_match(name, pattern))
        {
            // here's where you insert the fun stuff that you're going to do with 
            // the file that you've found
            std::cout << file_path << std::endl;
            std::cout << "    " << file_size(iter->path()) << std::endl;

            // open the file and read it into a string
            std::string contents;
            read_file(file_path, contents);

            // output
            std::cout << contents << endl;

        }
    }
}


