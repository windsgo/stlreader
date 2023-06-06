#pragma once

#include <fstream>
#include <string>
#include <exception>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include <functional>
#include <vector>
#include <iostream>
#include <list>
#include <unordered_map>

#include <functional>

#include "ShellProgress.h"

// #define PYTHON_EXPORT

#ifdef PYTHON_EXPORT
#include <boost/python.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/numpy.hpp>
namespace py = boost::python;
namespace np = boost::python::numpy;
#endif

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    seed ^= std::hash<T>()(val)+0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T>
inline void hash_val(std::size_t& seed, const T& val)
{
    hash_combine(seed, val);
}

template<typename T, typename... Types>
inline void hash_val(std::size_t& seed, const T& val, const Types&... args)
{
    hash_combine(seed, val);
    hash_val(seed, args...);
}

template<typename... Types>
inline std::size_t hash_val(const Types& ...args)
{
    std::size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

namespace stlreader {

class STLReaderException : ::std::exception
{
public:
    STLReaderException() : _str("STLReaderException") {}
    STLReaderException(const ::std::string& str) : _str(str) {}
    virtual const char* what() const throw() override {
        return _str.c_str();
    }
private:
    ::std::string _str;
};

struct __attribute__((__packed__)) STLBinaryHeaderBlock_t
{
    char str[80];
};

typedef uint32_t STLBinaryFaceNumber_t;

struct __attribute__((__packed__)) STLBinaryVertexBlock_t
{
    float x; float y; float z;
};

struct __attribute__((__packed__)) STLBinaryFaceBlock_t
{
    float normal_x;
    float normal_y;
    float normal_z;
    STLBinaryVertexBlock_t vertex[3];
    uint8_t _reserved[2];
};

// forward declaration
struct Vert;
struct Face;
struct Edge;

struct PointXYZ
{
    PointXYZ(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float x;
    float y;
    float z;
    bool operator==(const PointXYZ &rhs) const {
        return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
    }
};

struct Vert
{
    using ptr = std::shared_ptr<Vert>;
    Vert(float x_, float y_, float z_) : point(x_, y_, z_) {}
    Vert(const STLBinaryVertexBlock_t& bin_vert) : point(bin_vert.x, bin_vert.y, bin_vert.z) {}
    PointXYZ point;
    int32_t index = -1;
    ::std::unordered_set<std::shared_ptr<Edge> > out_edge_set;
    bool operator==(const Vert &rhs) const {
        return (point == rhs.point);
    }
};

struct Face
{
    using ptr = std::shared_ptr<Face>;
    std::weak_ptr<Vert> v[3];
    std::weak_ptr<Edge> e[3];
};

struct Edge
{
    using ptr = std::shared_ptr<Edge>;
    std::weak_ptr<Vert> v_start; // 使用weak_ptr不获取所有权，避免点、面、边之间的回环引用，导致内存泄露问题
    std::weak_ptr<Vert> v_end;
    std::weak_ptr<Edge> e_next;
    std::weak_ptr<Edge> e_pair;
    std::weak_ptr<Face> face;

    // 如果非流形
};

}

namespace std
{
    template<>
    struct hash<stlreader::Vert>
    {
        size_t operator() (const stlreader::Vert& v) const noexcept {
            return hash_val(v.point.x, v.point.y, v.point.z);
        }
    };
}

namespace stlreader
{

class STLReader
{
public:
    using ptr = std::shared_ptr<STLReader>;
    STLReader(const ::std::string& stl_file, bool print = true);

    void set_print(bool print) {_print = print;}

    // default save vertices as float type
    // 6位有效数字
    void save_indices(const ::std::string& face_file, const ::std::string& vert_file, size_t start_index = 1);

    // 获取等高线,可能有多个断开的曲线段或闭环段,每段用一个list
    std::list<std::list<PointXYZ> > get_contour(float z);
    void save_contour(float z, const ::std::string& contour_file);

    // 获取进度接口
    bool stl_read_finished()        const {return _stl_read_progress.finished();}
    float stl_read_progress()       const {return _stl_read_progress.progress();}
    bool contour_gen_finished()     const {return _contour_gen_progress.finished();}
    float contour_gen_progress()    const {return _contour_gen_progress.progress();}

#ifdef PYTHON_EXPORT
    py::list get_contour_py(float z);
#endif

private:
    void _init();
    std::list<PointXYZ> _contour_handle_one_circle(float z, Edge::ptr start_edge);
    inline bool _is_edge_crossed(const Edge::ptr& edge, float z) const;
    inline PointXYZ _get_crossed_point(const Edge::ptr& edge, float z) const;
private:
    ::std::string _stl_file;

    bool _is_manifold_mesh = true; // 流形曲面，主要由是否一条边被大于2个面共享决定

    ::std::unordered_map<Vert, std::weak_ptr<Vert> > _vert_umap;

    // ::std::unordered_set<Vert> _vert_set; // used for simplify vert indices
    ::std::vector<Face::ptr> _face_vec;
    ::std::vector<Edge::ptr> _edge_vec;
    ::std::vector<Vert::ptr> _vert_vec;

    ::std::unordered_set<Edge::ptr> _edge_to_be_handled_set; // used for handle multiple contour loops

    struct Progress_t {
        size_t total_num = 0;
        size_t cur_num = 0;
        void reset() {total_num = 0; cur_num = 0;}
        float progress() const {return total_num == 0 ? 0 : (float)cur_num / (float)total_num;}
        bool finished() const {return total_num == 0 ? false : (cur_num == total_num);}
    };

    // 提供读取stl生成数据结构的进度和标志
    Progress_t _stl_read_progress;

    // 等高线生成进度
    Progress_t _contour_gen_progress;

    void _update_progress(const Progress_t&);

    ShellProgress _shell_progress;

    bool _print = true;
};

}

std::ostream& operator<<(std::ostream& os, const stlreader::Vert& vert);