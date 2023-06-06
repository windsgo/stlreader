#! /bin/bash
echo -e "\033[33m Running Python Test... \033[0m"
python3 test/test.py "bunny_rough_binary.STL" 36

echo -e "\033[33m Running CXX Test [gen_contour]... \033[0m"
build/bin/gen_contour "bunny_rough_binary.STL" 36

echo -e "\033[33m Running CXX Test [plot_contour]... \033[0m"
build/bin/plot_contour "bunny_rough_binary.STL" 36 40

echo -e "\033[33m Test Over \033[0m"