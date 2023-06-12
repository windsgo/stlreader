#include "STLReader.h"
#include <cassert>
#include <iostream>
#include <chrono>
#include <sstream>

namespace stlreader
{

STLReader::STLReader(const ::std::string& stl_file, bool print)
    : _stl_file(stl_file), _print(print)
{
    _init();
}

void STLReader::_init()
{
    _stl_read_progress.reset();
    _shell_progress.reset();
    if (_print)
        std::cout << "Reading Stl..." << std::endl;

    _face_vec.clear();
    _vert_vec.clear();
    _edge_vec.clear();
    // _vert_set.clear();
    _edge_to_be_handled_set.clear();

    ::std::ifstream ifs(_stl_file, ::std::ios::binary);
    if (!ifs.is_open())
    {
        throw STLReaderException("open stl binary file error");
    }

    STLBinaryHeaderBlock_t header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(STLBinaryHeaderBlock_t));

    STLBinaryFaceNumber_t face_number;
    ifs.read(reinterpret_cast<char*>(&face_number), sizeof(STLBinaryFaceNumber_t));

    _stl_read_progress.total_num = face_number;

    _face_vec.reserve(face_number);
    _edge_vec.reserve(face_number * 3);
    _vert_vec.reserve(face_number / 2); // estimated volume

    // size_t read_ms = 0;
    // size_t vert_delete_ms = 0;
    // size_t edge_ms = 0;

    for (uint32_t i = 0; i < face_number; ++i)
    {
        // auto t1 = ::std::chrono::high_resolution_clock::now();
        STLBinaryFaceBlock_t face_block;
        ifs.read(reinterpret_cast<char*>(&face_block), sizeof(STLBinaryFaceBlock_t));
        // auto t2 = ::std::chrono::high_resolution_clock::now();
        // read_ms += ::std::chrono::duration_cast<::std::chrono::milliseconds>(t2 - t1).count();

        Face::ptr face = std::make_shared<Face>();
        for (int j = 0; j < 3; ++j)
        {
            // std::cout << "vert:j=" << j << std::endl;
            Vert::ptr vert = std::make_shared<Vert>(face_block.vertex[j]);

            if (vert->point.z == 354) {
                std::cout << vert->point.x << ", " << vert->point.y 
                << ", " << vert->point.z << std::endl;
            }

            // std::cout << *vert << std::endl;
            // auto exist_vert = _vert_set.find(*vert);
            // if (exist_vert == _vert_set.end()) {
            //     vert->index = _vert_vec.size();
            //     _vert_set.insert(*vert);
            //     _vert_vec.push_back(vert);
            // } else {
            //     // std::cout << *exist_vert << std::endl;
            //     vert = _vert_vec[exist_vert->index];
            //     assert(vert->index == exist_vert->index);
            // }

            /*
                test begin           
            */

            auto itr = _vert_umap.find(*vert);
            if (itr == _vert_umap.end()) {
                _vert_umap.insert(std::make_pair(*vert, std::weak_ptr<Vert>(vert)));
                _vert_vec.push_back(vert);
            } else {
                vert = itr->second.lock();
            }

            /*
                test end
            */


            // now `vert` is a pointer to a unique vertex in the `_vert_vec`
            face->v[j] = vert;
        }

        _face_vec.push_back(face);
        // auto t3 = ::std::chrono::high_resolution_clock::now();
        // vert_delete_ms += ::std::chrono::duration_cast<::std::chrono::milliseconds>(t3 - t2).count();

        for (int j = 0; j < 3; ++j)
        {
            Edge::ptr edge = std::make_shared<Edge>();
            edge->face = face;
            edge->v_start = face->v[j];
            edge->v_end = face->v[(j + 1)%3];
            face->v[j].lock()->out_edge_set.insert(edge);
            face->e[j] = edge;
            _edge_vec.push_back(edge);
        }

        for (int j = 0; j < 3; ++j)
        {
            face->e[j].lock()->e_next = face->e[(j + 1)%3];
            for (const auto& edge: face->e[j].lock()->v_end.lock()->out_edge_set) {
                assert(edge->v_start.lock() == face->e[j].lock()->v_end.lock());
                if (edge->v_end.lock() == face->e[j].lock()->v_start.lock()) {
                    if (edge->e_pair.lock()) {
                        _is_manifold_mesh = false; 
                        continue; // 防止覆盖之前确立的pair对应关系
                    } // 已经有对边了，表示非流形边
                    edge->e_pair = face->e[j];
                    face->e[j].lock()->e_pair = edge;
                    break;
                }
            }
        }

        // auto t4 = ::std::chrono::high_resolution_clock::now();
        // edge_ms += ::std::chrono::duration_cast<::std::chrono::milliseconds>(t4 - t3).count();

        // std::cin.get();

        _stl_read_progress.cur_num = i + 1;
        _update_progress(_stl_read_progress);
        
    }

    ifs.close();
    if (_print) {
        std::cout << std::endl;


        std::cout << "_face_vec" << _face_vec.size() << std::endl;
        // std::cout << _vert_set.size() << std::endl;
        std::cout << "_vert_vec" << _vert_vec.size() << std::endl;
        std::cout << "_edge_vec" << _edge_vec.size() << std::endl;
        std::cout << "Read Stl Done." << std::endl;
    }

    // std::cout << "time:" << std::endl;
    // std::cout << read_ms << std::endl;
    // std::cout << vert_delete_ms << std::endl;
    // std::cout << edge_ms << std::endl;

    // _vert_set.clear();
    _vert_umap.clear();
}

void STLReader::save_indices(const ::std::string& face_file, const ::std::string& vert_file, size_t start_index)
{
    ::std::ofstream ofs_face(face_file);
    if (!ofs_face.is_open()) {
        throw STLReaderException("open face_file failed");
    }
    // ::std::stringstream ss;
    for (const auto &face : _face_vec) {
        for (int i = 0; i < 3; ++i) 
            ofs_face << face->v[i].lock()->index + start_index << (i == 2 ? '\n' : ',');
    }
    ofs_face.close();

    ::std::ofstream ofs_vert(vert_file);
    if (!ofs_vert.is_open()) {
        throw STLReaderException("open vert_file failed");
    }
    // ::std::stringstream ss;
    for (const auto &vert : _vert_vec) {
        ofs_vert << vert->point.x << ',';
        ofs_vert << vert->point.y << ',';
        ofs_vert << vert->point.z << '\n';
    }
    ofs_vert.close();
}

std::list<std::list<PointXYZ> > STLReader::get_contour(float z)
{
    _edge_to_be_handled_set.clear();
    _contour_gen_progress.reset();
    _shell_progress.reset();
    if (_print)
        std::cout << "get_contour start (z = " << z << " ) ..." << std::endl;

    std::list<std::list<PointXYZ>> crossed_loop_list;

    // find every edge that crossed
    for (const auto& edge : _edge_vec) {
        if (_is_edge_crossed(edge, z)) {
            _edge_to_be_handled_set.insert(edge);
        }
    }

    _contour_gen_progress.total_num = _edge_to_be_handled_set.size();

    while (!_edge_to_be_handled_set.empty()) {
        crossed_loop_list.push_back(_contour_handle_one_circle(z, *_edge_to_be_handled_set.begin()));
        _contour_gen_progress.cur_num = _contour_gen_progress.total_num - _edge_to_be_handled_set.size();
        _update_progress(_contour_gen_progress);
    }

    if (_print) {
        std::cout << std::endl;
        std::cout << "get_contour done." << std::endl;
    }

    return crossed_loop_list;
}

std::list<PointXYZ> STLReader::_contour_handle_one_circle(float z, Edge::ptr start_edge)
{
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    // std::cout << start_edge->v_start.lock()->point.x << std::endl;
    // std::cout << start_edge->v_start.lock()->point.y << std::endl;
    // std::cout << start_edge->v_end.lock()->point.x << std::endl;
    // std::cout << start_edge->v_end.lock()->point.y << std::endl;
    // std::cout << _edge_to_be_handled_set.size() << std::endl;
    // std::cout << _edge_to_be_handled_set.empty() << std::endl;

    std::list<PointXYZ> crossed_verts_list;

    if (!_is_edge_crossed(start_edge, z)) {
        start_edge = start_edge->e_next.lock();
    }
    auto cur_edge = start_edge;

    auto submit_crossd_edge = [&](Edge::ptr edge) {
        if (!_is_edge_crossed(edge, z)) {
            throw STLReaderException("edge should be crossed but not!");
        }
        // assert(_is_edge_crossed(edge, z));
        crossed_verts_list.push_back(_get_crossed_point(edge, z));
        auto ret = _edge_to_be_handled_set.erase(edge);
        if (ret != 1) {
            throw STLReaderException("erase failed");
        }
    };

    submit_crossd_edge(cur_edge);

    bool flag_search_over = false;
    bool flag_searching_another_side = false;

    while (!flag_search_over) {
        
        // in this case, 
        // the first edge of the face (cur_edge) always crossed
        // and the crossed point was submitted to the list
        // go to the second edge of the face
        cur_edge = cur_edge->e_next.lock();

        if (!_is_edge_crossed(cur_edge, z)) {
            // if 2nd not crossed, go to the 3rd, which must cross
            cur_edge = cur_edge->e_next.lock();
        } else {
            /* 有可能有三个边都判定交点的情况，把第三条边也从set中剔除 */
            auto third_edge = cur_edge->e_next.lock();
            if (_is_edge_crossed(third_edge, z)) {
                _edge_to_be_handled_set.erase(third_edge);
            }
        }

        submit_crossd_edge(cur_edge);

        // try to go to the next face
        if (cur_edge->e_pair.lock()) {
            cur_edge = cur_edge->e_pair.lock();
            _edge_to_be_handled_set.erase(cur_edge);
        } else {
            // no next face
            if (!flag_searching_another_side && !start_edge->e_pair.expired()) {
                // 如果还没搜索另一侧，且另一侧可以继续查询，就调转方向
                flag_searching_another_side = true;
                // search another side
                cur_edge = start_edge->e_pair.lock();

                if (_edge_to_be_handled_set.find(cur_edge) == _edge_to_be_handled_set.end()) {
                    throw STLReaderException("pair edge should be in the `_edge_to_be_handled_set` but not");
                }
                if (!_is_edge_crossed(cur_edge, z)) {
                    throw STLReaderException("pair edge should be crossed but not");
                }

                _edge_to_be_handled_set.erase(cur_edge);
                // if (!_is_edge_crossed(cur_edge, z)) {
                //     cur_edge = cur_edge->e_next;
                // }
                crossed_verts_list.reverse();
            } else {
                flag_search_over = true;
            }
        }

        if (cur_edge->face.lock() == start_edge->face.lock()) {
            break;
        }
    }

    // std::cout << "crossed_verts_list" << crossed_verts_list.size() << std::endl;
    // std::cout << "left size" << _edge_to_be_handled_set.size() << std::endl;

    return crossed_verts_list;
}

bool STLReader::_is_edge_crossed(const Edge::ptr& edge, float z) const
{
    return ((edge->v_start.lock()->point.z - z) * (edge->v_end.lock()->point.z - z) <= 0);
}

PointXYZ STLReader::_get_crossed_point(const Edge::ptr& edge, float z) const
{
    float tmp = (z - edge->v_start.lock()->point.z) / (edge->v_end.lock()->point.z - edge->v_start.lock()->point.z);
    return {edge->v_start.lock()->point.x + tmp * (edge->v_end.lock()->point.x - edge->v_start.lock()->point.x),
            edge->v_start.lock()->point.y + tmp * (edge->v_end.lock()->point.y - edge->v_start.lock()->point.y),
            z};
}

void STLReader::save_contour(float z, const ::std::string& contour_file) {
    auto list_list = get_contour(z);
    ::std::ofstream ofs(contour_file);
    if (!ofs.is_open()) {
        throw STLReaderException("open contour file failed");
    }

    size_t group = 0;
    for (const auto &list : list_list) {
        ofs << '#' << group << '\n';
        for (const auto& vert : list) {
            ofs << vert.x << ',' << vert.y << ',' << vert.z << '\n';
        }
        ++group;
    }

    ofs.close();
}

#ifdef PYTHON_EXPORT
py::list STLReader::get_contour_py(float z)
{
    np::initialize();
    auto cxx_list = get_contour(z);
    py::list py_list;
    for (const auto &group : cxx_list) {
        ::std::size_t point_num = group.size();

        py::tuple shape = py::make_tuple(point_num, 3);
        np::dtype type = np::dtype::get_builtin<float>();
        np::ndarray points = np::zeros(shape, type);

        ::std::size_t i = 0;
        for (const auto& vert : group) {
            points[i][0] = vert.x;
            points[i][1] = vert.y;
            points[i][2] = vert.z;
            ++i;
        }
        py_list.append(points);
    }

    return py_list;
}

np::ndarray STLReader::get_verts_py() const
{
    np::initialize();
    py::tuple shape = py::make_tuple(_vert_vec.size(), 3);
    np::dtype type = np::dtype::get_builtin<float>();
    np::ndarray points = np::zeros(shape, type);
    ::std::size_t i = 0;
    for (const auto& vert : _vert_vec) {
        points[i][0] = vert->point.x;
        points[i][1] = vert->point.y;
        points[i][2] = vert->point.z;
        ++i;
    }

    return points;
}

#endif

void STLReader::_update_progress(const STLReader::Progress_t& progress) {
    if (_print) {
        _shell_progress.setProgress(progress.progress());
        std::cout << _shell_progress;
    }
}

}

std::ostream& operator<<(std::ostream& os, const stlreader::Vert& vert)
{
    os << "[Vert] " << vert.index << ";[" << vert.point.x << "," << vert.point.y << "," << vert.point.z << "]\t";
    os << " " << vert.out_edge_set.size();
    return os;
}

#ifdef PYTHON_EXPORT
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(save_indices_overloads, stlreader::STLReader::save_indices, 2, 3)

BOOST_PYTHON_MODULE(libstlread)
{
    class_<stlreader::STLReader>("stlreader", boost::python::init<const ::std::string &, boost::python::optional<bool> >())
        .def("save_indices", &stlreader::STLReader::save_indices, save_indices_overloads())
        .def("save_contour", &stlreader::STLReader::save_contour)
        .def("get_contour_py", &stlreader::STLReader::get_contour_py)
        .def("set_print", &stlreader::STLReader::set_print)
        .def("get_verts_py", &stlreader::STLReader::get_verts_py);
}
#endif