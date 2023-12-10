#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include <iostream>

using namespace std;
using namespace boost::filesystem;

int main()
{
    path current_dir(".");
    boost::regex pattern("(.*\\.json)");
    for (recursive_directory_iterator iter(current_dir), end; iter !=end; ++iter)
    {
        std::string name = iter->path().filename().string();
        if (regex_match(name, pattern))
        {
            // here's where you insert the fun stuff that you're going to do with 
            // the file that you've found
            std::cout << iter->path() << std::endl;
        }
    }
}


