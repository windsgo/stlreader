#! /bin/bash
echo -e "\033[33m Running Python Test... \033[0m"
python3 src/test.py "R_V18.STL"

echo -e "\033[33m Running CXX Test [gen_contour]... \033[0m"
build/bin/gen_contour "R_V18.STL" 250

echo -e "\033[33m Running CXX Test [plot_contour]... \033[0m"
build/bin/plot_contour "R_V18.STL" 250 265

echo -e "\033[33m Test Over \033[0m"