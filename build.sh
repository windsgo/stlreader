#! /bin/bash
echo -e "\033[33m Clean before build... \033[0m"
./clean.sh
echo -e "\033[33m Building... \033[0m"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8