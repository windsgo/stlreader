#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <iostream>
#include "STLReader.h"
#include <exception>
#include <vector>
#include <string>
#include <sstream>
using namespace stlreader;

int main(int argc, char** argv) {

    ::std::vector<float> z_vec;
    
    std::string filename = "../R_V18.STL";
    if (argc == 2) {
        filename = argv[1];
        z_vec.push_back(250);
    } else if (argc >= 3) {
        filename = argv[1];
        for (int i = 2; i < argc; ++i) {
            z_vec.push_back(std::stof(argv[i]));
        }
    } else {
        std::cerr << "Usage: gen_contour [filename] [z1] ([z2] ... )" << std::endl;
        return 1;
    }

    try {
        STLReader::ptr reader = std::make_shared<STLReader>(filename);

        for (auto z : z_vec) {
            ::std::stringstream ss;
            ss << filename << "_contour_" << z;
            auto c_list = reader->get_contour(z);
            for (auto& group : c_list) {
                auto n = group.size();
                std::vector<float> x, y;
                x.reserve(n);
                y.reserve(n);
                for (auto point : group) {
                    x.emplace_back(point.x);
                    y.emplace_back(point.y);
                }
                plt::plot(x, y);
            }
            plt::title(ss.str());
            // plt::legend();
            ss << ".png";
            plt::save(ss.str(), 600);
            plt::show();
        }

        // auto c_list = reader->get_contour(z);
        // size_t sum = 0;
        // for (auto& item : c_list) {
        //     sum += item.size();
        // }

        // std::cout << sum << std::endl;
        // std::cout << c_list.size() << std::endl;
    } catch (const STLReaderException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception" << std::endl;
    }

    return 0;
}