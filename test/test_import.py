from stlreader import *
  
if __name__ == "__main__":
    argc = len(sys.argv)
    if (argc > 3 or argc == 1):
        print("Usage python3 (dir/)test.py [filename] [z]")
        exit(1)
      
    stl_file = sys.argv[1]
    z = float(sys.argv[2])
    
    try:
        a = stlreader(stl_file, True) # use False here to disable print!
        print("stlreader success")
    except:
        print("read stl exception")
        exit(1)
    
    # a.save_indices("face.csv", "vert.csv")
    
    try:
        a.save_contour(z, stl_file + "_contour_" + str(z) + ".txt")
        print("save_contour success")
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