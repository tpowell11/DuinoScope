# -*- coding: utf-8 -*-
"""
Created on Thu Apr 30 09:39:14 2020

@author: thoma
"""
import mmap
import linecache

foundLines = []
with open('db.txt', 'rb', 0) as file, \
     mmap.mmap(file.fileno(), 0, access=mmap.ACCESS_READ) as s:
    if s.find(b'2') != -1:
        print('true')

for i in range (len(foundLines)) : 
    print(linecache.getline('db.txt',foundLines[i]))