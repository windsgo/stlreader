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
    
    ## plot
    fig = plt.figure()
    
    ## 2d
    ax_2d = plt.axes()
    for group in x:
        ax_2d.plot(group[:,0], group[:,1])
    # 先保存一张二维图
    plt.title(stl_file + "_contour_py_" + str(z))
    plt.savefig(stl_file + "_contour_py_" + str(z) + ".png", dpi=900)
    plt.axis("equal")
    plt.show()
    
    ## 3d
    ax = plt.axes(projection='3d')
    # fig = plt.figure()
    # ax = fig.add_subplot(projection='3d')
    for group in x:
        # if len(fig.axes) < 1:
        #     fig.add_subplot(projection='3d')
        # ax = fig.gca()
        # ax1 = fig.add_subplot(ax)
        # print(type(ax) == type(ax1))
        ax.plot(group[:,0], group[:,1], group[:,2], linewidth=2.5, linestyle='solid')
    
    
    ori_points : np.ndarray = a.get_verts_py()
    print(ori_points.shape)
    
    ## 对原始数据筛选z附近
    delete_indexes = []
    delete_range : int = 50 # 设置绘图点在z附近的范围
    for i in range(ori_points.shape[0]):
        if ori_points[i,2] > z + delete_range or ori_points[i,2] < z - delete_range:
            delete_indexes.append(i)

    # delete_indexes = [] 
    m = np.delete(ori_points, delete_indexes, axis=0) # 一次性进行筛选，尚不明确这样删除的内存开销
    print(m.shape)
    down_s_ratio = 1 # 绘图间隔点，用于节省绘图开销
    # ax.scatter(m[::down_s_ratio,0], m[::down_s_ratio,1], m[::down_s_ratio,2], s=0.1, marker='.', alpha=0.4)
    ax.plot(m[::down_s_ratio,0], m[::down_s_ratio,1], m[::down_s_ratio,2], marker='.', alpha=1.0, linestyle="", markersize=1)
    
    try:
        plt.axis('tight')
        plt.axis('equal')
    except:
        pass
    plt.title(stl_file + "_contour_3d_py_" + str(z))
    plt.savefig(stl_file + "_contour_3d_py_" + str(z) + ".png", dpi=900)
    
    plt.show()