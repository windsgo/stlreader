#! /bin/bash
echo -e "\033[33m Cleaning... \033[0m"
rm -f src/*.so
rm -f src/*.a
rm -f test/*.so
rm -f src/gen_contour
rm -f src/plot_contour
rm -f *.png
rm -f src/*.png

# cmake
rm -rf build

# txt

echo PWD:$PWD

function FileSuffix() {
    local filename="$1"
    if [ -n "$filename" ]; then
        echo "${filename##*.}"
    fi
}

function IsSuffix() {
    local filename="$1"
    local suffix="$2"
    if [ "$(FileSuffix ${filename})" = "$suffix" ]; then
        return 0
    else
        return 1
    fi
}

for file in $PWD/*; do

    
    filename=${file##*/}
    # echo ${filename}

    IsSuffix ${filename} "txt"
    ret=$?
    if [ $ret -eq 0 ]; then
        # echo "the suffix of the ${file} is txt"
        if [ $filename != "CMakeLists.txt" ]; then
            echo -e "\033[33m Deleting ${filename}... \033[0m"
            rm $filename
        fi
    fi

done