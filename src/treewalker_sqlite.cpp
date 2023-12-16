#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "boost/json.hpp"
#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

using namespace std::chrono;
using namespace std;
namespace fs = boost::filesystem;
namespace json = boost::json;

struct Metadata
{
    std::string name;
    std::string skin;
    std::string type;
    std::vector<float> position;
    std::string parent;
};

void read_file(std::string const& file_path, std::string & contents)
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

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void create_db(const char* file_path)
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(file_path, &db);

    if (rc)
    {
        std::cout << "Can't open dabatase: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Database opened" << std::endl;
    }
    
    sqlite3_close(db);
}

char make_table()
{
    char *sql;

    sql = "CREATE TABLE metadata(" \
          "ID INT PRIMARY KEY  NOT NULL, " \
          "NAME           TEXT NOT NULL, " \
          "SKIN           TEXT NOT NULL, " \
          "TYPE           TEXT NOT NULL, " \
          "POSITION       TEXT, " \
          "PARENT         TEXT);";
    return *sql;
}

char insert(std::string name, std::string skin, std::string type, std::string position, std::string parent)
{
    char *sql;

    sql = "INSERT INTO METADATA (ID, NAME, SKIN, TYPE, POSITION, PARENT) "\
          "VALUES (1, name, skin, type, position, parent);";
    return *sql;
}

void process_stmt(const char* file_path, const char & sql_stmt)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(file_path, &db);

    rc = sqlite3_exec(db, &sql_stmt, callback, 0, &zErrMsg);
}

int main(int argc, char* argv[])
{
    auto start = high_resolution_clock::now();
    const char* db_name = "test.db";
    if (argc > 1)
    {
        db_name = argv[1];
    } // enf if more than one argument
    std::cout << db_name << std::endl;
    char make_table_stmt = make_table();
    process_stmt(db_name, make_table_stmt);

    fs::path current_dir(".");  // start hunting at our current path
    boost::regex pattern("(.*\\.json)");  // regex pattern for *.json files
    for (fs::recursive_directory_iterator iter(current_dir), end; iter !=end; ++iter)
    {
        std::string name = iter->path().filename().string(); // just the basename 
        std::string file_path = iter->path().string(); // the full file path
        if (regex_match(name, pattern))
        {
            // here's where you insert the fun stuff that you're going to do with 
            // the file that you've found
            std::cout << file_path << std::endl;
            //std::cout << "    " << file_size(iter->path()) << std::endl;

            // open the file and read it into a string
            std::string contents; // the empty string used to store the contents
            read_file(file_path, contents);

            // parse contents of file into a json 
            json::value jv = json::parse(contents);

            // get key:values pairs from json value
            switch(jv.kind())
            {
                case json::kind::object:
                {
                    // find by specific key
                    auto const& obj = jv.get_object();
                    // auto name_key = obj.find("name");
                    // std::cout << name_key->value() << endl;

                    // iterate over all the keys
                    if (! obj.empty())
                    {
                        auto it = obj.begin();
                        for (;;)
                        {
                            // here's where you do things with the individual key/value pairs
                            std::cout << "\t" << json::serialize(it->key()) << " :: " << it->value() << endl;
                            if (++it == obj.end())
                            {
                                break;
                            } // end if we've reached the end of the json
                        } // end for json iterator
                    } // end if not an empty object
                    break;
                }// end case
            } // end switch
        } // end if regex
    } // end for recursive directory iterator: looking for files
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
} // end main
