#! /usr/bin/python3
import numpy as np
import sys
import os
import shutil
import importlib
import matplotlib.pyplot as plt

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

## import
def import_cxxlib_module(lib_path :str, module_name: str):
    lib_path_without_libname :str = os.path.dirname(lib_path)
    # add search path
    sys.path.append(lib_path_without_libname) 
    # import
    try:
        return importlib.import_module(module_name) # import the module 'libstlread' as var:`stl_module`
    except Exception as e:
        print(e)
        exit(1)
    except:
        exit(1)

## main
if __name__ == "__main__":
    argc = len(sys.argv)
    if (argc > 3 or argc == 1):
        print("Usage python3 (dir/)test.py [filename] [z]")
        exit(1)
      
    stl_file = sys.argv[1]
    z = float(sys.argv[2])
    
    
    # if built by cmake
    # copy .so lib built by CMAKE
    '''
    script_path = sys.path[0]
    if (not os.path.exists(os.path.join(script_path, "libstlread.so"))):
        print(os.path.abspath(script_path + "/../build/lib"))
        built_lib = os.path.join(os.path.abspath(script_path + "/../build/lib"), "libstlread.so")
        if (os.path.exists(built_lib)):
            shutil.copy(built_lib, script_path)
        else:
            print("Build libstlread.so first!")
            exit(1)
    
    from libstlread import stlread
    '''
    
    ### new way to import 
    ### (recommanded)
    
    '''
    script_path = sys.path[0] # 脚本路径
    built_path = os.path.abspath(script_path + "/../build/lib") # 构建出的libstlread.so库路径
    # 检查是否已构建(不是必要的)
    built_lib = os.path.join(built_path, "libstlread.so")
    if (not os.path.exists(built_lib) and not os.path.exists("libstlread.so")):
        print("Build libstlread.so first!")
        exit(1)
    # add search path
    sys.path.append(built_path) 
    # import
    try:
        stl_module = importlib.import_module('libstlread') # import the module 'libstlread' as var:`stl_module`
        stlreader = stl_module.stlreader # rename function: `libstlread.stlread` as var `stlread`
    except Exception as e:
        print(e)
        exit(1)
    except:
        exit(1)
    '''
    
    ## import lib
    script_path = sys.path[0] # 脚本路径
    lib_path = os.path.abspath(script_path + "/../build/lib/libstlread.so")
    print(lib_path)
    stlreader = import_cxxlib_module(lib_path, 'libstlread').stlreader
    
    ## example to use the cxx lib in python
    
    try:
        a = stlreader(stl_file, True) # use False here to disable print!
    except:
        print("read stl exception")
        exit(1)
    
    # a.save_indices("face.csv", "vert.csv")
    
    try:
        a.save_contour(z, stl_file + "_contour_" + str(z) + ".txt")
    except:
        print("save_contour exception")
        exit(1)

    # example to directly get data
    try:
        x : list = a.get_contour_py(z)
    except:
        print("get_contour_py exception")
        exit(1)

    # test    
    print("等高线段数:", len(x)) # 等高线段数
    
    if len(x) == 0:
        exit(1)

    first_group : np.ndarray = x[0] # 获取第一段等高线
    print("等高线数据类型:", type(first_group)) # 每一段是一个numpy二维数组
    print("一段等高线的数组维度:", first_group.shape) # 维度

    first_point_in_first_group : np.ndarray = first_group[0] # 第一段等高线的第一个点
    print("点数据类型:", type(first_point_in_first_group)) 
    print("点的数组维度:", first_point_in_first_group.shape) 

    print(first_group.size/3)
    
    ## ploting
    fig, ax = plt.subplots()
    
    for group in x:
        ax.plot(group[:,0], group[:,1])
    
    plt.title(stl_file + "_contour_" + str(z))
    plt.axis('equal')
    
    plt.show()