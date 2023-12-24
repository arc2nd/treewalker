CC = g++
# INCLUDES = /home/james/Downloads/boost_1_83_0
# LIBRARIES = /home/james/Downloads/boost_1_83_0/stage/lib
BOOST_INC = /usr/include/boost
INC = /usr/include
LIB = /usr/lib
.DEFAULT_GOAL := all
.PHONY := all walker filer jsoner sqler clean

all: walker filer jsoner sqler

walker:
	${CC} -o walker src/treewalker.cpp -I${BOOST_INC} -L${LIB} -lboost_filesystem -lboost_system -lboost_regex

filer:
	${CC} -o filer src/treewalker_file.cpp -I${BOOST_INC} -L${LIB} -lboost_filesystem -lboost_system -lboost_regex

jsoner:
	${CC} -o jsoner src/treewalker_json.cpp -I${BOOST_INC} -L${LIB} -lboost_filesystem -lboost_system -lboost_regex -lboost_json 

sqler:
	${CC} -o sqler src/treewalker_sqlite2.cpp -I${INC} -I${BOOST_INC} -L${LIB} -lboost_filesystem -lboost_system -lboost_regex -lboost_json -lsqlite3

clean:
	rm walker filer jsoner sqler test.db
