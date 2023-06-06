# Usage

- 对于Ubuntu低于20.04，CMAKE版本低于3.15的系统，该CMAKE构建方法可能出错
- 需要自行修改libboost_python以及libpython3的库搜索方法

- 对于Ubuntu20.04以上，请运行`install_essentials.sh`安装必须库

## How to build (and clean)

### Way 1

```bash
# clean
rm -rf build
rm src/*.so
# build
cmake -B build
cmake --build build -j8
# run test
./run_test.sh
```

### Way 2 (Recommanded)

```bash
./clean.sh # clean
./build.sh # build
./run_test.sh # run test
```

## Export for Python Use

- Copy the `libstlread.so` and `stlreader.py` to where you want to use it
- The usage of them is written in `test_import.py`.
- The example tree should be:
    
  ```
  test_folder
  ├─ stlreader.py
  ├─ libstlread.so
  └─ test_import.py
  ```

- codes in `test_import.py` should be like:
  ```python
  from stlreader import * # import stlreader module
  try:
    reader = stlreader(filename, True)
    reader.save_contour(50, save_filename)
    contours : list = reader.get_contour_py(50)
  except:
    pass
  ```

- run:
  ```bash
  python3 test_folder/test_import.py "./bunny_rough_binary.STL" 36
  ```