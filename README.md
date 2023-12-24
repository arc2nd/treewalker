# What

Treewalker will walk a directory tree. It will find all .json files (this may later be changed to "metadata.json" files.
It will read those files in as test. It will then parse that text as JSON values into a custom struct. It will then 
create an sqlite3 database and put the contents of each of those json files as an entry in the database.

# Why

We want to walk a directory tree and create an ephemeral database of the contents of all the "metadata.json" files that 
we find therein. We are using C++ to do this for speed reasons. Python will most likely be used to query the database 
once all the files have been parsed.

# How

We are using the boost library for walking the directory tree and parsing the JSONs. 
We are using the sqlite3 library for the database creation and SQL execution.

The makefile and python scripts are configured so that I can work out of the top level folder. 
It holds targets for:

* walker

    simply walk a directory tree

* filer

    walk the directory tree and look for \*.json files

* jsoner

    open and parse those JSON files

* sqler

    put the data you find in the JSON into an sqlite3 database

* clean

    cleanup compiled files and the test.db file


# TODOs
1. Add any sort of error-checking or validation
2. Ideally there would be a way to change the JSON schema without re-writing the whole thing
3. Clean up the unused includes
4. Convert the make\_table function to take a table name as an input
5. Change over from make to Cmake
6. Figure out why the treewalking routine is in seemingly random order
