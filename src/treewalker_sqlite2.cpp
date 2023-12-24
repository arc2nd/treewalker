// James Parks
// 12-16-23

/*
The intent of this program is to walk a directory structure and look for all .json files
Any found json files will be opened and read in
The contents of those files will be parsed into JSON objects
The data will then be fed into an sqlite3 database file

Other access to the database will probably be through Python
C++ was chosen for the creation of the database due to speed requirements
*/

// TODO: The whole things needs some error-checking
// TODO: I'd like to find a better way of changing the JSON schema

// 3rd party library includes
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "boost/json.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "sqlite3.h"

// std includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <typeinfo>

// namespaces
using namespace std::chrono;
using namespace std;
namespace fs = boost::filesystem;
namespace json = boost::json;
namespace ptree = boost::property_tree;

// a struct in which we will keep our decoded json metadata
struct Metadata
{
    std::string name;
    std::string skin;
    std::string type;
    std::vector<float> position;
    std::string parent;
};

template <typename T>
std::vector<T> as_vector(ptree::ptree const& pt, ptree::ptree::key_type const& key)
{
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}

// the function to read a file into text
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

// not quite sure what this does yet, I copied it directly
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

// a function to create the sqlite3 database file
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

// a function to formulate the sql statement to make the metadata table
// TODO: Replace the hardcoded table name with a variable
const char * make_table()
{
    const char *sql;

    sql = "CREATE TABLE metadata("
          "ID INTEGER PRIMARY KEY NOT NULL, "
          "NAME           TEXT NOT NULL, "
          "SKIN           TEXT NOT NULL, "
          "TYPE           TEXT NOT NULL, "
          "POSITION       TEXT, "
          "PARENT         TEXT);";
    return sql;
}

// a function to forumulate the sql statement to insert a row into the metadata table
// TODO: have to find a way to convert vector<float> into my position string
std::string insert(Metadata& ts)
{
    std::string preamble;
    std::string position = "[1.0, 2.0, 3.0]";
    std::string query;

    preamble = "INSERT INTO METADATA (ID, NAME, SKIN, TYPE, POSITION, PARENT) VALUES (NULL, '";
    query.append(preamble);
    query.append(ts.name);
    query.append("', '");
    query.append(ts.skin);
    query.append("', '");
    query.append(ts.type);
    query.append("', '");
    query.append(position);
    query.append("', '");
    query.append(ts.parent);
    query.append("');");
    return query;
}

// process an SQL statement into the sqlite3 database
// TODO: This particularly needs error checking to make sure statements work properly
int process_stmt(const char* file_path, const char* sql_stmt)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(file_path, &db);

    rc = sqlite3_exec(db, sql_stmt, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}

int main(int argc, char* argv[])
{
    auto start = high_resolution_clock::now();
    const char* db_name = "test.db";
    if (argc > 1)
    {
        db_name = argv[1];
    } // end if more than one argument

    // Make the metadata table
    const char *make_table_stmt = make_table();
    int result;
    result = process_stmt(db_name, make_table_stmt);

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

            // get key:values pairs from json property tree
            ptree::ptree doc;
            ptree::read_json(file_path, doc);
            // std::cout << file_path << std::endl;

            // load up the struct with the json info
            Metadata my_struct;
            my_struct.name = doc.get<std::string>("name");
            my_struct.skin = doc.get<std::string>("skin");
            my_struct.type = doc.get<std::string>("type");
            my_struct.parent = doc.get<std::string>("parent");
            my_struct.position = as_vector<float>(doc, "position");

            // create and process each insert statement
            // std::cout << my_struct.name << std::endl;
            std::string insert_stmt = insert(my_struct);
            result = process_stmt(db_name, insert_stmt.c_str());

        } // end if regex
    } // end for recursive directory iterator: looking for files
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
} // end main
