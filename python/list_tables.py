#!/usr/bin/env python
# James Parks

import pathlib
import sqlite3

conn = None
try:
    db_path = '../fletch.db'
    if pathlib.Path(db_path).exists():
        conn = sqlite3.connect('../fletch.db')
        sql_query = """SELECT name FROM sqlite_master WHERE type='table';"""
        cursor = conn.cursor()
        cursor.execute(sql_query)
        print(cursor.fetchall())
    else:
        print(f'DB {db_path} does not exist')
except sqlite3.Error as error:
    print('Failed to execute the above query', error)
finally:
    if conn:
        conn.close()
        print('the sqlite connection is closed')
