#!/usr/bin/env python

import os
import json
import pathlib
import time
import logging
from functools import wraps
import sqlite3

_logger = logging.getLogger(__name__)


def make_table(table_name):
    sql = f"CREATE TABLE {table_name}(" \
          "ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
          "NAME           TEXT NOT NULL, " \
          "SKIN           TEXT NOT NULL, " \
          "TYPE           TEXT NOT NULL, " \
          "POSITION       TEXT, " \
          "PARENT         TEXT);"
    return sql

def insert(name, skin, type, position, parent):
    sql = f"INSERT INTO metadata (ID, NAME, SKIN, TYPE, POSITION, PARENT) VALUES (NULL, '{name}', '{skin}', '{type}', '{position}', '{parent}');"
    return sql

def process_stmt(file_path, sql_stmt):
    conn = sqlite3.connect(file_path)
    cur = conn.cursor()
    cur.execute(sql_stmt)
    conn.commit()

def timer(f):
    @wraps(f)
    def wrapper_time(*args, **kwargs):
        start_time = time.perf_counter()
        value = f(*args, **kwargs)
        end_time = time.perf_counter()
        run_time = end_time - start_time
        print('func {} took {}us'.format(f.__name__, run_time*1000000))
        return value
    return wrapper_time

def walkit(start_dir:str = '', db_name:str = 'test.db'):
    conn = sqlite3.connect(db_name)
    make_table_stmt = make_table(table_name='metadata')
    process_stmt(db_name, make_table_stmt)
    for (root,dirs,files) in os.walk(start_dir, topdown=True): 
        for this_file in files:
            if this_file.endswith('.json'):
                print(pathlib.Path(root).joinpath(this_file))
                with open(pathlib.Path(root).joinpath(this_file), 'r') as fp:
                    contents = json.load(fp)
                insert_stmt = insert(name=contents['name'],
                                     skin=contents['skin'],
                                     type=contents['type'],
                                     position=contents['position'],
                                     parent=contents['parent'])
                process_stmt(file_path=db_name, sql_stmt=insert_stmt)
                for key in contents:
                    print(f'\t{key} :: {contents[key]}')
            for this_dir in dirs:
                walkit(start_dir=this_dir)

@timer
def main():
    walkit(start_dir='.')

if __name__ == '__main__':
    main()
