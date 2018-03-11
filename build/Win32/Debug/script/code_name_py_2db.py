#coding=utf-8

import os
import sys
import sqlite3

import pypinyin
from pypinyin import pinyin, lazy_pinyin

import tushare as ts
 
DB_FILE_PATH = "C:/Users/wwang.ZDST/Desktop/TradeWinner_run/TradeWinner/pzwj.kd"
 
def is_ascii(s):
    return all(ord(c) < 128 for c in s)
    
def getpinyinhead(str):
    ""
    str = str.replace(' ', '')
    head_arry = []
    for s in str:
        if not is_ascii(s) :
            tmp = lazy_pinyin(s) 
            print(tmp[0][0]) 
            head_arry.append(lazy_pinyin(s)[0][0].upper())
            print(s)
        else:
            head_arry.append(s.upper())   
    return head_arry
    
def open_db():
    ''' open data base 保存数据库'''
    global DB_FILE_PATH
    global g_db_conn
    if not os.access(DB_FILE_PATH, os.F_OK):
        print ("%s not exist\n" % DB_FILE_PATH)
        os._exit(0)
    g_db_conn = sqlite3.connect(DB_FILE_PATH, detect_types=sqlite3.PARSE_COLNAMES)
    if g_db_conn is not None:
        print ("opened database successfully")
    else:
        print ("opened database fail!")
        os._exit(0)
    g_db_conn.text_factory = lambda x : str(x, 'utf-8', 'ignore')
    
def close_db():
    global g_db_conn
    g_db_conn.commit()
    g_db_conn.close() 
    
def main(): 
    global g_db_conn
    open_db()
    stocks_list = []
    cur = g_db_conn.cursor()
    
    stock_info = ts.get_stock_basics()
    
    for i in range(0, len(stock_info.index)):
        #print( "%s %s" % (stock_info.index[i], stock_info.name[i]) )
        py_str = getpinyinhead(stock_info.name[i])
        sql = "INSERT INTO Stock VALUES(?, ?, ?, '', '') "  
        cur.execute(sql, (stock_info.index[i], stock_info.name[i], ''.join(py_str))) 
        print(sql)
      
    g_db_conn.commit()    
    close_db()
    
if __name__ == '__main__':
    main()