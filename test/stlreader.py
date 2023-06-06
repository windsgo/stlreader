#! /usr/bin/python3
import numpy as np
import sys
import os
import importlib
import matplotlib.pyplot as plt

## 这是c++导出库的python签名封装，使用python时import此文件即可

## USAGE START

'''
from stlreader import * # import stlreader module
try:
    reader = stlreader(filename, True)
    reader.save_contour(50, save_filename)
    contours : list = reader.get_contour_py(50)
except:
    pass
'''
## USAGE END

# to import stlreader
# dummy declare, implemented in cpp
class stlreader:
    def __init__(self, filename :str, print_enable :bool) -> None:
        pass
    def save_contour(self, z: float, filename: str) -> None:
        pass
    def get_contour_py(self, z: float) -> list:
        pass
    def save_indices(self, face_filename :str, vert_filename :str) -> None:
        pass
    def get_verts_py(self) -> np.ndarray:
        pass

script_path = sys.path[0] # 脚本路径
lib_path = os.path.abspath(script_path + "/../build/lib/")
sys.path.append(script_path)
sys.path.append(lib_path) # 增加导入库时的系统搜索路径
# 导入库Way1
stlreader = importlib.import_module('libstlread').stlreader
# 导入库Way2
# from libstlread import stlreader
